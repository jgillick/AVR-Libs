/*----------------------------------------*/
// Test receiving messages from master
//
// This receives a message from master (command 0x01),
// which tells it to turn an LED on or off.
//
/*----------------------------------------*/

#include <stdint.h>

#include "MultidropSlave.h"
#include "MultidropDataUart.h"

#define NODE_ADDRESS 1

void responseHandler(uint8_t command, uint8_t *buff,uint8_t len);

int main() {
  uint8_t ledOn = 0;

  // Button on PB3
  DDRB &= ~(1 << PB3);
  
  // Setup serial
  MultidropDataUart serial;
  serial.begin(9600);

  // Slave node, with response handler
  MultidropSlave slave(&serial);
  slave.setAddress(NODE_ADDRESS);
  slave.setResponseHandler(&responseHandler);

  // Wait around reading from the bus. Responses are handled automatically.
  while(1) {
    slave.read();
  }
}

void responseHandler(uint8_t command, uint8_t *buff, uint8_t len) {
  buff[0] = !!(PINB & (1<<PB3));
}

