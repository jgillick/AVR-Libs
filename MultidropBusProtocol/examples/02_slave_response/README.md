# Slave Response Example

This example demonstrates receiving data from slave nodes.

Master sends a message asking a slave node what it's button value 
is. The slave node then puts a 0 or 1 into that message body as 
the response. This is mostly handled by the library automatically, 
all the slave program code needs to do is provide a callback function 
that will provide this value at runtime.

## Setup

 * Start with 2 AVRs connected to the same ground line.
 * Program one of them with the `master_node` program and the other with `slave_node`.
 * Connect `TX` from the master AVR to the `RX` of the slave AVR.
 * Connect `RX` from the master AVR to the `TX` of the slave AVR.
 * Master AVR: Connect an LED from `PB0' to ground (with a current limiting resistor).
 * Slave AVR: Connect a button from `PB3` to positive.

Now add power to both AVRs, when you press the button on the slave node, the LED connected
to master should turn on.
