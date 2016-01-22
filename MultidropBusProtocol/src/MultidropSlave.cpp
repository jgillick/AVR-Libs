
#include "MultidropSlave.h"
#include <util/crc16.h>

#define SOM 0xff

MultidropSlave::MultidropSlave(MultidropData *_serial) : Multidrop(_serial) {
  flags = 0;
  myAddress = 0;
  parseState = NO_MESSAGE;
}

uint8_t MultidropSlave::isReady() {
  return parseState == MESSAGE_READY;
}

void MultidropSlave::reset() {
  parseState = NO_MESSAGE;
}

uint8_t MultidropSlave::inBatchMode() {
  return flags & BATCH_FLAG;
}

uint8_t MultidropSlave::isResponseMessage() {
  return flags & RESPONSE_MESSAGE_FLAG;
}

uint8_t* MultidropSlave::getData() {
  return dataBuffer;
}

uint8_t MultidropSlave::getDataLen() {
  return dataIndex;
}

uint8_t MultidropSlave::getCommand() {
  return command;
}

void MultidropSlave::setAddress(uint8_t addr) {
  myAddress = addr;
}

void MultidropSlave::startMessage() {
  flags = 0;
  length = 0;
  address = 0;
  dataIndex = 0;
  dataBuffer[0] = '\0';
  fullDataLength = 0;
  fullDataIndex = 0;
  dataStartOffset = 0;

  messageCRC = ~0;
  messageCRC = _crc16_update(messageCRC, SOM);
  messageCRC = _crc16_update(messageCRC, SOM);
}

uint8_t MultidropSlave::read() {

  receiveMode();
  while (serial->available()) {
    if(parse(serial->read()) == 1) {
      return 1;
    }
  }
  return 0;
}

uint8_t MultidropSlave::parse(uint8_t b) {

  if (parseState == HEADER_SECTION) {
    parseHeader(b);
  }
  else if (parseState == DATA_SECTION) {
    processData(b);
  }
  // Validate CRC
  else if (parseState == END_SECTION) {
    uint8_t crcByte;

    if (parsePos == DATA_POS) { // First byte
      crcByte = (messageCRC >> 8) & 0xFF;
      parsePos = EOM1_POS;
    }
    else { // Second byte
      crcByte = messageCRC & 0xff;
      parsePos = EOM2_POS;
    }

    // Validate each byte
    if (crcByte != b) {
      parseState = NO_MESSAGE; // no match, abort
    }
    else if (parsePos == EOM2_POS) {
      parseState = MESSAGE_READY;
      return 1;
    } 
  }
  else if (parseState == START_SECTION) {
    if (b == SOM) {
      startMessage();
      parsePos = SOM2_POS;
      parseState = HEADER_SECTION;
    }
    // No second 0xFF, so invalid start to message
    else {
      parseState = NO_MESSAGE;
    }
  }
  else if (parseState == NO_MESSAGE && b == SOM) {
    parsePos = SOM1_POS;
    parseState = START_SECTION;
  }

  return 0;
}

void MultidropSlave::parseHeader(uint8_t b) {
  messageCRC = _crc16_update(messageCRC, b);

  // Header flags
  if (parsePos == SOM2_POS) {
    parsePos = HEADER_FLAGS_POS;
    flags = b;
  }
  // Address
  else if (parsePos == HEADER_FLAGS_POS) {
    parsePos = HEADER_ADDR_POS;
    address = b;
  }
  // Command
  else if (parsePos == HEADER_ADDR_POS) {
    parsePos = HEADER_CMD_POS;
    command = b;
  }
  // Length, 1st byte
  else if (parsePos == HEADER_CMD_POS) {
    parsePos = HEADER_LEN1_POS;

    // in batch mode, the first length byte is the number of nodes
    if (inBatchMode()) {
      numNodes = b;
    } else {
      length = b;
      fullDataLength = b;
      dataStartOffset = 0;
      parseState = DATA_SECTION;
    }
  }
  // Length, 2nd byte (if in batch mode)
  else if (parsePos == HEADER_LEN1_POS) {
    length = b;
    fullDataLength = length * numNodes;
    dataStartOffset = (myAddress - 1) * length; // Where our data starts in the message

    parsePos = HEADER_LEN2_POS;
    parseState = DATA_SECTION;
  }

  // No data, continue to CRC
  if (parseState == DATA_SECTION && length == 0) {
    parseState = END_SECTION;
  }
}

void MultidropSlave::processData(uint8_t b) {
  messageCRC = _crc16_update(messageCRC, b);
  parsePos = DATA_POS;
  fullDataIndex++;

  // If we're still in our data section, fill data buffer
  if (fullDataIndex >= dataStartOffset && dataIndex < length){
    dataBuffer[dataIndex++] = b;
    dataBuffer[dataIndex] = '\0';
  }
  // Done with data
  else if (fullDataIndex >= fullDataLength) {
    parseState = END_SECTION;
  }
}
