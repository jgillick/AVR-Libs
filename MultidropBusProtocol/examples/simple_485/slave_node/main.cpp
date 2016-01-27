/*----------------------------------------*/
// Test receiving messages from master
//
// This receives a message from master (command 0x01),
// which tells it to turn an LED on or off.
//
/*----------------------------------------*/

#include <stdint.h>

#include "MultidropSlave.h"
#include "MultidropData485.h"

#define NODE_ADDRESS 1

int main() {
  uint8_t ledOn = 0;

  // LED on PB0
  DDRB |= (1 << PB0);
  PORTB &= ~(1 << PB0);

  MultidropData485 rs485(PD2, &DDRD, &PORTD);
  MultidropSlave slave(&rs485);

  rs485.begin(9600);
  slave.setAddress(NODE_ADDRESS);

  while(1) {
    slave.read();
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


