#include "Multidrop.h"
#include "MultidropData.h"


Multidrop::Multidrop(MultidropData *_serial) : serial(_serial) {
}

void Multidrop::addDaisyChain(volatile uint8_t d1_pin_number,
                              volatile uint8_t* d1_ddr_register, 
                              volatile uint8_t* d1_port_register, 
                              volatile uint8_t* d1_pin_register,

                              volatile uint8_t d2_pin_number,
                              volatile uint8_t* d2_ddr_register, 
                              volatile uint8_t* d2_port_register, 
                              volatile uint8_t* d2_pin_register) {

  d1_num  = d1_pin_number;
  d1_ddr  = d1_ddr_register;
  d1_port = d1_port_register;
  d1_pin  = d1_pin_register;

  d2_num  = d2_pin_number;
  d2_ddr  = d2_ddr_register;
  d2_port = d2_port_register;
  d2_pin  = d2_pin_register;

  // Init polarity checks
  daisy_prev = 0;
  daisy_next = 0;

  // Init registers
  *d1_ddr &= ~(1 << d1_num);
  *d1_port &= ~(1 << d1_num);

  *d2_ddr &= ~(1 << d2_num);
  *d2_port &= ~(1 << d2_num);
}

void Multidrop::checkDaisyChainPolarity() {
  if (daisy_prev && daisy_next) return;

  uint8_t d1 = *d1_pin & (1 << d1_num),
          d2 = *d2_pin & (1 << d2_num);

  // Both HIGH, then can't decide
  if (d1 && d2) {
    return;
  }
  else if (d1) {
    daisy_prev = 1;
    daisy_next = 2;
  }
  else if (d2) {
    daisy_prev = 2;
    daisy_next = 1; 
  }
}

uint8_t Multidrop::getDaisyChainPrev() {
  return daisy_prev;
}

uint8_t Multidrop::getDaisyChainNext() {
  return daisy_next;
}

void Multidrop::setDaisyChainPolarity(uint8_t next, uint8_t prev) {
  checkDaisyChainPolarity();
  if (prev == 0 || prev > 2 || next == 0 || next > 2) return;

  daisy_prev = prev;
  daisy_next = next;
}

void Multidrop::setNextDaisyValue(uint8_t val) {
  checkDaisyChainPolarity();
  if (!daisy_next) return;

  uint8_t mask = (daisy_next == 2) ? (1 << d2_num) : (1 << d1_num);
  volatile uint8_t* port = (daisy_next == 2) ? d2_port : d1_port;

  if (val) {
    *port |= mask;
  } else {
    *port &= ~mask;
  }

}

uint8_t Multidrop::getPrevDaisyChainValue() {
  checkDaisyChainPolarity();
  if (!daisy_prev) return 0;

  uint8_t mask = (daisy_prev == 2) ? (1 << d2_num) : (1 << d1_num);
  volatile uint8_t* pin = (daisy_prev == 2) ? d2_pin : d1_pin;

  return !!(*pin & mask);
}

