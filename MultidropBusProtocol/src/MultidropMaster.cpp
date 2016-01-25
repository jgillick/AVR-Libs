
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

void MultidropMaster::setResponseSettings(uint8_t *buff, uint16_t time, uint16_t timeout, uint8_t *defaultResponse) {
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

uint8_t MultidropMaster::checkForResponses(uint16_t time) {
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

  return (waitingOnNodes == 0);
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

  sendByte((messageCRC >> 8) & 0xFF, false);
  sendByte(messageCRC & 0xff, false);

  state = EOM;
  return 1;
}

