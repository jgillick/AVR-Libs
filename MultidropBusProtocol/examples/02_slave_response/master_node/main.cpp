/*----------------------------------------*/
// Test sending messages from master.
//
// In this script, master sends a message (command = 0x01)
// with alternating 0s and 1s for each node on the bus.
//
/*----------------------------------------*/

#include <stdint.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "MultidropMaster.h"
#include "MultidropDataUart.h"

#define NODE_COUNT 1

volatile uint32_t current_time = 0;

void startClock();
uint32_t getTime();

int main() {
  uint32_t t;
  uint8_t response_buff[NODE_COUNT];
  uint8_t default_response[1] = {0};

  // LED on PB0
  DDRB |= (1 << PB0);
  PORTB &= ~(1 << PB0);

  // Setup serial connection
  MultidropDataUart serial;
  serial.begin(9600);

  // Setup master node
	MultidropMaster master(&serial);
  master.setNodeLength(NODE_COUNT);

  startClock();
  while(1) {

    // Start message asking for button values
    master.startMessage(0x02, MultidropMaster::BROADCAST_ADDRESS, 1, true, true);

    // Timeout in 1ms, default response is 0
    master.setResponseSettings(response_buff, getTime(), 1, default_response);

    // Wait for response
    while(t = getTime() && !master.checkForResponses(t));

    // Light the LED if the first node's button is pressed
    if (response_buff[0] == 1) {
      PORTB |= (1 << PB0);
    } else {
      PORTB &= ~(1 << PB0);
    }
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