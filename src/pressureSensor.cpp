#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

const int configure_base_pin = 6;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long BUTTON_DEBOUNCE_MS = 10;

float base_pressure;
bool base_pressure_set = false;
float height_filtered = 0;

unsigned long lastDebounceTimeBMP = 0;
const unsigned long BMP_SAMPLE_INTERVAL_MS = 693;

// Constants
constexpr float TEMP_AT_SEA_LEVEL = 288.15f;			   // K (15°C)
constexpr float MOLAR_MASS_OF_AIR = 0.0289644f;			   // kg/mol
constexpr float ACCELERATION_GRAVITY = 9.80665f;		   // m/s^2
constexpr float UNIVERSAL_GAS_CONSTANT = 8.3144598f;	   // J/(mol·K)
constexpr float TEMPERATURE_CHANGE_BY_ALTITUDE = 0.0065f;  // K/m

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET_PIN = -1;
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 myDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

Adafruit_BMP085 myBMP;

// In Pascals
float air_pressure;

// In Celcius
float temperature;

// Im Altitude
float altitude;

// Whether the device is connected or not.
bool bmp_connected;

float computeChangeInAltitude(float currentPressure, float basePressure) {
	float left_term = TEMP_AT_SEA_LEVEL / TEMPERATURE_CHANGE_BY_ALTITUDE;
	float power = (UNIVERSAL_GAS_CONSTANT * TEMPERATURE_CHANGE_BY_ALTITUDE) /
				  (MOLAR_MASS_OF_AIR * ACCELERATION_GRAVITY);
	float pressure_ratio = pow(currentPressure / basePressure, power);
	return left_term * (1 - pressure_ratio);
}

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
		message = F("Found BMP Device!");
	} else {
		message = F("Error: Unable to find the BMP sensor or unable to begin.");
	}

	myDisplay.println(message);
	myDisplay.display();

	pinMode(configure_base_pin, INPUT_PULLUP);
}

void loop() {
	if (!bmp_connected)
		return;

	bool didPressReconfigureButton = digitalRead(configure_base_pin);
	unsigned long millis_time = millis();

	if (didPressReconfigureButton != lastButtonState) {
		if (millis_time - lastDebounceTime > BUTTON_DEBOUNCE_MS) {
			if (didPressReconfigureButton == LOW) {
				base_pressure = air_pressure;
				base_pressure_set = true;
			}
			lastDebounceTime = millis_time;
		}
	}
	lastButtonState = didPressReconfigureButton;

	if (millis_time - lastDebounceTimeBMP > BMP_SAMPLE_INTERVAL_MS) {
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

		if (base_pressure_set) {
			myDisplay.println("--------");
			myDisplay.print("h: ");
			float height_now = computeChangeInAltitude(air_pressure, base_pressure);

			// Low pass filter
			height_filtered = 0.98 * height_filtered + 0.02 * height_now;

			myDisplay.print(height_filtered);
			myDisplay.println(" m");
		}

		lastDebounceTimeBMP = millis_time;
		myDisplay.display();
	}
}
