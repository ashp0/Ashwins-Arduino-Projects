#include "Stepper.h"
#include <Arduino.h>

const int input_1 = 2;

const int input_2 = 4;

const int input_3 = 3;

const int input_4 = 5;

const int steps = 2038;
int rpm = 15;

Stepper stepper(steps,input_1,input_3,input_2,input_4);

void setup() {

}

void loop() {
stepper.setSpeed(rpm);
stepper.step(steps);
delay(1000);
stepper.setSpeed(19);
stepper.step(-steps);

}