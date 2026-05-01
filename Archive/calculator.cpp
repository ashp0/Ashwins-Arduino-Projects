#include <Arduino.h>
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

int firstDigitButton = 12;
int secondDigitButton = 10;
int operatorButton = 5;
int equalsButton = 3;

int currentOperator = 0; // 0 for none, 1 for +, 2 for -, 3 for *, 4 for /
int firstDigit = 0;
int secondDigit = 0;

int firstOperatingNumber = 0;
int secondOperatingNumber = 0;
int resultNumber = 0;

bool secondNumberInput = false;

// Macro expression to convert a single digit into its array representation
#define DIGIT_TO_ARRAY(digit) (digit == 0 ? number0 : \
                             digit == 1 ? number1 : \
                             digit == 2 ? number2 : \
                             digit == 3 ? number3 : \
                             digit == 4 ? number4 : \
                             digit == 5 ? number5 : \
                             digit == 6 ? number6 : \
                             digit == 7 ? number7 : \
                             digit == 8 ? number8 : \
                             digit == 9 ? number9 : number0)

byte number0[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0}
};

byte number1[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1}
};

byte number2[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1}
};

byte number3[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0}
};

byte number4[8][6] = {
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0}
};

byte number5[8][6] = {
    {0, 1, 1, 1, 1, 1},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0}
};

byte number6[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0}
};

byte number7[8][6] = {
    {0, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0}
};

byte number8[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0}
};

byte number9[8][6] = {
    {0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0}
};

byte additionOperator[8][12] = {
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}
};

byte subtractionOperator[8][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

byte multiplicationOperator[8][12] = {
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0}
};

byte divisionOperator[8][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

byte result[8][12];

void displayCombinedNumbers(byte number1[8][6], byte number2[8][6]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 6; j++) {
            result[i][j] = number1[i][j];
            result[i][j + 6] = number2[i][j];
        }
    }
    matrix.loadPixels((uint8_t*)result, sizeof(result));
}

void displayMathOperator(int mathOperator) {
    byte* operatorArray;
    switch (mathOperator) {
        case 1:
            operatorArray = (byte*)additionOperator;
            break;
        case 2:
            operatorArray = (byte*)subtractionOperator;
            break;
        case 3:
            operatorArray = (byte*)multiplicationOperator;
            break;
        case 4:
            operatorArray = (byte*)divisionOperator;
            break;
        default:
            return; // Invalid operator
    }
    matrix.loadPixels(operatorArray, sizeof(additionOperator));
}

void setup() {
    matrix.begin();

    pinMode(firstDigitButton, INPUT_PULLUP);
    pinMode(secondDigitButton, INPUT_PULLUP);
    pinMode(operatorButton, INPUT_PULLUP);
    pinMode(equalsButton, INPUT_PULLUP);

    displayCombinedNumbers(number0, number0);
}

void loop() {
  // Not pressed = 1, pressed = 0 because of INPUT_PULLUP
  int firstButtonState = digitalRead(firstDigitButton);
  int secondButtonState = digitalRead(secondDigitButton);
  int operatorButtonState = digitalRead(operatorButton);
  int equalsButtonState = digitalRead(equalsButton);

  if (equalsButtonState == LOW) {
    secondOperatingNumber = secondDigit * 10 + firstDigit; // Convert digits to number
    switch (currentOperator) {
      case 1: // Addition
        resultNumber = firstOperatingNumber + secondOperatingNumber;
        break;
      case 2: // Subtraction
        resultNumber = firstOperatingNumber - secondOperatingNumber;
        break;
      case 3: // Multiplication
        resultNumber = firstOperatingNumber * secondOperatingNumber;
        break;
      case 4: // Division
        if (secondOperatingNumber != 0) {
          resultNumber = firstOperatingNumber / secondOperatingNumber;
        } else {
          // Handle division by zero case (e.g., display an error or set result to a specific value)
          resultNumber = 0; // For simplicity, set to 0
        }
        break;
      default:
        resultNumber = 0; // No operator selected, default to 0
    }

    // Display the result (assuming result is a two-digit number for simplicity)
    int resultFirstDigit = (resultNumber / 10) % 10; // Get the tens digit
    int resultSecondDigit = resultNumber % 10; // Get the units digit
    displayCombinedNumbers(DIGIT_TO_ARRAY(resultFirstDigit), DIGIT_TO_ARRAY(resultSecondDigit));

    // Reset everything
    currentOperator = 0;
    firstDigit = 0;
    secondDigit = 0;
    firstOperatingNumber = 0;
    secondOperatingNumber = 0;
    secondNumberInput = false;
  } else if (operatorButtonState == LOW) {
    displayMathOperator((currentOperator % 4) + 1); // Cycle through operators
    currentOperator = (currentOperator + 1) % 4; // Update current operator
    delay(400); // Short delay to allow operator display to be visible
    displayCombinedNumbers(number0, number0); // Clear digits when operator is selected
    secondNumberInput = true; // Start input for second number
  } else if (secondButtonState == LOW) {
    if (secondNumberInput) {
      firstOperatingNumber = secondDigit * 10 + firstDigit; // Convert digits to number
      secondDigit = 0; // Reset second digit for new input
      firstDigit = 0; // Reset first digit for new input
      secondNumberInput = false; // Reset second number input flag
    }
    secondDigit = (secondDigit + 1) % 10; // Cycle through digits 0-9
    displayCombinedNumbers(DIGIT_TO_ARRAY(firstDigit), DIGIT_TO_ARRAY(secondDigit));
  } else if (firstButtonState == LOW) {
    if (secondNumberInput) {
      firstOperatingNumber = secondDigit * 10 + firstDigit; // Convert digits to number
      secondDigit = 0; // Reset second digit for new input
      firstDigit = 0; // Reset first digit for new input
      secondNumberInput = false; // Reset second number input flag
    }

    firstDigit = (firstDigit + 1) % 10; // Cycle through digits 0-9
    displayCombinedNumbers(DIGIT_TO_ARRAY(firstDigit), DIGIT_TO_ARRAY(secondDigit));
  }

  delay(225); // Debounce delay
}