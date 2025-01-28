#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


const int buttonMorsePin = 8;  
const int buttonErasePin = 9;

unsigned long pressStartTime = 0;
bool buttonPressed = false;
String morse_code = ""; 
String text_translated = "";

const char* morseTable[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", // A-J
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",   // K-T
  "..-", "...-", ".--", "-..-", "-.--", "--.."                         // U-Z
};
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void setup() {
  pinMode(buttonMorsePin, INPUT_PULLUP);
  pinMode(buttonErasePin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Codigo Morse");
  delay(2000);
  lcd.clear();
  updateDisplay();
}

void loop() {
  handlemorse_code();
  handleEraseInput();
}

void handlemorse_code() {
  if (digitalRead(buttonMorsePin) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      pressStartTime = millis();
    }
  } else {
    if (buttonPressed) {
      buttonPressed = false;
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration < 300) {
        morse_code += "."; 
      } else {
        morse_code += "-";
      }
      updateDisplay();
    }
  }
}

void handleEraseInput() {
  if (digitalRead(buttonErasePin) == LOW) {
    delay(200); 
    if (morse_code.length() > 0) {
      morse_code.remove(morse_code.length() - 1);
    } else if (text_translated.length() > 0) {
      text_translated.remove(text_translated.length() - 1);
    }
    updateDisplay();
  }
}

void decodeMorse() {
  for (int i = 0; i < 26; i++) {
    if (String(morseTable[i]) == morse_code) {
      text_translated += alphabet[i];
      break;
    }
  }
  morse_code = ""; 
  updateDisplay();
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse: ");
  lcd.print(morse_code);

  lcd.setCursor(0, 1);
  lcd.print("Word: ");
  lcd.print(text_translated);

  if (morse_code.endsWith(" ")) {
    morse_code.remove(morse_code.length() - 1);
    decodeMorse();
  }
}
