# AVR Multi-drop Bus Message Protocol

A versatile communication protocol for a multi-drop master/slave bus (like RS485).
In this protocol, master facilitates all message and responses on the bus.

## Implemenation Quickstart

This is the most basic master/server setup where the slave node address is known and master
just sends messages. (see the [simple example](/MultidropBusProtocol/examples/01_simple))

### Slave

```c
MultidropDataUart serial;
MultidropSlave slave(&serial);

serial.begin(9600);
slave.setAddress(0x01);

while(1) {
  slave.read();
  if (slave.hasNewMessage() && slave.isAddressedToMe()) {
    // Do something with the received message
    // See: slave.getCommand() and slave.getData()
  }
}
```

### Master

```c
MultidropDataUart serial;
MultidropMaster master(&serial);

serial.begin(9600);

// send command 0xA1, to node 1, message data length will be 2
master.startMessage(0xA1, 0x01, 2);
master.sendData('H');
master.sendData('i');
master.finishMessage();
```

### Explanation

Both master and slave nodes need a way to communicate to each other. So the first thing you need to do
is initialize a data object. In this case `MultidropDataUart` communicates over the main UART line.
There's also a `MultidropData485`, which can be used to communicate via RS485 transceivers. You can
easily create your own data class by extending `MultidropData`.

The rest is probably self explanatory. The slave nodes will need to call the `read()` method regularly
in order to read new bytes from the data source. When a full message has been received, `hasNewMessage()`
will return `true`.

### Examples

There are more complex examples in the [examples directory](/MultidropBusProtocol/examples/).

## Communication Protocol Overview

This bus assumes that all nodes have addresses from 1 (master) - 255 (see Addressing section).

## Message Packet Format

Each message has 4 section:
 * Start: Two bytes signaling the start of the message.
 * Header: Start of message, flags, destination address, command and length
 * Data: The data for the message
 * End: a 16 bit CRC to validate the message

A full message would look something like this:

```
   Start                  Header                  Data   End
  -------   -----------------------------------  -----  -----
 0xFF 0xFF  <Flags> <Address> <Command> <length> <Data> <CRC>

 0xFF 0xFF  - The start of the message
 <Flags>    - Flags about the messages
 <Address>  - The node the message is going to (0 - 255)
 <Command>  - The type of command that's being sent (see next paragraph)
 <Length>   - The length of the message, if in individual mode (limited to MD_MAX_DATA_LEN).
              If in batch mode, it's 2 bytes: first is the total number of
              slave nodes and the second is the number of bytes per node (limited to MD_MAX_DATA_LEN).
 <Data>     - The data for the message. This can be optional if `Length` is 0.
 <CRC>      - A 16 bit CRC to validate the message
```

### Flags

This is an 8-bit flag that represents the nature of the message.

 * `00000000` - Standard message
 * `00000001` - This is a batch message (see section below)
 * `00000010` - This is a response message. (see section below)
 * `00000100` - Reserved
 * `00001000` - Reserved
 * `00010000` - User assignable
 * `00100000` - User assignable
 * `01000000` - User assignable
 * `10000000` - User assignable

### Node address

The address the message is going to. Slave node addresses start counting at 1 and  0 means the
message applies to all nodes. Master does not have an address, as it coordinates all messages.

### Command

A byte that represents what this message is for.

#### Reserved commands:

These commnads are reserved and used by the system.

 * `0xFA` - Reset node's address and daisy lines.
 * `0xFB` - Automatic addressing (see section below).
 * `0xFC` - Reserved, but unused.
 * `0xFD` - Reserved, but unused
 * `0xFE` - Reserved, but unused
 * `0xFF` - Null message

## Length

In a standard message, this is the number of bytes in the data section of the message.
As you see below, this meaning changes for batch and response messages.

## Types of messages

Every message is either a command to a node (or multiple nodes), or a request
for a response from a node (or multiple nodes).  This message can also be directed at a
single node or all nodes.

### Individual vs Broadcast vs Batch

For all messages, master is either sending to an individual node or to all nodes at once.
The default mode is master sending to an individual node.

If broadcasting, the node address will be `0x00` and the data section apply to all nodes.

Batch is a combination of the two. In this mode, the data section contains a unique piece
of data for each node in the bus. A message in batch mode will be addressed to `0x00`, the
flag will have the first bit set to 1 and length will be two bytes: first is the number of nodes
on the bus and the second is how many byte of data per node.

For example, to send an on/off value to 3 nodes, the message would look something like this:

```
0xFF 0xFF - Start of message
0x01      - Flags: broadcast batch
0x00      - Broadcast address
0x05      - Command
0x03 0x01 - Length: There are 3 nodes and 1 byte message for each
0x00      - Message for the 1st slave
0x01      - Message for the 2nd slave
0x00      - Message for the 3rd slave
0xF9 0x5B - Message CRC
```

In this example the message flag and the address sets it as a broadcast message. Then the
message sends the value 0 to the first node, 1 to the second node and 0 to the third. Notice
that length is set to `0x01`, specifying that the value for each node will just be 1 byte long.

### Response messages

A response message is sent by the master in order to retrieve information from the slave nodes.
In this type of message, master sends the header section and expects the node(s) to fill in the
data section before sanding the CRC to close up the message.

For example, if master wants 3 RGB values from node `0x05` and the command for RGB is `0x08`, master will send:

```
0xFF 0xFF - Start of message
0x02      - Flags: response message
0x05      - Address message to node 0x05
0x08      - RGB command
0x03      - Response should be 3 bytes long
```

Then it waits for node 0x05 to respond:

`0xFF 0x10 0x00`

Then master completes the message with the CRC:

`0x46 0xDC`


### Batch response message

When sending a batch response message, all nodes will fill the message body based on their
order in the bus. So node 1 will put its response in first then 2 and so on. Each node
monitors the bus to know when to add a response. So if the message states that each response
is 3 bytes long, node 4 waits until 9 bytes have been sent and then adds it's response to the
body. If a node takes too long to respond, master will insert dummy values (usually zeros)
in it's place to unblock the nodes after it.

Length will match normal batch messages, the first length byte will be the number of nodes on the
bus and the second byte will be the data length per node expected in each response.

## Automatic Addressing

You can set a node's address manually (via `setAddress()`) or you can use the library's
automatic addressing scheme.

The addressing method we use involves an extra daisy chain wire in addition to the data lines.
This wire is connected from the master to the first node, then outgoing from the first node
to the second and so on. Each node controls the outgoing portion of the daisy chain line.

To address all nodes, master will first send out a global `reset` command that
tells all nodes to forger their address. Then master starts a batch response message:

`0xFF 0xFF 0x03 0x00 <Address Command> 0x00 0x02`

It then sets the daisy chain line to the first node to `high` and sends `0x00` to the bus:

`0x00`

The first node will see it's incoming daisy line is `high` and increment the received
address by 1, to `0x01`, sends that to the bus and waits for confirmation:

`0x01`

Master receives this address and confirms by sending that address back to the bus
(if the node sends an invalid address, master will resend the last valid address instead):

`0x01`

The first node receives the confirmation, assigns the to itself and raises it's outgoing daisy
line to `high` so the next node can address itself.

This continues for all nodes on the bus. If the bus is quiet for too long (timeout), master will
assume that all nodes have been addressed and it closes the address command by repeating `0xFF` twice:

`0xFF 0xFF`

In summary, the addressing looks like this for two slave nodes:

```
master -> 0xFF 0xFF 0x03 0x00 <Address Command> 0x00 0x02
master -> {next daisy pin high}
master -> 0x00
node1  -> 0x01
master -> 0x01
node1  -> {next daisy pin high}
node2  -> 0x02
master -> 0x02
node2  -> {next daisy pin high}
master -> 0xFF 0xFF
```

If node2 initially sent the wrong address, it would look like this:

```
master -> 0xFF 0xFF 0x03 0x00 <Address Command> 0x00 0x02
master -> {next daisy pin high}
master -> 0x00
node1  -> 0x01
master -> 0x01
node1  -> {next daisy pin high}
node2  -> 0x05 (invalid address, not 1 greater than the previous)
master -> 0x00 (master correcting node)
master -> 0x01 (last valid address)
node2  -> 0x02 (node responding with correct address)
master -> 0x02 (master confirming correct address)
node2  -> {next daisy pin high}
master -> 0xFF 0xFF
```

If master is never able to correct a node sending the wrong address, master closes the addressing message (`0x00 0x00`) and the entire addressing operation ends in error status.

### Another ending option
The last node could also connect it's outgoing daisy line back to master, so that it
will be alerted when all nodes have been addressed. (this is not implemented)

### Fun Fact
The direction of the daisy lines are determined dynamically. The node will wait
to see which side goes `high` and then assume that this side is the input line.
Note that it will save this direction to EEPROM and if a node is switched, master
will need to send a reset message to reset the direction detection.

