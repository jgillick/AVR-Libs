# IO Ports

This is a simple wrapper that makes it easy to encapsulate the relationship
between ports, pins and direction registers. 

For example, this initializes pin PB0 to output and sets it HIGH:

```cpp

IOPin led_pin = PORTB_IO(PB0);
led_pin.as_output();
led_pin.set(1);

```

This is especially handy when you need to pass a pin configuration
to a function or library:

```cpp
#include <util/delay.h>
#include "ioport.h"

void flashPin(IOPin *pin) {
  led_pin->as_output();
  while(1) {
    pin->set(1);
    _delay_ms(1000);
    pin->set(0);
    _delay_ms(1000);
  }
}

int main() {
  IOPin led_pin = PORTB_IO(PB0);
  flashPin(*led_pin);
}

```

Currently there is `PORTA_IO` - `PORTJ_IO` implemented, but more can be added very easily
in `ioport.h`.

## Optional helpers methods

The API defines the following optional helper methods:

* `void as_output()` -- Set the pin to be output
* `void as_input()`  -- Set the pin to be input
* `void set(uint8_t value = 1)` -- Set the pin to HIGH (1) or LOW (0)
* `value()` -- Returns 1 if the input is HIGH


# Accessing the registers

You can still access the registers directly, if you wish. 
The following is identical to the first example:

```cpp
IOPin led_pin = PORTB_IO(PB0);
*(led_pin.ddr) |= (1 << led_pin.num);
*(led_pin.port) |= (1 << led_pin.num);
```