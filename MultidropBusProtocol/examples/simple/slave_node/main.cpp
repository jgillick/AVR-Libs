/*----------------------------------------*/
// Test receiving messages from master
//
// This receives a message from master (command 0x01),
// which tells it to turn an LED on or off.
//
/*----------------------------------------*/

#include <stdint.h>

#include "MultidropSlave.h"
#include "MultidropUart.h"

#define NODE_COUNT 5

int main() {
  uint8_t ledOn = 0;

  DDRB |= (1 << PB0);
  PORTB &= ~(1 << PB0);

  MultidropUart serial;
  serial.begin(9600);

  MultidropSlave slave(&serial);
  slave.setAddress(1);

  while(1) {
    slave.read();
    if (slave.isReady()){

      if(slave.getCommand() == 0x01) {
        ledOn = slave.getData()[0];

        if (ledOn) {
          PORTB |= (1 << PB0);
        } else {
          PORTB &= ~(1 << PB0);
        }
      }
      slave.reset();
    }
  }
}


