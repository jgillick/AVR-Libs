UART Serial Library
===================

This is a simplified port of Arduino's `Serial` library.

My problem with many examples of serial communication on the internet,
is they all seem to use these two lines for sending a character:

```cpp
while (UCSR0A & (1<<UDRE0));
UDR = nextByte;
```

The while loop is completely blocking. I have something against code that just
sits around and blocks everything while waiting for something to happen.

The good news is that there is an under documented feature on most (all?) atmegas --
an interrupt that is called when the TX register is ready to receive a byte:


```cpp
int main() {
  // Enable interrupt
  UCSR0B |= (1 << UDRIE0);
}

ISR(UART_UDRE_vect) {
  UDR = nextByte;
}
```

Differences from Arduino Serial
-------------------------------
This library was initially ported from Arduino with a few differences:

 * Larger RX/TX buffers (150/60 bytes, respectively)
 * When RX buffer overruns, it removes the oldest byte to make way for the newest.
   This is based on the assumption that the newest data is the most important.
   (Arduino stops receiving data when the RX buffer is full)
 * New `clear` method will remove everything currently in the RX buffer.