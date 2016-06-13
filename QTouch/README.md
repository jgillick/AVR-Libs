# Minimal QTouch Proximity Implementation

This is a simple minimalistic implementation of the
[Atmel QTouch library](http://www.atmel.com/tools/QTOUCHLIBRARY.aspx) for the Atmega8 chips.

## Setup

 * Connect an LED from `PB2` to ground.
 * Connect a 22nF capacitor between `PC0` and `PC1`.
 * Connect a 1k resistor between `PC1` and the sensor.
   * The sensor can be a wire or sheet of metal or foil.

## Use

When power is supplied, the LED will turn on until it detects something come within range
of the sensor.

## Configuration

Define your settings in `touch.h` and `touch_control.cpp` based on the settings defined
in the [QTouch Library Documentation](http://www.atmel.com/Images/doc8207.pdf).

## IMPORTANT

When you compile this you MUST include the `-fshort-enums` compilation flag.