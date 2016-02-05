/*----------------------------------------*/
// Test addressing and sending data to master
//
// This will get it's address automatically
// and then report to master when it's button
// is pressed.
//
/*----------------------------------------*/

#include <stdint.h>

#include "MultidropSlave.h"
#include "MultidropData485.h"

void responseHandler(uint8_t command, uint8_t *buff,uint8_t len);

int main() {
  // Button on PB3
  DDRB &= ~(1 << PB3);
  
  // Setup serial
  MultidropData485 serial(PD2, &DDRD, &PORTD);
  serial.begin(9600);

  // Setup slave node
  MultidropSlave slave(&serial);
  slave.setResponseHandler(&responseHandler);
  slave.addDaisyChain(PD5, &DDRD, &PORTD, &PIND,
                      PD6, &DDRD, &PORTD, &PIND);

  // Wait around reading from the bus. Responses are handled automatically.
  while(1) {
    slave.read();
  }
}

void responseHandler(uint8_t command, uint8_t *buff, uint8_t len) {
  buff[0] = !!(PINB & (1<<PB3));
}
