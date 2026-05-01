#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int screen_width = 128;
int screen_height = 64;

int OLED_reset = -1;

#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 myDisplay(screen_width, screen_height,&Wire,OLED_reset);


void setup() {
  Serial.begin(9600);

  myDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
}
void loop() {
  Serial.println("Scanning...");

  myDisplay.clearDisplay();
  myDisplay.display();
  myDisplay.setCursor(0,0);
  myDisplay.setTextColor(WHITE);
  myDisplay.setTextSize(1);
  myDisplay.println("Hello World");
  myDisplay.println("Number 1");
  myDisplay.display();

  delay(10000);
}