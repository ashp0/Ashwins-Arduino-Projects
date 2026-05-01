#include <Arduino.h>

const int pir_sensor_pin = 2;

int has_motion = 0;

void setup() {
    pinMode(pir_sensor_pin, INPUT);
    Serial.begin(9600);
}

void loop() {
    has_motion = digitalRead(pir_sensor_pin);

    if (has_motion == HIGH) {
        Serial.println("Detected Motion!");
    } 
    if (has_motion == LOW) {
        Serial.println("Environment is Constant.");
    }

    delay(100);
}

// Wow, this PIR sensor is really really simple. The only issue is that it cannot detect constant movement, the sensor must first get adjusted to the current environment, then when a sudden change appears will the sensor "detect movement." Constantly swinging my arm around stops triggering it. 