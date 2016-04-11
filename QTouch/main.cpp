/*******************************************************************************
* A minimal QTouch proximity implementation with a single sensor.
*
* Setup
* -----
*  + Connect an LED from `PB2` to ground.
*  + Connect a 22nF capacitor between `PC0` and `PC1`.
*  + Connect a 1k resistor between `PC1` and the sensor.
*  + The sensor can be a wire or sheet of metal or foil.
*
* Programming
* -----------
* Set the first 5 variables in the `Makefile` to match your device.
*
* Use
* ---
* When power is supplied, the LED will turn on until it detects something come within range
* of the sensor.
*
******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "touch.h"
#include "touch_control.h"

/*----------------------------------------------------------------------------
                                prototypes
----------------------------------------------------------------------------*/
void init_timer_isr();
uint16_t get_time();

/*----------------------------------------------------------------------------
                                    macros
----------------------------------------------------------------------------*/
#define UART_BAUD_SELECT(baudRate)  (((F_CPU) + 8UL * (baudRate)) / (16UL * (baudRate)) -1UL)
#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))

// Measure touch every x milliseconds
#define MEASUREMENT_PERIOD_MS 50u

/*----------------------------------------------------------------------------
                                global variables
----------------------------------------------------------------------------*/

/* Timer for measurements */
volatile uint16_t measurement_timer = 0;

/* flag set by timer ISR when it's time to measure touch */
volatile uint8_t time_to_measure_touch = 0u;

/* current time, set by timer ISR */
volatile uint16_t current_time = 0u;

/**
 * Main program entry point
 */
int main() {
  DDRB |= (1 << PB2);
  PORTB &= ~(1 << PB2);

  /* configure timer ISR to fire regularly */
  init_timer_isr();

	/* Initialize Touch sensors */
	touch_init();

  /* enable interrupts */
  sei();

  /* loop forever */
  for( ; ; )
  {
    if (time_to_measure_touch) {
      time_to_measure_touch = 0u;

      if (touch_measure(0, get_time())) {
        // Touch detected
        PORTB &= ~(1 << PB2);
      } else {
        // No touch
        PORTB |= (1 << PB2);
      }
    }
  }
  return 0;
}

/**
 * Returns the current time in milliseconds.
 */
uint16_t get_time() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    return current_time;
  }
}

/**
 * Initialize the timer interrupt.
 */
void init_timer_isr() {
  TIMSK0 |= (1 << OCIE0A); // enabled timer
  TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler = 64
  TCCR0A |= ( 1 << WGM01 ); // Timer mode = CTC

  // Fire interrupt every 1ms: (CLK_FREQ * seconds) / TIMER_PRESCALER
  OCR0A = (uint16_t)(F_CPU * 1/1000) / 64;

  sei();
}

/**
 * Interrupt which keeps the current time in milliseconds.
 */
ISR(TIMER0_COMPA_vect) {
  measurement_timer += 1;
  current_time += 1;

  // It's time to measure touch again
  if(measurement_timer >= MEASUREMENT_PERIOD_MS) {
    time_to_measure_touch = 1u;
    measurement_timer = 0;
  }
}
