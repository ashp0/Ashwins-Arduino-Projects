#include <Arduino.h>
#include "Arduino_LED_Matrix.h"
#include "ArduinoGraphics.h"

const int echo_pin = 3;
const int trig_pin = 4;

long ping_time;
float distance_cm;

ArduinoLEDMatrix matrix;

void setup() {
    pinMode(echo_pin, INPUT);
    pinMode(trig_pin, OUTPUT);
    Serial.begin(9600);

    matrix.begin();
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF); // white
    matrix.textScrollSpeed(50); // smooth scrolling
    matrix.endDraw();
}

void loop() { 
    // Ensure clean trigger
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);

    // Send 10 µs pulse
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    // Read echo time (with timeout for stability)
    ping_time = pulseIn(echo_pin, HIGH, 1000000); // 1s timeout

    // Convert to distance (cm)
    distance_cm = (ping_time * 0.0343) / 2;

    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");

        // Display on LED matrix
    matrix.beginDraw();
    matrix.clear();

    matrix.textFont(Font_5x7);
    matrix.beginText(0, 1, 0xFFFFFFFF);

    matrix.print((int)distance_cm); // show whole number

    matrix.endText();
    matrix.endDraw();

    

    delay(200);
}