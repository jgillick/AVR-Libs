# Addressing example

This example demonstrates automatic addressing on the bus with the daisy 
chain line.

This builds on the [slave_response](/MultidropBusProtocol/examples/02_slave_response) and 
[simple_485](/MultidropBusProtocol/examples/03_simple_485) examples.

Master will have 2 LEDs and a bus with 2 or more slaves, each with a button. 
When it turns on it will automatically address the nodes on the bus and then
light an LED for each slave that has a button pressed.

## Setup

 * Start with 3 AVRs connected to the same ground line.
 * Program one of them with the `master_node` program and the others with `slave_node`.
 * Wire a RS485 transceiver to each AVR
   * `DI` -> `TX`
   * `RO` -> `RX`
   * `DE` and `RE` -> `PB1`
 * Master AVR: Connect an LEDs from `PB0', `PB1` and `PB2` to ground (with current limiting resistors).
 * Slave AVRs: Connect a button from `PB1` to `VCC`.

When you power up the circuit, master will address all nodes and then light the LED on `PB1`, indicating
that it's ready for input. When you press a slave's button, one of the LEDs will light.
