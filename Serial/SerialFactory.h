
#include <avr/io.h>

#ifndef SerialFactory_h
#define SerialFactory_h

#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE 150
#endif

#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE 60
#endif

class SerialFactory {

  protected:
      volatile uint8_t * const _ubrrh;
      volatile uint8_t * const _ubrrl;
      volatile uint8_t * const _ucsra;
      volatile uint8_t * const _ucsrb;
      volatile uint8_t * const _ucsrc;
      volatile uint8_t * const _udr;

      volatile uint8_t _tx_buffer[SERIAL_TX_BUFFER_SIZE];
      volatile uint8_t _rx_buffer[SERIAL_RX_BUFFER_SIZE];

      volatile uint8_t _tx_buffer_head;
      volatile uint8_t _tx_buffer_tail;
      volatile uint8_t _rx_buffer_head;
      volatile uint8_t _rx_buffer_tail;

  public:
    SerialFactory(
        volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
        volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
        volatile uint8_t *ucsrc, volatile uint8_t *udr);

    void begin(uint32_t);
    void end();

    uint8_t available();
    uint8_t read();

    void write(uint8_t);
    void print(const char*);

    inline void write(uint32_t n) { write((uint8_t)n); }
    inline void write(int32_t n) { write((uint8_t)n); }
    inline void write(uint16_t n) { write((uint8_t)n); }
    inline void write(int16_t n) { write((uint8_t)n); }

    void flush();
    void clear();

    // Private-ish members used by interrupts
    void _sendNextByte();
    void _receiveByte();
};

#endif
