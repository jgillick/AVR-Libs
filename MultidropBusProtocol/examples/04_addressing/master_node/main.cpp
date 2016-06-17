/*----------------------------------------*/
// Test addressing and receiving data from slave nodes
//
// This will address all nodes automatically
// and then poll for button press info from
// slave nodes.
//
/*----------------------------------------*/

#include <stdint.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "MultidropMaster.h"
#include "MultidropData485.h"


void startClock();
uint32_t getTime();
void addressNodes();

uint8_t response_buff[254];
volatile uint32_t current_time;

MultidropData485 serial(PD2, &DDRD, &PORTD);
MultidropMaster master(&serial);

int main() {
  uint8_t targetNode = 0;

  // enable pull-up on RX pin
  PORTD |= (1 << PD0);

  // Setup serial and clock
  serial.begin(9600);
  startClock();

  // Wait for all nodes to come online
  _delay_ms(200);

  // Set daisy chain and the polarity
  master.addDaisyChain(PC4, &DDRC, &PORTC, &PINC, // prev daisy line
                       PC3, &DDRC, &PORTC, &PINC, // next daisy line
                       true);

  // Addressing
  addressNodes();

  // Now start blinking some LEDs
  while(1) {

    // Start blink message
    master.startMessage(0x01, MultidropMaster::BROADCAST_ADDRESS, 1, true);

    // Send 0x00 to all nodes except one
    for (uint8_t i = 1; i <= master.nodeNum; i++) {
      if (i == targetNode){
        master.sendData(0x01);
      } else {
        master.sendData(0x00);
      }
    }

    // Finish message
    master.finishMessage();

    // Increment the target node to blink
    targetNode++;
    if (targetNode > master.nodeNum) {
      targetNode = (master.nodeNum > 1) ? 1 : 0; // if we only have 1 node, reset to zero
    }
    _delay_ms(1000);
  }
}

void addressNodes() {
  uint32_t time = getTime();
  MultidropMaster::adr_state_t response;

  // Reset all nodes and then start addressing
  master.resetAllNodes();
  _delay_ms(10);
  master.startAddressing(time, 100);

  // Wait for all nodes to finish being addressed
  while(1) {
    time = getTime();
    response = master.checkForAddresses(time);
    if (response != MultidropMaster::ADR_WAITING) {
      break;
    }
  }

  // Error, try again in a few ms
  if (response == MultidropMaster::ADR_ERROR) {
    _delay_ms(3);
    addressNodes();
  }
}


// Get current time, in milliseconds
uint32_t getTime() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    return current_time;
  }
}

// Start the millisecond clock timer
void startClock() {
  current_time = 0;

  TIMSK0 |= (1 << OCIE0A); // enabled timer
  TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler = 64
  TCCR0A |= (1 << WGM01); // Timer mode = CTC

  // Fire interrupt every 1ms: (CLK_FREQ * seconds) / TIMER_PRESCALER
  OCR0A = (uint16_t)(F_CPU * 1/1000) / 64;

  sei();
}

// Interrupt to keep time
ISR(TIMER0_COMPA_vect) {
  current_time++;
}
