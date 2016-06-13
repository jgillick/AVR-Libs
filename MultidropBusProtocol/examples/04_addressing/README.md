# Addressing example

This example demonstrates automatic addressing on the bus with the daisy
chain line.

This builds on the [slave_response](/MultidropBusProtocol/examples/02_slave_response) and
[simple_485](/MultidropBusProtocol/examples/03_simple_485) examples.

Master will be connected to a bus with 2 or more slaves, each with a an LED.
When it turns on it will automatically address the nodes on the bus and then
light an LED for each slave every second.

## Setup

 * Start with 3 AVRs connected to the same ground line.
 * Program one of them with the `master_node` program and the others with `slave_node`.
 * Wire a RS485 transceiver to each AVR
   * `DI` -> `TX`
   * `RO` -> `RX`
   * `DE` / `RE` -> `PD2`
 * Slave AVR: Connect an LEDs from `PB0` to ground (with current limiting resistors).
 * Daisy chain:
   * Connect `PC4` from master to `PC3` on the first slave node.
   * Connect `PC4` from the first slave node to `PC3` on the next.
   * Continue for all slave nodes
   * (optional) Connect `PC4` from the last slave node to `PC3` on the master node.

When you power up the circuit, master will address all nodes and then light the LED on the first slave
AVR. A second later, master will light the LED on the second slave. And so on, down the line of slave AVRs.
