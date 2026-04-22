#include <Arduino.h>

int potPin = A2;
int ledPin = 3;

int potVal;
int serialMonitor = 9600;
int waitT = 500;
float volts;

void setup() {
  // put your setup code here, to run once:
  pinMode(potPin, INPUT);
  pinMode(3, OUTPUT);

  Serial.begin(serialMonitor );
}

void loop() {
  // put your main code here, to run repeatedly:
  potVal = analogRead(potPin);
  volts = 255.0/1023.0 * float(potVal);

  analogWrite(ledPin, volts);

  Serial.print(potVal);
  Serial.print(" ");
  Serial.println(volts);
  delay(waitT);
}
