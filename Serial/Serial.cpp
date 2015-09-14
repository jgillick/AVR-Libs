
#include <avr/interrupt.h>
#include "Serial.h"

SerialFactory Serial(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);

// Received a byte from the RX line
ISR(USART_RX_vect){
  Serial._receiveByte();
}

// Ready to send a byte on the TX line
ISR(USART_UDRE_vect) {
  Serial._sendNextByte();
}