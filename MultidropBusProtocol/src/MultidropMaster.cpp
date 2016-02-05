
#include <util/crc16.h>

#include "MultidropMaster.h"

#define BATCH_FLAG            0b00000001
#define RESPONSE_MESSAGE_FLAG 0b00000010

MultidropMaster::MultidropMaster(MultidropData *serial) : Multidrop(serial) {
  state = EOM;
  nodeNum = 0;
}

void MultidropMaster::setNodeLength(uint8_t num) {
  nodeNum = num;
}

void MultidropMaster::addNextDaisyChain(volatile uint8_t next_pin_num,
                                    volatile uint8_t* next_ddr_register, 
                                    volatile uint8_t* next_port_register, 
                                    volatile uint8_t* next_pin_register) {

  d1_num  = next_pin_num;
  d1_ddr  = next_ddr_register;
  d1_port = next_port_register;
  d1_pin  = next_pin_register;

  *d1_ddr &= ~(1 << d1_num);
  *d1_port &= ~(1 << d1_num);

  daisy_prev = 0;
  daisy_next = 1;
}

uint8_t MultidropMaster::startMessage(uint8_t command,
                                      uint8_t destinationAddr,
                                      uint8_t dataLen,
                                      uint8_t batchMode,
                                      uint8_t responseMessage) {

  state = 0;
  messageCRC = ~0;
  dataLength = dataLen;
  destAddress = destinationAddr;

  uint8_t flags = 0;
  if (batchMode) {
    flags |= BATCH_FLAG;
  }
  if (responseMessage) {
    flags |= RESPONSE_MESSAGE_FLAG;
  }

  // Start sending header
  sendByte(0xFF);
  sendByte(0xFF);
  sendByte(flags);
  sendByte(destAddress);
  sendByte(command);

  // Length
  if (batchMode) {
    sendByte(nodeNum);
  }
  sendByte(dataLength);

  state = HEADER_SENT;
  return 1;
}

void MultidropMaster::startAddressing(uint32_t t, uint32_t timeout) {
  nodeNum = 0;
  lastAddressReceived = 0x00;
  nodeAddressTries = 0;
  timeoutDuration = timeout;
  timeoutTime = t + timeoutDuration;

  // Send reset message
  startMessage(CMD_RESET, BROADCAST_ADDRESS);
  finishMessage();

  // Start address message
  startMessage(CMD_ADDRESS, BROADCAST_ADDRESS, 2, true, true);
  state = ADDRESSING;
  setNextDaisyValue(1);
  sendByte(0x00);
}

void MultidropMaster::setResponseSettings(uint8_t *buff, uint32_t time, uint32_t timeout, uint8_t *defaultResponse) {
  responseIndex = 0;
  responseBuff = buff;
  timeoutDuration = timeout;
  defaultResponseValues = defaultResponse;
  timeoutTime = time + timeoutDuration;

  if (destAddress != BROADCAST_ADDRESS) {
    waitingOnNodes = nodeNum;
  } else {
    waitingOnNodes = 1;
  }

  checkForResponses(time);
}

uint8_t MultidropMaster::checkForResponses(uint32_t time) {
  uint8_t b, i;

  // Received all responses
  if (waitingOnNodes == 0) {
    return 1;
  }

  // Get more responses
  while (serial->available()) {
    b = serial->read();
    responseBuff[responseIndex] = b;
    messageCRC = _crc16_update(messageCRC, b);

    responseIndex++;
    timeoutTime = time + timeoutDuration;

    // Have we received all the data for this node?
    if (responseIndex % dataLength == 0) {
      waitingOnNodes--;
    }
  }

  // Node timeout, send default response
  if (time > timeoutTime) {

    // It's possible the node sent a partial response, so send whatever is left
    for (i = responseIndex % dataLength; i < dataLength; i++) {
      sendByte(defaultResponseValues[i]);
      responseBuff[responseIndex] = defaultResponseValues[i];
      responseIndex++;
    }

    timeoutTime = time + timeoutDuration;
    waitingOnNodes--;
  }

  if (waitingOnNodes == 0) {
    finishMessage();
    return true;
  }
  return false;
}

MultidropMaster::adr_state_t MultidropMaster::checkForAddresses(uint32_t time) {
  uint8_t b;
  
  if (state != ADDRESSING) return ADR_DONE; 

  // If master's prev daisy chain is HIGH, then the network has gone full circle
  if (getPrevDaisyChainValue() == 1) {
    finishMessage();
    return ADR_DONE;
  }

  // Receive next address
  while (serial->available()) {
    b = serial->read();
    timeoutTime = (time + timeoutDuration);

    // Verify it's 1 larger than the last address and send confirmation
    if (b == lastAddressReceived + 1) { 
      nodeNum++;
      lastAddressReceived = b;
      nodeAddressTries = 0;
      sendByte(b);
    }
    // Invalid address
    else { 
      // Max tries, end in error
      nodeAddressTries++;
      if (nodeAddressTries > MD_MASTER_ADDR_MAX_TRIES) {
        return ADR_ERROR;
      }
      // Send last valid address again
      else {
        sendByte(lastAddressReceived);
      }
    }
  }

  // Node timeout, try again or finish
  if (time > timeoutTime) {
    nodeAddressTries++;
    if (nodeAddressTries <= MD_MASTER_ADDR_MAX_TRIES) {
      sendByte(lastAddressReceived);
      serial->flush();
      timeoutTime = (time + timeoutDuration);
    }
    else {
      finishMessage();
      if (nodeNum > 0) {
        return ADR_DONE;
      } else {
        return ADR_ERROR;
      }
    } 
  }

  // Max nodes
  if (lastAddressReceived == 255) {
    finishMessage();
    return ADR_DONE;
  }
  return ADR_WAITING;
}

uint8_t MultidropMaster::sendData(uint8_t d) {
  if (state == EOM) return 0;

  sendByte(d);
  state = DATA_SENDING;
  return 1;
}

uint8_t MultidropMaster::sendData(uint8_t *data, uint16_t len) {
  if (state == EOM) return 0;

  for(uint16_t i = 0; i < len; i++) {
    sendData(data[i]);
  }

  return 1;
}

void MultidropMaster::sendByte(uint8_t b, uint8_t updateCRC) {
  serial->write(b);
  if (updateCRC) {
    messageCRC = _crc16_update(messageCRC, b);
  }
}

uint8_t MultidropMaster::finishMessage() {
  if (state == EOM) return 0;

  if (state == ADDRESSING) {
    sendByte(0x00);
    sendByte(0x00);
  }
  else {
    sendByte((messageCRC >> 8) & 0xFF, false);
    sendByte(messageCRC & 0xff, false);
  }

  state = EOM;
  return 1;
}

