

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

protected:

  enum Masks {
    batch_mode = 0x80,
    response_msg = 0x40
  };

  MultidropData *serial;

  uint16_t messageCRC;
};

#endif
