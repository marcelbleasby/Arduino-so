#include <LiquidCrystal.h>

// Definição dos pinos do LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Definição dos pinos dos botões
const int buttonMorsePin = 8;  // Botão para enviar código Morse
const int buttonErasePin = 9; // Botão para apagar o último caractere
const int buttonDecodePin = 10; // Botão para decodificar o código Morse

// Variáveis de controle
unsigned long pressStartTime = 0;
bool buttonPressed = false;
String morseInput = ""; // Armazena o código Morse atual (ex.: "... --- ...")
String decodedWord = ""; // Palavra decodificada

// Mapeamento de código Morse para caracteres
const char* morseTable[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", // A-J
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",   // K-T
  "..-", "...-", ".--", "-..-", "-.--", "--.."                         // U-Z
};
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Estrutura de Tarefa
struct Task {
  void (*taskFunction)();
  unsigned long lastRun;
  unsigned long interval;
};

// Declaração das tarefas
void handleMorseInput();
void handleEraseInput();
void handleDecodeInput();
void updateDisplay();

Task tasks[] = {
  {handleMorseInput, 0, 50}, // Tarefa para entrada de Morse (50ms)
  {handleEraseInput, 0, 50}, // Tarefa para apagar entrada (50ms)
  {handleDecodeInput, 0, 50}, // Tarefa para decodificar entrada (50ms)
  {updateDisplay, 0, 500}    // Tarefa para atualizar o display (500ms)
};

const int numTasks = sizeof(tasks) / sizeof(Task);

void setup() {
  pinMode(buttonMorsePin, INPUT_PULLUP);
  pinMode(buttonErasePin, INPUT_PULLUP);
  pinMode(buttonDecodePin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Morse Decoder");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // Escalonador simples para alternar entre as tarefas
  for (int i = 0; i < numTasks; i++) {
    if (currentMillis - tasks[i].lastRun >= tasks[i].interval) {
      tasks[i].taskFunction();
      tasks[i].lastRun = currentMillis;
    }
  }
}

void handleMorseInput() {
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
        morseInput += "."; // Toque curto
      } else {
        morseInput += "-"; // Toque longo
      }
    }
  }
}

void handleEraseInput() {
  if (digitalRead(buttonErasePin) == LOW) {
    delay(200); // Debounce
    if (morseInput.length() > 0) {
      morseInput.remove(morseInput.length() - 1);
    } else if (decodedWord.length() > 0) {
      decodedWord.remove(decodedWord.length() - 1);
    }
  }
}

void handleDecodeInput() {
  if (digitalRead(buttonDecodePin) == LOW) {
    delay(200); // Debounce
    decodeMorse();
  }
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse: ");
  lcd.print(morseInput);

  lcd.setCursor(0, 1);
  lcd.print("Word: ");
  lcd.print(decodedWord);
}

void decodeMorse() {
  for (int i = 0; i < 26; i++) {
    if (String(morseTable[i]) == morseInput) {
      decodedWord += alphabet[i];
      break;
    }
  }
  morseInput = ""; // Limpa o código Morse após decodificar
}