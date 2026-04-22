#include <Servo.h>
#include <Arduino.h>

int potPin = A2;
float potVal;
int waitT = 500;
float angle;

int servoPin = 5;
float servoAngle = 0;
int br = 9600;

int delayTime=50;

Servo myServo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(br);
  myServo.attach(servoPin);

  pinMode(potPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  potVal = analogRead(potPin);
  servoAngle = potVal/1023 * 180;

  myServo.write(servoAngle);

  Serial.print(potVal);
  Serial.print(" ");
  Serial.println(servoAngle);
  delay(delayTime);
}
