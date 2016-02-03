

#ifndef Multidrop_H
#define Multidrop_H

// Maximum size of the message buffer
#define MSG_BUFFER_LEN  150

#include <avr/io.h>
#include <stdint.h>
#include "MultidropData.h"

#define CMD_RESET   0xAF
#define CMD_ADDRESS 0xBF

class Multidrop {

public:
  static const uint8_t BROADCAST_ADDRESS = 0;
  static const uint8_t BATCH_FLAG = 0b00000001;
  static const uint8_t RESPONSE_MESSAGE_FLAG = 0b00000010;

  Multidrop(MultidropData*);

  // Add the pin and registers for the daisy chain lines. 
  // There should be two and polarity is determined at runtime
  // (i.e. the first one to go HIGH is 'previous' and the other is 'next')
  void addDaisyChain(volatile uint8_t d1_pin_num,
                     volatile uint8_t* d1_ddr_register, 
                     volatile uint8_t* d1_port_register, 
                     volatile uint8_t* d1_pin_register,

                     volatile uint8_t d2_pin_num,
                     volatile uint8_t* d2_ddr_register, 
                     volatile uint8_t* d2_port_register, 
                     volatile uint8_t* d2_pin_register);

  // Get the daisy chain number (1 or 2) for the previous node
  // this will return 0 if the polarity has not been determined yet
  uint8_t getDaisyChainPrev();

  // Get the daisy chain number (1 or 2) for the next node
  // this will return 0 if the polarity has not been determined yet
  uint8_t getDaisyChainNext();

  // Set the daisy chain polarity
  // next and prev should be set to 1 or 2, to match 
  // the pins for d1_* and d2* defined in addDaisyChain
  void setDaisyChainPolarity(uint8_t next, uint8_t prev);


protected:

  enum Masks {
    batch_mode = 0x80,
    response_msg = 0x40
  };

  MultidropData *serial;

  uint16_t messageCRC;

  // Daisy chain pin registers
  volatile uint8_t d1_num,
                   d2_num;
  volatile uint8_t* d1_ddr;
  volatile uint8_t* d1_port;
  volatile uint8_t* d1_pin;
  volatile uint8_t* d2_ddr;
  volatile uint8_t* d2_port;
  volatile uint8_t* d2_pin;

  uint8_t daisy_prev,
          daisy_next;

  // Try to determine the next/prev daisy chain pins
  void checkDaisyChainPolarity();

  // Set the next daisy chain pin to HIGH or LOW (1 or 0)
  void setNextDaisyValue(uint8_t val);

  // Get the value (1 or 0) from the prev daisy chain pin
  uint8_t getPrevDaisyChainValue();

};

#endif
