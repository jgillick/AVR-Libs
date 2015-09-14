#include <avr/io.h>

#include "../Serial.h"

int main(void){

  Serial.begin(9600);

  while(1) {
    if (Serial.available()) {
      Serial.print("\nReceived: ");
      while (Serial.available()) {
        Serial.write(Serial.read());
        Serial.print("\n");
      }
    }
  }
  return -1;
}