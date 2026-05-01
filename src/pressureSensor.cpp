#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

int screen_width = 128;
int screen_height = 64;
int OLED_reset = -1;
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 myDisplay(screen_width, screen_height, &Wire, OLED_reset);

Adafruit_BMP085 myBMP;

// In Pascals
float air_pressure;

// In Celcius
float temperature;

// Im Altitude
float altitude;

// Whether the device is connected or not.
bool bmp_connected;

void setup() {
	Serial.begin(9600);

	// Initialize Display
	myDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
	myDisplay.clearDisplay();
	myDisplay.display();
	myDisplay.setCursor(0, 0);
	myDisplay.setTextColor(WHITE);
	myDisplay.setTextSize(1);

	bmp_connected = myBMP.begin();

	String message;

	if (bmp_connected) {
		message = "Found BMP Device!";
	} else {
		message = "Error: Unable to find the BMP sensor or unable to begin.";
	}

	myDisplay.println(message);
	myDisplay.display();
}

void loop() {
	temperature = myBMP.readTemperature();
	air_pressure = myBMP.readPressure();
	altitude = myBMP.readAltitude();

	myDisplay.setCursor(0, 0);
	myDisplay.clearDisplay();
	myDisplay.print(temperature);
	myDisplay.println(" C");
	myDisplay.print(altitude);
	myDisplay.println(" m");
	myDisplay.print(air_pressure);
	myDisplay.println(" Pa");
	myDisplay.display();

	delay(2000);
}
