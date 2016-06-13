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

  // Setup serial and clock
  serial.begin(9600);
  startClock();

  // Debugging LEDs
  DDRD |= (1 << PD5) | (1 << PD6);
  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);

  PORTB |= (1 << PB1);

  // Wait for all nodes to come online
  _delay_ms(1000);

  // Set daisy chain
  master.addDaisyChain(PC3, &DDRC, &PORTC, &PINC,
                       PC4, &DDRC, &PORTC, &PINC);
  master.setDaisyChainPolarity(1, 2);

  PORTB &= ~(1 << PB1);
  PORTB |= (1 << PB2);
  _delay_ms(1000);

  // Addressing
  addressNodes();

  // Now start blinking some LEDs
  while(1) {
    master.startMessage(0x01, MultidropMaster::BROADCAST_ADDRESS, 1, true);

    // Send alternating 1s and 0s
    for (uint8_t i = 1; i <= master.nodeNum; i++) {
      if (i == targetNode){
        master.sendData(0x01);
      } else {
        master.sendData(0x00);
      }
    }

    targetNode++;
    if (targetNode > master.nodeNum) {
      targetNode = 0;
    }
    _delay_ms(1000);
  }
}

void addressNodes() {
  uint32_t t = getTime();
  MultidropMaster::adr_state_t response;

  // Reset all nodes and then start addressing
  master.resetAllNodes();
  _delay_ms(5);
  master.startAddressing(t, 2);

  // Wait for all nodes to finish being addressed
  while(1) {
    t = getTime();
    response = master.checkForAddresses(t);
    if (response != MultidropMaster::ADR_WAITING) {
      break;
    }
  }

  // Finished successfully
  if (response == MultidropMaster::ADR_DONE) {
    PORTB &= ~(1 << PB1) & ~(1 << PB2);
    PORTB |= (1 << PB0);
  }
  // Error, try again in a few ms
  else if (response == MultidropMaster::ADR_ERROR) {
    PORTB &= ~(1 << PB0) & ~(1 << PB2);
    PORTB |= (1 << PB1);
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
  TCCR0A |= ( 1 << WGM01 ); // Timer mode = CTC

  // Fire interrupt every 1ms: (CLK_FREQ * seconds) / TIMER_PRESCALER
  OCR0A = (uint16_t)(F_CPU * 1/1000) / 64;

  sei();
}

// Interrupt to keep time
ISR(TIMER0_COMPA_vect) {
  current_time++;
}