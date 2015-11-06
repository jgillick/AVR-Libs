
/**
  Flashes an LED on PB0 every 1 second.
  This contains the helper functions 
  as well as examples (commented out) of using 
  the registers directly.
*/

#include <avr/io.h>
#include <util/delay.h>
#include "ioport.h"

int main() {
  IOPin led_pin = PORTB_IO(PB0);

  led_pin.as_output();
  // *(led_pin.ddr) |= (1 << led_pin.num);

  while(1) {
    led_pin.set(1);
    // *(led_pin.port) |= (1 << led_pin.num);

    _delay_ms(1000);
    
    led_pin.set(0);
    // *(led_pin.port) &= ~(1 << led_pin.num);

    _delay_ms(1000);
  }
}