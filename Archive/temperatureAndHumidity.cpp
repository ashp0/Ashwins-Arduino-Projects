#include <Arduino.h>
#include "DHT.h"
#include "ArduinoGraphics.h"
#include <Arduino_LED_Matrix.h>

#define DHTPIN 5
#define DHTTYPE DHT11
#define BUTTON_PIN 3

DHT dhtSensor(DHTPIN, DHTTYPE);
ArduinoLEDMatrix matrix;

float tempC, humidity, feelsLike;
String display;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  dhtSensor.begin();
  matrix.begin();
  delay(1000);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    tempC     = dhtSensor.readTemperature(false);
    humidity  = dhtSensor.readHumidity();
    feelsLike = dhtSensor.computeHeatIndex(tempC, humidity, false);

    display = "      " + String(tempC) + "C  " + String(humidity) + "%  Feels: " + String(feelsLike) + "C";

    // Scroll the text across the LED matrix
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF);
    matrix.textScrollSpeed(81);
    matrix.textFont(Font_5x7);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println(display);
    matrix.endText(SCROLL_LEFT);
    matrix.endDraw();

    Serial.print(tempC);
    Serial.print(", ");
    Serial.print(humidity);
    Serial.print(", ");
    Serial.println(feelsLike);
  }

  delay(250);
}