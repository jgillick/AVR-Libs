

#ifndef Multidrop_H
#define Multidrop_H

// Maximum size of the message buffer
#define MSG_BUFFER_LEN  150

#include <avr/io.h>
#include <stdint.h>
#include "MultidropData.h"

class Multidrop {

public:
  static const uint8_t BROADCAST_ADDRESS = 0;
  static const uint8_t BATCH_FLAG = 0b00000001;
  static const uint8_t RESPONSE_MESSAGE_FLAG = 0b00000010;

  Multidrop(MultidropData*);

  // Sets the address of this node 
  // (0 makes this node master -- there can only be 1 on the bus)
  void setMyAddress(uint8_t);

  // Is there a new message
  uint8_t isReady();

  // The message command for the latest received message
  uint8_t getCommand();

  // Set the response for a message
  void setResponse(uint8_t val);
  void setResponse(uint8_t *buf, uint8_t len);

  // Return the body of the message
  uint8_t* getBody();

  // Return the length of the message body
  int getBodyLen();

protected:

  enum Masks {
    batch_mode = 0x80,
    response_msg = 0x40
  };

  MultidropData *serial;

  uint16_t messageCRC;

  uint8_t buffer[MSG_BUFFER_LEN];

  // Put the bus transceiver in TX mode
  void sendMode();

  // Put the bus transceiver in RX mode
  void receiveMode();

  // Send a byte to the serial stream and 
  // update the messageCRC value
  void sendByte(uint8_t b, uint8_t updateCRC=1);
};

#endif
