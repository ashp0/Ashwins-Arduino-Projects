#include <Arduino.h>

int ledPins[] = {2, 4, 6, 8};
int numPins = 4;

void setup() {
  for (int i = 0; i < numPins; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  Serial.begin(9600);
}

void configureLEDS(bool first, bool second, bool third, bool fourth) {
  digitalWrite(ledPins[0], first == true ? HIGH : LOW);
  digitalWrite(ledPins[1], second == true ? HIGH : LOW);
  digitalWrite(ledPins[2], third == true ? HIGH : LOW);
  digitalWrite(ledPins[3], fourth == true ? HIGH : LOW);
}

void loop() {
  for (int i = 0; i <= 15; i++) {
    displayNumber(i);
    Serial.println(i);
    delay(300);
  }

  delay(1000);
}

void displayNumber(int number) {
  bool first = false;
  bool second = false;
  bool third = false;
  bool fourth = false;

  for (int i = number; i > 0;) {
    if (i >= 8) {
      first = true;
      i -= 8;
    } else if (i >= 4) {
      second = true;
      i -= 4;
    } else if (i >= 2) {
      third = true;
      i -= 2;
    } else if (i >= 1) {
      fourth = true;
      i -= 1;
    } else {
      i = 0;
    }
  }

  configureLEDS(first, second, third, fourth);
}