# Simple Example

In this simple example, the master node sends a 1 or 0 to the slave node(s)
every second, telling them to turn their LED on or off.

This should make every node blink it's LED every second.

## Setup

 * Start with 2 AVRs connected to the same ground line.
 * Program one of them with the `master_node` program and the other with `slave_node`.
 * Connect `TX` from the master AVR to the `RX` of the slave AVR.
 * On the slave AVR, connect an LED from `PB0' to ground (with a current limiting resistor).

Now add power to both AVRs and you should see the LED blink every second.

## More than one slave

 If you want to connect more than one slave to master with this example:

  * Follow setup instructions above.
  * Connect `TX` from master to `RX` of all slaves.
  * Open `master_node/main.cpp`
    * Change `NODE_COUNT` to the number of slave nodes
    * Reprogram master
  * Open `slave_node/main.cpp`
    * For the second slave, change `NODE_ADDRESS` to `2` and reprogram that node.
    * Do this for all slave nodes (third node, change `NODE_ADDRESS` to `3` and so on)

Now add power and you should see all slaves blink their LEDs in alternating order.