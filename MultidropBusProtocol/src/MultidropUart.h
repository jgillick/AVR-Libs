
#ifndef MultidropUart_H
#define MultidropUart_H

#include "MultidropData.h"
#include <avr/io.h>

class MultidropUart : public MultidropData {
public:
  MultidropUart();

  void begin(uint32_t);
  void end();

  uint8_t available();
  uint8_t read();

  void write(uint8_t);

  void flush();
  void clear();
};

#endif