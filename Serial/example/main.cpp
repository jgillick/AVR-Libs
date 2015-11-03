/*#####################################
 *
 * Echos whatever is received on RX to TX.
 * If you connect an LED to PD5, it will
 * light up as soon as data has been
 * received.
 *
 #####################################*/

#include <avr/io.h>

#include "../Serial.h"

#define LED PD5

int main(void){

  Serial.begin(9600);

  DDRD  |= (1 << LED);

  while(1) {
    if (Serial.available()) {
      PORTD |= (1 << LED);

      Serial.print("\nReceived: ");
      Serial.write(Serial.read());
      Serial.print("\n");
    }
  }
  return -1;
}