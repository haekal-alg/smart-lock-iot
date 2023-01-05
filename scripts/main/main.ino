// ----- RELAY CONFIG -----
#define INTERNAL_LED 2

// ----- KEYPAD CONFIG -----
#include "Keypad.h"
const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 15};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String input_password;

// ----- RFID CONFIG -----
// Key  UID -> 80 E7 21 21
// Card UID -> 01 5C 11 1C
#include <MFRC522.h>
#include <SPI.h>
#define SS_PIN 5
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// ----- OTHERS -----
#define LOCK_DELAY 5000
bool is_first_time = 1;

// ----- PASSWORDS -----
const String KEYPAD_PASSWORd = "147369";
const String RFID_PASSWORD = "80 E7 21 21";

void setup() {
    Serial.begin(115200);

    // Initiate relay
    pinMode(INTERNAL_LED, OUTPUT);

    // Initiate RFID reader
    SPI.begin();
    mfrc522.PCD_Init();

    Serial.println("[+] System is initialized");
}

void loop() {
    char key = keypad.getKey();

    // check if the RFID reader or keypad detects any input
    bool is_rfid_not_found = !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial();
    bool is_waiting_input = is_rfid_not_found || !key;

    if (is_waiting_input) {
        if (is_first_time) {
            Serial.println("\n[*] Waiting input...");
            is_first_time = 0;
        }
    }

    // if input from keypad detected
    if (key) {
        Serial.print(key);

        if (key == '*') {
            input_password = "";  // reset the input password
            Serial.println();
            Serial.println("[*] Emptied password buffer");
        } else if (key == '#') {
            Serial.println();

            if (!(input_password == KEYPAD_PASSWORd)) {
                Serial.println("[-] Invalid Password. ACCESS DENIED!");
                is_first_time = 1;
                return;
            }

            Serial.println("[+] Valid Password. DOOR UNLOCKED!");
            door_unlock();

            input_password = "";  // reset the input password
            is_first_time = 1;

        } else {
            input_password += key;  // append new character to input password string
        }
    }

    // if input from rfid detected
    if (!is_rfid_not_found) {
        // Show UID on serial monitor
        Serial.print("\n[*] Detected UID tag :");
        String content = "";

        // read the data byte per byte
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);

            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }

        Serial.println();
        content.toUpperCase();

        if (!(content.substring(1) == RFID_PASSWORD)) {
            Serial.println("[-] Unauthorized tag. ACCESS DENIED!");
            is_first_time = 1;
            return;
        }

        Serial.println("[+] Authorized tag. DOOR UNLOCK!");
        door_unlock();
        is_first_time = 1;
    }
}

void door_unlock() {
    digitalWrite(INTERNAL_LED, HIGH);  // unlock the door
    delay(LOCK_DELAY);
    digitalWrite(INTERNAL_LED, LOW);  // lock the door
}