/*----------------------------------------*/
// Test addressing and receiving data from master
//
// This will get it's address automatically
// and then turn on it's LED when the master
// node tells it to.
//
/*----------------------------------------*/

#include <stdint.h>

#include "MultidropSlave.h"
#include "MultidropData485.h"

int main() {
  uint8_t ledOn = 0;

  // LED indicator
  DDRB |= (1 << PB0);
  PORTB &= ~(1 << PB0);

  // enable pull-up on RX pin
  PORTD |= (1 << PD0);

  // Setup serial and slave node
  MultidropData485 serial(PD2, &DDRD, &PORTD);
  serial.begin(9600);

  MultidropSlave slave(&serial);

  // Define daisy chain lines and let polarity (next/previous) be determined at runtime
  slave.addDaisyChain(PC3, &DDRC, &PORTC, &PINC,
                      PC4, &DDRC, &PORTC, &PINC);

  // Wait around reading from the bus.
  // Addressing will be handled automatically
  while(1) {
    slave.read();

    // Message addressed to us!
    if (slave.hasNewMessage() && slave.isAddressedToMe() && slave.getCommand() == 0x01) {
      ledOn = slave.getData()[0];

      if (ledOn) {
        PORTB |= (1 << PB0);
      } else {
        PORTB &= ~(1 << PB0);
      }
    }
  }
}
