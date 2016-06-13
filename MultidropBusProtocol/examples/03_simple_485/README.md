# Simple RS458 Example

This is the same as the original [simple example](../simple/), but communicates on an
RS485 bus through a common 485 transceiver, like the Maxim MAX487E.

## Setup

 * Start with 2 AVRs connected to the same ground line.
 * Program one of them with the `master_node` program and the other with `slave_node`.
 * Wire a RS485 transceiver to each AVR
   * `DI` -> `TX`
   * `RO` -> `RX`
   * `DE` / `RE` -> `PD2`
 * Connected both transceiver together by their A/B pins (`A` -> `A`, `B` -> `B`).
 * On the slave AVR, connect an LED from `PD2` to ground (with a current limiting resistor).
 * Optionally, you can also attach an LED on master from `PD2` to ground (with a current limiting resistor).
   and it will blink ever time it tells the slave AVR to blink.

Now add power to both AVRs and you should see the LED blink every second.

## More than one slave

 If you want to connect more than one slave to master with this example:

  * Follow setup instructions above.
  * Open `master_node/main.cpp`
    * Change `NODE_COUNT` to the number of slave nodes
    * Reprogram master
  * Open `slave_node/main.cpp`
    * For the second slave, change `NODE_ADDRESS` to `2` and reprogram that node.
    * Do this for all slave nodes (third node, change `NODE_ADDRESS` to `3` and so on)

Now add power and you should see all slaves blink their LEDs in alternating order.