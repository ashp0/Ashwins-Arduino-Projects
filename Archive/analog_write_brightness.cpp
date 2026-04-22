#include <Arduino.h>

int ledPins[] = {3, 4, 6, 8};
int numPins = 4;

void setup() {
  for (int i = 0; i < numPins; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {
  digitalWrite(ledPins[1], HIGH);

  for (float i = 0; i <= 254; i += 5) {
    analogWrite(ledPins[0], i);
    Serial.println(i);
    delay(300);
  }

  delay(1000);
}