#ifndef IOPORT_H
#define IOPORT_H

#include <avr/io.h>

struct IOPin {

  uint8_t num;
  volatile uint8_t *port;
  volatile uint8_t *pin;
  volatile uint8_t *ddr;

  IOPin(uint8_t *num, volatile uint8_t *PIN, volatile uint8_t *PORT, volatile uint8_t *DDR) 
    : pin(PIN), port(PORT), ddr(DDR) { }

  // Set the pin to be output
  void as_output() {
    *ddr |= (1 << num);
  }

  // Set the pin to be input
  void as_input() {
    *ddr &= ~(1 << num);
  }

  // Set the pin to HIGH (1) or LOW (0)
  void set(uint8_t value = 1) {
    if (value) *port |= (1 << num);
    else *port &= ~(1 << num);
  }

  // Returns 1 if the input is HIGH
  uint8_t value() { 
    return (*pin & (1 << num)); 
  }
};


#ifdef PORTA
#define PORTA_IO(n) IOPin(n, &PINA, &PORTA, &DDRA)
#endif 
#ifdef PORTB
#define PORTB_IO(n) IOPin(n, &PINB, &PORTB, &DDRB)
#endif 
#ifdef PORTC
#define PORTC_IO(n) IOPin(n, &PINC, &PORTC, &DDRC)
#endif 
#ifdef PORTD
#define PORTD_IO(n) IOPin(n, &PIND, &PORTD, &DDRD)
#endif 
#ifdef PORTE
#define PORTE_IO(n) IOPin(n, &PINE, &PORTE, &DDRE)
#endif 
#ifdef PORTF
#define PORTF_IO(n) IOPin(n, &PINF, &PORTF, &DDRF)
#endif 
#ifdef PORTG
#define PORTG_IO(n) IOPin(n, &PING, &PORTG, &DDRG)
#endif 
#ifdef PORTH
#define PORTH_IO(n) IOPin(n, &PINH, &PORTH, &DDRH)
#endif 
#ifdef PORTI
#define PORTI_IO(n) IOPin(n, &PINI, &PORTI, &DDRI)
#endif 
#ifdef PORTJ
#define PORTJ_IO(n) IOPin(n, &PINJ, &PORTJ, &DDRJ)
#endif 


#endif