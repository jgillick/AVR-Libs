#include "Multidrop.h"
#include "MultidropData.h"
#include <util/crc16.h>

Multidrop::Multidrop(MultidropData *_serial) : serial(_serial) {
}

void Multidrop::sendMode() {
  
}

void Multidrop::receiveMode(){
}

void Multidrop::sendByte(uint8_t b, uint8_t updateCRC) {
  serial->write(b);
  if (updateCRC) {
    messageCRC = _crc16_update(messageCRC, b);
  }
}