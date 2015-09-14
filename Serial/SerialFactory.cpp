
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Serial.h"

#define UART_BAUD_SELECT(baudRate)  (((F_CPU) + 8UL * (baudRate)) / (16UL * (baudRate)) -1UL)

SerialFactory::SerialFactory(
  volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
  volatile uint8_t *ucsrc, volatile uint8_t *udr) :
    _ubrrh(ubrrh), _ubrrl(ubrrl),
    _ucsra(ucsra), _ucsrb(ucsrb), _ucsrc(ucsrc),
    _udr(udr),
    _rx_buffer_head(0), _rx_buffer_tail(0),
    _tx_buffer_head(0), _tx_buffer_tail(0)
{
}


// Hook into the UART and start receiving data
void SerialFactory::begin(uint32_t baud) {
  *_ucsra = 0;

  // Endable TX/RX
  *_ucsrb = (1<<TXEN0) | (1<<RXEN0);
  *_ucsrb |= (1<<RXCIE0);  // RX Interrupt

  // Frame format (8-bit, 1 stop bit)
  *_ucsrc = 1<<UCSZ01 | 1<<UCSZ00 ;

  // Set baud
  *_ubrrl =  (unsigned char)UART_BAUD_SELECT(baud);
  *_ubrrh =  (unsigned char)(UART_BAUD_SELECT(baud) << 8);

  // Enable interrupts
  sei();
}

// End RX/TX
void SerialFactory::end() {
	*_ucsrb &= ~(1 << RXEN0);
  *_ucsrb &= ~(1 << TXEN0);
  *_ucsrb &= ~(1 << RXCIE0);
  *_ucsrb &= ~(1 << UDRIE0);
}

// How many bytes are available in the RX buffer
uint8_t SerialFactory::available() {
  return ((uint8_t)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

// Read a byte from the RX buffer or -1
uint8_t SerialFactory::read() {
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    uint8_t c = _rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (uint8_t)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
    return c;
  }
}

// Send everything in the TX buffer with blocking
void SerialFactory::flush() {

	while (_tx_buffer_head != _tx_buffer_tail) {
		while(!(*_ucsra & (1<<UDRE0))); // wait for register to be ready
		_sendNextByte();
	}

	// Disable interrupt until another byte is written
	*_ucsrb &= ~(1 << UDRIE0);
}

// Clears the RX buffer
void SerialFactory::clear() {
  _rx_buffer_head = 0;
  _rx_buffer_tail = 0;
}

// Write something to the TX line
void SerialFactory::write(uint8_t c) {

	// If buffer is empty and the register is ready to be written
	// to, just send it directly
	if ((_tx_buffer_head == _tx_buffer_tail) && (*_ucsra & (1<<UDRE0))) {
		*_udr = c;
		return;
	}

	// If TX buffer is full, we need to flush a byte out first
	uint8_t i = (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;
	if (i == _tx_buffer_tail) {
		while(!(*_ucsra & (1<<UDRE0)));
		_sendNextByte();
	}

	// Add to buffer and enable interrupt
	_tx_buffer[_tx_buffer_head] = c;
  _tx_buffer_head = i;
  *_ucsrb |= (1 << UDRIE0);
}

// Output a string to the TX line
void SerialFactory::print(const char *str) {
  while (*str != '\0') {
    write(*str);
    ++str;
  }
}

// Send the next byte off the TX buffer
void SerialFactory::_sendNextByte() {
	if (_tx_buffer_head == _tx_buffer_tail) return;

	*_udr = _tx_buffer[_tx_buffer_tail];
	_tx_buffer_tail = (_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;

	// If buffer is empty, disable interrupt
	if (_tx_buffer_head == _tx_buffer_tail) {
    *_ucsrb &= ~(1 << UDRIE0);
  }
}

// Receive the byte out of the RX register
void SerialFactory::_receiveByte() {
  _rx_buffer[_rx_buffer_head] = *_udr;

  // Buffer overrun, move the tail forward one
  _rx_buffer_head = (_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
  if (_rx_buffer_head == _rx_buffer_tail) {
    _rx_buffer_tail = (_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
  }
}