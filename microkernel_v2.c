#include <EEPROM.h>
#include "thread_scheduler.h"
#include "lcd_driver.h"

const int buttonMorsePin = 8;
const int buttonErasePin = 9;
const int buttonDecodePin = 10;


char morseInput[50] = "";
char decodedWord[50] = "";
uint8_t morseIndex = 0;
uint8_t decodedIndex = 0;


const char* const morseTable[] PROGMEM = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
  "..-", "...-", ".--", "-..-", "-.--", "--.."
};
const char alphabet[] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;

Thread morseThread;
Thread eraseThread;
Thread decodeThread;
const SchedulerTask schedulerQueue[] = {
  { &decodeThread, 3 },  
  { &morseThread, 2 },
  { &eraseThread, 1 }    
};

void handleMorseInput();
void handleEraseInput();
void handleDecodeInput();
void saveToEEPROM();
void loadFromEEPROM();
void scheduler();

void setup() {
  pinMode(buttonMorsePin, INPUT_PULLUP);
  pinMode(buttonErasePin, INPUT_PULLUP);
  pinMode(buttonDecodePin, INPUT_PULLUP);

  LcdDriver::initDisplay();
  loadFromEEPROM();
  
  morseThread.onRun(handleMorseInput);
  eraseThread.onRun(handleEraseInput);
  decodeThread.onRun(handleDecodeInput);
}

void loop() {
  scheduler();
  LcdDriver::updateDisplay(decodedWord, morseInput);
}

void scheduler() {
  for (uint8_t i = 0; i < 3; i++) {
    if (schedulerQueue[i].thread->shouldRun()) {
      schedulerQueue[i].thread->run();
    }
  }
}

void handleMorseInput() {
  static bool buttonPressed = false;
  static unsigned long pressStartTime = 0;
  
  if ((millis() - lastDebounceTime) < debounceDelay) return;
  
  bool currentState = !digitalRead(buttonMorsePin);
  
  if (currentState && !buttonPressed) {
    pressStartTime = millis();
    buttonPressed = true;
    lastDebounceTime = millis();
  }
  else if (!currentState && buttonPressed) {
    buttonPressed = false;
    lastDebounceTime = millis();
    
    if (morseIndex < sizeof(morseInput)-1) {
      unsigned long duration = millis() - pressStartTime;
      morseInput[morseIndex++] = (duration < 200) ? '.' : '-';
      morseInput[morseIndex] = '\0';
    }
  }
}

void handleEraseInput() {
  if ((millis() - lastDebounceTime) < debounceDelay) return;
  
  if (!digitalRead(buttonErasePin)) {
    lastDebounceTime = millis();
    morseIndex = 0;
    decodedIndex = 0;
    morseInput[0] = '\0';
    decodedWord[0] = '\0';
    EEPROM.update(0, 0);
    EEPROM.update(50, 0);
  }
}

void handleDecodeInput() {
  if ((millis() - lastDebounceTime) < debounceDelay) return;
  
  if (!digitalRead(buttonDecodePin)) {
    lastDebounceTime = millis();
    decodedIndex = 0;
    
    char buffer[6] = {0};
    uint8_t bufIndex = 0;
    
    for (uint8_t i = 0; i <= morseIndex; i++) {
      if (morseInput[i] == ' ' || morseInput[i] == '\0') {
        for (uint8_t j = 0; j < 26; j++) {
          char morseCode[6];
          strcpy_P(morseCode, (char*)pgm_read_word(&morseTable[j]));
          
          if (strcmp(buffer, morseCode) == 0) {
            decodedWord[decodedIndex++] = pgm_read_byte(&alphabet[j]);
            break;
          }
        }
        bufIndex = 0;
        memset(buffer, 0, sizeof(buffer));
      } else {
        buffer[bufIndex++] = morseInput[i];
      }
    }
    decodedWord[decodedIndex] = '\0';
    saveToEEPROM();
  }
}

void saveToEEPROM() {
  
  if (strncmp(morseInput, (char*)EEPROM.getConstDataPtr(0), morseIndex) != 0) {
    for (uint8_t i = 0; i <= morseIndex; i++) {
      EEPROM.update(i, morseInput[i]);
    }
  }
  
  if (strncmp(decodedWord, (char*)EEPROM.getConstDataPtr(50), decodedIndex) != 0) {
    for (uint8_t i = 0; i <= decodedIndex; i++) {
      EEPROM.update(50 + i, decodedWord[i]);
    }
  }
}

void loadFromEEPROM() {
  for (uint8_t i = 0; i < 50; i++) {
    morseInput[i] = EEPROM.read(i);
    decodedWord[i] = EEPROM.read(50 + i);
    if (morseInput[i] == '\0') break;
  }
  morseIndex = strlen(morseInput);
  decodedIndex = strlen(decodedWord);
}
