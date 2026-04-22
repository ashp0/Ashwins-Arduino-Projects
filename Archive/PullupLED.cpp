#include <Arduino.h>

int ledRed = 10;
int ledGreen = 9;
int ledBlue = 6; 

int butRed = 2;
int butGreen = 3;
int butBlue = 4;

// RGB brightness values (Starts off white: 255, 255, 255)
int rVal = 255;
int gVal = 255;
int bVal = 255;

// Button state tracking to detect individual presses (Edge Detection)
bool lastRedState = HIGH;
bool lastGreenState = HIGH;
bool lastBlueState = HIGH;

// Variables for the 2-second hold feature
unsigned long allPressedStartTime = 0;
bool isAllPressed = false;
bool randomMode = false;

void setup() {
  Serial.begin(9600);

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  // INPUT_PULLUP means the buttons will read LOW when pressed
  pinMode(butRed, INPUT);
  pinMode(butGreen, INPUT_PULLUP);
  pinMode(butBlue, INPUT_PULLUP);
}

void loop() {
  // Read current button states
  bool currentRedState = digitalRead(butRed);
  bool currentGreenState = digitalRead(butGreen);
  bool currentBlueState = digitalRead(butBlue);

  // --- 1. CHECK FOR 3-BUTTON HOLD (2 SECONDS) ---
  if (currentRedState == LOW && currentGreenState == LOW && currentBlueState == LOW) {
    if (!isAllPressed) {
      isAllPressed = true;
      allPressedStartTime = millis(); // Start the timer
    } else if (millis() - allPressedStartTime >= 2000) {
      randomMode = true; // 2 seconds have passed, trigger the party mode!
    }
  } else {
    isAllPressed = false; // Reset the timer logic if any button is released
  }

  // --- 2. RANDOM LED PATTERN MODE ---
  if (randomMode) {
    // Flash random colors
    analogWrite(ledRed, random(0, 256));
    analogWrite(ledGreen, random(0, 256));
    analogWrite(ledBlue, random(0, 256));
    delay(150); // Speed of the random pattern

    // To exit random mode, release all buttons and press just one
    if (!isAllPressed && (currentRedState == LOW || currentGreenState == LOW || currentBlueState == LOW)) {
      randomMode = false;
      rVal = 255; gVal = 255; bVal = 255; // Reset back to white
      delay(300); // Quick debounce delay
    }
    return; // Skip the rest of the loop while in random mode
  }

  // --- 3. NORMAL BUTTON PRESS LOGIC ---

  // Red Button: Increments by larger chunks
  if (lastRedState == HIGH && currentRedState == LOW) {
    rVal = rVal + 15; 
    if (rVal > 255) rVal = 0; // Wrap around when max is reached
  }

  // Blue Button: Increments "little by little"
  if (lastBlueState == HIGH && currentBlueState == LOW) {
    bVal = bVal + 15; 
    if (bVal > 255) bVal = 0; // Wrap around when max is reached
  }

  // Green Button: Added this just in case you want it to do something too!
  if (lastGreenState == HIGH && currentGreenState == LOW) {
    gVal = gVal + 15; 
    if (gVal > 255) gVal = 0; 
  }

  // Update previous button states for the next loop
  lastRedState = currentRedState;
  lastGreenState = currentGreenState;
  lastBlueState = currentBlueState;

  // --- 4. WRITE TO LEDS ---
  analogWrite(ledRed, rVal);
  analogWrite(ledGreen, gVal);
  analogWrite(ledBlue, bVal);

  delay(20); // Small delay to debounce the buttons (prevents flickering inputs)
}