

#ifndef MultidropMaster_H
#define MultidropMaster_H

#include <avr/io.h>
#include <stdint.h>
#include "Multidrop.h"

class MultidropMaster: public Multidrop {

public:
  MultidropMaster(MultidropData *_serial);

  // Set the number of nodes on the bus
  void setNodeLength(uint8_t);

  // Start a new message to send
  uint8_t startMessage(uint8_t command, 
                      uint8_t destination=BROADCAST_ADDRESS, 
                      uint8_t dataLength=0,
                      uint8_t batchMode=0, 
                      uint8_t responseMessage=0);

  // Call this regularly in order for master to keep 
  // accurate time when waiting for responses.
  void setTime(uint16_t time);

  // Set the default response if a node is quiet for too 
  // long in a response message
  void defaultResponse(uint8_t *data, uint16_t len);

  // Send a single byte of data
  uint8_t sendData(uint8_t data);

  // Send several bytes of data
  uint8_t sendData(uint8_t *data, uint16_t len);

  // Send the message
  uint8_t finishMessage();

private:
  enum State {
    EOM,
    HEADER_SENT,
    DATA_SENDING
  };

  uint8_t  nodeNum,
           destAddress,
           state;
};

#endif