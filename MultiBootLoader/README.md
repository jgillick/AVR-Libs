# MultiBootloader

This is based on the [MultidropBusProtocol](../MultidropBusProtocol/) and is used to program one or more
AVRs on a mutlidrop bus at once.

## Setup 

This receives data on the RX line of the AVR and assumes an additional "signal" wire is connected
in order to notify us of errors.

## Programming Flow

<a href="./flow-diagram.png?raw=true"><img src="flow-diagram.png?raw=true" width="80%" /></a>

## Commands

`0xFF 0xFF 0xF1 0 0 0 0xFF 0xFF`

`0xFF 0xFF 0xF2 <mem addr> <len> <data> <crc>`

`0xFF 0xFF 0xF3 0 0 0`
