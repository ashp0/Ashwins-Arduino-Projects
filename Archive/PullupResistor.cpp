#include <Arduino.h>

int portLED = 2;
int pushButtonReadPort = 3;

bool ledOn = false;

void setup() {
  pinMode(portLED, OUTPUT);
  // Use INPUT_PULLUP to prevent random behavior
  pinMode(pushButtonReadPort, INPUT_PULLUP); 
  Serial.begin(9600);
}

void loop() {
  int buttonPressed = digitalRead(pushButtonReadPort);

  // With PULLUP, 0 means the button IS pressed
  if (buttonPressed == 0) {
    
    // Toggle the boolean (if true, becomes false; if false, becomes true)
    ledOn = !ledOn;

    // Apply the state to the LED
    if (ledOn == true) {
      digitalWrite(portLED, HIGH);
      Serial.println("LED ON");
    } else {
      digitalWrite(portLED, LOW);
      Serial.println("LED OFF");
    }

    // Wait a moment so one press doesn't count as 100 presses
    delay(250); 
  }
}