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

uint8_t MultidropMaster::startMessage(uint8_t command, 
                                      uint8_t destination, 
                                      uint8_t dataLength,
                                      uint8_t batchMode, 
                                      uint8_t responseMessage) {
  
  state = 0;
  messageCRC = ~0;

  uint8_t flags = 0;
  if (batchMode) {
    flags |= BATCH_FLAG;
  }
  if (responseMessage) {
    flags |= RESPONSE_MESSAGE_FLAG;
  }

  // Start sending header
  sendMode();
  sendByte(0xFF);
  sendByte(0xFF);
  sendByte(flags);
  sendByte(destination);
  sendByte(command);

  // Length
  if (batchMode) {
    sendByte(nodeNum);
  }
  sendByte(dataLength);
  
  receiveMode();

  state = HEADER_SENT;
  return 1;
}

uint8_t MultidropMaster::sendData(uint8_t d) {
  if (state == EOM) return 0;

  sendMode();
  sendByte(d);
  receiveMode();

  state = DATA_SENDING;
  return 1;
}

uint8_t MultidropMaster::sendData(uint8_t *data, uint16_t len) {
  if (state == EOM) return 0;

  sendMode();
  for(uint16_t i = 0; i < len; i++) {
    sendData(data[i]);
  }
  receiveMode();
  
  return 1;
}

uint8_t MultidropMaster::finishMessage() {
  if (state == EOM) return 0;

  sendMode();
  sendByte((messageCRC >> 8) & 0xFF, false);
  sendByte(messageCRC & 0xff, false);
  receiveMode();

  state = EOM;
  return 1;
}

