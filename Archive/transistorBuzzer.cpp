#include <Arduino.h>

#define TRANSISTOR_PIN 8

void setup() {
  pinMode(TRANSISTOR_PIN, OUTPUT);
}

void loop() {
    while (true) {
  digitalWrite(TRANSISTOR_PIN, HIGH); // Turn on the transistor
  delay(1000); // Wait for 1 second
  digitalWrite(TRANSISTOR_PIN, LOW); // Turn off the transistor
  delay(1000); // Wait for 1 second
    }
}