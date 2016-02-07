
#include "MultidropData485.h"

MultidropData485::MultidropData485(volatile uint8_t de_pin_num,
                                   volatile uint8_t* de_ddr_register, 
                                   volatile uint8_t* de_port_register):
                                   de_pin(de_pin_num),
                                   de_ddr(de_ddr_register),
                                   de_port(de_port_register) { 

  *de_ddr |= (1 << de_pin_num);
  *de_port &= ~(1 << de_pin_num); 
}

void MultidropData485::write(uint8_t b) {
  *de_port |= (1 << de_pin);
  MultidropDataUart::write(b);
  flush();
  *de_port &= ~(1 << de_pin);
}
