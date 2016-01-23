
#include "MultidropData485.h"
#include <avr/interrupt.h>

MultidropData485::MultidropData485() {
}

// Set send enable pin and the write
void MultidropData485::write(uint8_t c) {
  MultidropDataUart::write(c);
}

