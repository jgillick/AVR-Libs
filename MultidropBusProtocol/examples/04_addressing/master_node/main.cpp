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
void updateLEDs();

uint8_t response_buff[254];
volatile uint32_t current_time;

MultidropData485 serial(PD2, &DDRD, &PORTD);
MultidropMaster master(&serial);

int main() {
  uint32_t t;
  uint8_t default_response[2] = {0, 0};

  // Wait for all nodes to come online
  _delay_ms(5);

  // Setup serial and clock
  serial.begin(9600);
  startClock();

  // Init LEDs
  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
  PORTB &= ~(1 << PB0) & ~(1 << PB1) & ~(1 << PB2);

  // Set daisy chain
  master.addDaisyChain(PD5, &DDRD, &PORTD, &PIND,
                       PD6, &DDRD, &PORTD, &PIND);
  master.setDaisyChainPolarity(1, 2);
  
  // Addressing
  addressNodes();

  // How handle responding to button presses
  while(1) {

    // Start message asking for button values
    master.setResponseSettings(response_buff, getTime(), 2, default_response);
    master.startMessage(0x02, MultidropMaster::BROADCAST_ADDRESS, 1, true, true);

    // Wait for response
    while(1) {
      t = getTime();
      if (master.checkForResponses(t)) {
        break;
      }
    }

    // Close the message
    master.finishMessage();

    // Light the LEDs
    updateLEDs();
  }
}

void addressNodes() {
  uint32_t t = getTime();
  MultidropMaster::adr_state_t response;

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
    PORTB |= (1 << PB0);
  }
  // Error, try again in a few ms
  else if (response == MultidropMaster::ADR_ERROR) {
    _delay_ms(3);
    addressNodes();
  }
}

void updateLEDs() {
  PORTB &= ~(1 << PB1) & ~(1 << PB2);

  if (response_buff[0] == 1) {
    PORTB |= (1 << PB1);
  }
  if (response_buff[1] == 1) {
    PORTB |= (1 << PB2);
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
  OCR0A = (F_CPU * 1/1000) / 64;

  sei();
}

// Interrupt to keep time
ISR(TIMER0_COMPA_vect) {
  current_time++;
}