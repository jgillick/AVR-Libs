
#ifndef MultidropData485_H
#define MultidropData485_H

#include "MultidropDataUart.h"
#include <avr/io.h>

class MultidropData485 : public MultidropDataUart {
public:
  MultidropData485();
  
  void write(uint8_t);
};

#endif