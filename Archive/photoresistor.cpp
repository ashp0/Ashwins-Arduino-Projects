#include <Arduino.h>
#include "Servo.h"

#define VOLTAGE_OUT_PIN A3
#define SERVO_CONTROL_PIN 3

float analogReadVoltageRange;
float voltageOut;
float staticResistorValue = 10000;
float photoresistorValue;

Servo myServo;

void sunAngle(float voltageOut);

void setup() {
    myServo.attach(SERVO_CONTROL_PIN);

    pinMode(VOLTAGE_OUT_PIN, INPUT);
    Serial.begin(9600);
}

void loop() {
    analogReadVoltageRange = analogRead(VOLTAGE_OUT_PIN);
    voltageOut = analogReadVoltageRange * (5.0 / 1023.0);

    photoresistorValue = (staticResistorValue * 5) / voltageOut - staticResistorValue;

    sunAngle(voltageOut);
    // V_out = (staticResistorValue * 5) / (Rp + staticResistorValue)
    // Rp = (staticResistorValue * 5) / V_out - staticResistorValue;
    Serial.print(photoresistorValue);
    Serial.print(", ");
    Serial.println(voltageOut);
    delay(1000);
}

void sunAngle(float voltageOut) {
    int angle;

    if (voltageOut >= 4.65) {
        // Sunny
        angle = 180;    
    } else if (voltageOut >= 4.20) {
        // Cloudy
        angle = 135;
    } else if (voltageOut >= 3.5) {
        // Rainy
        angle = 90;
    } else if (voltageOut >= 0) {
        // Night/Dark
        angle = 45;
    }

    myServo.write(angle);
}