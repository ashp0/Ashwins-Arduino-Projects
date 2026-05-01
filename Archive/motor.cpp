#include <Arduino.h>

const int forwards_input_pin = 3;   // PWM pin
const int backwards_input_pin = 5;  // PWM pin


int direction = 1; // 1 = forward, -1 = backward


// INPUT: Potentiometer should be connected to 5V and GND
int potPin = A3; // Potentiometer output connected to analog pin 3
int speedVal = 0;

int direction_button_pin = 11;

void setup() {
    Serial.begin(9600);

    pinMode(forwards_input_pin, OUTPUT);
    pinMode(backwards_input_pin, OUTPUT);
    pinMode(potPin, INPUT);
    pinMode(direction_button_pin, INPUT_PULLUP);
}

void setMotor(int speed, int dir) {
    if (dir == 1) {
        analogWrite(forwards_input_pin, speed);
        analogWrite(backwards_input_pin, 0);
    } else {
        analogWrite(forwards_input_pin, 0);
        analogWrite(backwards_input_pin, speed);
    }
}

void loop() {
    speedVal = analogRead(potPin) * (255.0/1023.0);
    Serial.println(analogRead(A3));

    if (digitalRead(direction_button_pin) == LOW) {
        setMotor(0, direction);
        direction = direction == 1 ? -1 : 1;
        delay(250);
        setMotor(speedVal, direction);
    } else {
    // Accelerate
    setMotor(speedVal, direction);
    delay(250);
    }

}