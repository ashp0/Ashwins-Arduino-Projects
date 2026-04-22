#include <Arduino.h>

const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

// Keep track of what color the LED currently is
int currentR = 0;
int currentG = 0;
int currentB = 0;

// Keep track of what color the LED is trying to fade into
int targetR = 0;
int targetG = 0;
int targetB = 0;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // This reads background noise from an empty pin to generate TRULY random numbers
  randomSeed(analogRead(A0)); 
  
  // Pick the very first color to fade toward
  pickNewTargetColor();
}

void loop() {
  // 1. Check if we have successfully reached the target color
  if (currentR == targetR && currentG == targetG && currentB == targetB) {
    // Hang out on this color for half a second, then pick a new one
    delay(500); 
    pickNewTargetColor();
  }

  // 2. Nudge the current colors 1 step closer to the target colors
  if (currentR < targetR) currentR++;
  if (currentR > targetR) currentR--;
  
  if (currentG < targetG) currentG++;
  if (currentG > targetG) currentG--;
  
  if (currentB < targetB) currentB++;
  if (currentB > targetB) currentB--;

  // 3. Update the LED with the new slightly-shifted color
  analogWrite(redPin, currentR);
  analogWrite(greenPin, currentG);
  analogWrite(bluePin, currentB);

  // 4. Wait a tiny bit before taking the next step. 
  // Decrease this number to make the fade faster, increase it to make it slower.
  delay(10); 
}

// Custom function to pick vibrant, saturated random colors
void pickNewTargetColor() {
  int colorMode = random(0, 3);       // Pick a random number: 0, 1, or 2
  int randomMix = random(0, 256);     // Pick a random brightness: 0 to 255

  // By keeping one pin at 255 and one at 0, we guarantee a rich, vibrant color.
  if (colorMode == 0) {
    targetR = 255; 
    targetG = randomMix; 
    targetB = 0;
  } 
  else if (colorMode == 1) {
    targetR = 0; 
    targetG = 255; 
    targetB = randomMix;
  } 
  else {
    targetR = randomMix; 
    targetG = 0; 
    targetB = 255;
  }
}