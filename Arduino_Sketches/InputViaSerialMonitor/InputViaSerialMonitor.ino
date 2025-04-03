#include <Keypad.h>

const int keys[] = {10, 9, 8, 7, 6, 5, 4, 3, 2};
const int keyValues[] = {1, 2, 7, 3, 4, 5, 6, 8, 0};
const int numKeys = sizeof(keys) / sizeof(keys[0]);
int pressed_key = -1;

String arr[][2] = {
  {"A", "1"},
  {"B", "1,3"},
  {"C", "1,2"},
  {"D", "1,2,4"},
  {"E", "1,4"},
  {"F", "1,2,3"},
  {"G", "1,2,3,4"},
  {"H", "1,3,4"},
  {"I", "2,3"},
  {"J", "2,3,4"},
  {"K", "1,5"},
  {"L", "1,3,5"},
  {"M", "1,2,5"},
  {"N", "1,2,4,5"},
  {"O", "1,4,5"},
  {"P", "1,2,3,5"},
  {"Q", "1,2,3,4,5"},
  {"R", "1,3,4,5"},
  {"S", "2,3,5"},
  {"T", "2,3,4,5"},
  {"U", "1,5,6"},
  {"V", "1,3,5,6"},
  {"W", "2,3,4,6"},
  {"X", "1,2,5,6"},
  {"Y", "1,2,4,5,6"},
  {"Z", "1,4,5,6"}
};

const int arrSize = sizeof(arr)/sizeof(arr[0]);
String inputLetter = "";
String inputWord = "";

void setup() {
  for (int i = 0; i < numKeys; i++) {
    pinMode(keys[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  inputLetter.reserve(32);
  inputWord.reserve(128);
}

void checkPressedKey() {
  pressed_key = -1;
  for (int i = 0; i < numKeys; i++) {
    if (digitalRead(keys[i]) == LOW) {
      pressed_key = keyValues[i];
      break;
    }
  }
}

bool compareSets(String input, String pattern) {
  // Split input
  String inputParts[10];
  int inputCount = 0;
  int start = 0;
  int commaIndex;
  
  do {
    commaIndex = input.indexOf(',', start);
    if (commaIndex == -1) commaIndex = input.length();
    inputParts[inputCount++] = input.substring(start, commaIndex);
    start = commaIndex + 1;
  } while (commaIndex < input.length() && inputCount < 10);

  // Split pattern
  String patternParts[10];
  int patternCount = 0;
  start = 0;
  
  do {
    commaIndex = pattern.indexOf(',', start);
    if (commaIndex == -1) commaIndex = pattern.length();
    patternParts[patternCount++] = pattern.substring(start, commaIndex);
    start = commaIndex + 1;
  } while (commaIndex < pattern.length() && patternCount < 10);

  // Compare counts
  if (inputCount != patternCount) {
    return false;
  }

  // Compare elements
  for (int i = 0; i < patternCount; i++) {
    bool found = false;
    for (int j = 0; j < inputCount; j++) {
      if (patternParts[i] == inputParts[j]) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

void sendLetter() {
  if (inputLetter.length() == 0) {
    return;
  }

  bool found = false;
  for (int i = 0; i < arrSize; i++) {
    if (compareSets(inputLetter, arr[i][1])) {
      Serial.print(arr[i][0]);
      inputWord += arr[i][0];
      found = true;
      break;
    }
  }

  if (!found) {
    inputWord += " ";
  }
}

void loop() {
  checkPressedKey();
  
  if (pressed_key != -1) {
    delay(50); // Debounce delay

    // Wait for key release
    while (digitalRead(keys[getKeyIndex(pressed_key)]) == LOW) {
      delay(10);
    }

    // Handle key press
    if (pressed_key >= 1 && pressed_key <= 6) { // Number keys
      if (inputLetter.length() > 0) {
        inputLetter += ",";
      }
      inputLetter += String(pressed_key);
    }
    else if (pressed_key == 7) { // Submit
      sendLetter();
      inputLetter = "";
    }
    else if (pressed_key == 8) { // Clear
      inputLetter = "";
      inputWord = "";
      Serial.println("Input cleared");
    }
    else if (pressed_key == 0) { // Space
      Serial.print(" ");
      inputWord += "_";
    }
    
    delay(50); 
  }
}

int getKeyIndex(int value) {
  for (int i = 0; i < numKeys; i++) {
    if (keyValues[i] == value) {
      return i;
    }
  }
  return -1;
}