#include "Keypad.h"

// KEYPAD CONFIG
const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// di kiri semua
// HX (dark blue)
// { X X X A } 
// { X X X B } 
// { X X X C } 
// { X X X D }
// ZRX (light blue)
// { X 2 3 A } 
// { X 5 6 B } 
// { X 8 9 C } 
// { X 0 # D }
//byte rowPins[ROWS] = {13, 12, 14, 27};
//byte colPins[COLS] = {26, 25, 33, 32};

// -----------------------------
// ZRX (light blue)
// { X 2 3 A } 
// { X 5 6 B } 
// { X 8 9 C } 
// { X 0 # D }
// HX (dark blue) -> WORKS!
// { 1 2 3 A } 
// { 4 5 6 B }
// { 7 8 9 C }
// { * 0 # D }
//byte rowPins[ROWS] = {13, 12, 14, 27};
//byte colPins[COLS] = {19, 25, 33, 15};
// -----------------------------
// HX (dark blue)
// { 1 2 3 A } 
// { 4 5 6 B }
// { 7 8 9 C }
// { * 0 # D }
// revisi pindahkan pin 19 karena digunakan RFID reader
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 15};

// https://forum.arduino.cc/t/3x4-keypad-using-esp32-strange-behavior/985196/3
// { X X 3 A } 
// { X 5 6 B } 
// { X 8 9 C } 
// { X 0 # D }
//byte rowPins[ROWS] = {19, 21, 22, 23};
//byte colPins[COLS] = {13, 12, 14, 27};

// https://diyi0t.com/keypad-arduino-esp8266-esp32/
// DARK BLUE
// { X 2 3 A } 
// { X 5 6 B }
// { X X 9 C }
// { X 0 # D }
// LIGHT BLUE -> wait a couple minutes; works on breadboard too! dekatkan dengan pin
// { 1 2 3 A } 
// { 4 5 6 B }
// { 7 8 9 C }
// { * 0 # D }
//byte rowPins[ROWS] = {23, 22, 3, 21}; 
//byte colPins[COLS] = {19, 18, 5, 17};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    Serial.begin(115200);
    Serial.println("[*] Starting keypad...");
}

void loop() {
    char key = keypad.getKey();

    if (key) {
        Serial.print(key);
        Serial.print(" ");
    }
}