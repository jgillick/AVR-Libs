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

int main() {
  uint8_t ledOn = 0;

  // Button on PB1
  DDRB &= ~(1 << PB1);
  
  // Setup serial
  MultidropDataUart serial;
  serial.begin(9600);

  // Slave node, with response handler
  MultidropSlave slave(&serial);
  slave.setResponseHandler(&responseHandler);

  // Wait around reading from the bus. Responses are handled automatically.
  while(1) {
    slave.read();
  }
}

void responseHandler(uint8_t command, uint8_t *buff, uint8_t len) {
  buff[0] = !!(PINB & (1<<PB1));
}
