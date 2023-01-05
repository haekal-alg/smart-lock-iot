#include "Credentials.h"

// ----- BLYNK CONFIG -----
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

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
#include <MFRC522.h>
#include <SPI.h>
#define SS_PIN 5
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// ----- OTHERS -----
#define LOCK_DELAY 5000
bool is_first_time = 1;

BLYNK_WRITE(V0) {
    int LED_value = param.asInt();

    if (LED_value == 1) { 
        Serial.println("[+] Door is unlocked via Blynk");
        // Blynk.virtualWrite(0, 1);
        door_unlock();
        Blynk.virtualWrite(0, 0);
    } 
}

void setup() {
    Serial.begin(115200);

    // Initiate LED
    pinMode(INTERNAL_LED, OUTPUT);
    digitalWrite(INTERNAL_LED, HIGH);

    // Initiate RFID reader
    SPI.begin();
    mfrc522.PCD_Init();

    // Initiate Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

    Serial.println("[+] System has been initialized!");
}

void loop() {
    Blynk.run();

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

    // if input from keypad is detected
    if (key) {
        Serial.print(key);

        if (key == '*') {
            input_password = "";  // reset the input password
            Serial.println();
            Serial.println("[*] Emptied password buffer");
        } else if (key == '#') {
            Serial.println();

            if (!(input_password == KEYPAD_PASSWORD)) {
                failed_attempt(1);
                return;
            }

            Serial.println("[+] Valid Password. DOOR UNLOCKED!");
            door_unlock();
            input_password = "";  // reset the input password

        } else {
            input_password += key;  // append new character to input password string
        }
    }

    // if card near rfid reader is detected
    if (!is_rfid_not_found) {
        String card_uuid = rfid_read();

        Serial.print("\n[*] Detected UID tag: ");
        Serial.println(card_uuid);

        if (!(card_uuid == RFID_PASSWORD)) {
            failed_attempt(2);
            return;
        }

        Serial.println("[+] Authorized tag. DOOR UNLOCK!");
        door_unlock();
    }
}

void door_unlock() {
    digitalWrite(INTERNAL_LED, LOW);  // Unlocks door
    delay(LOCK_DELAY);
    digitalWrite(INTERNAL_LED, HIGH);  // Locks door
    is_first_time = 1;
}

void failed_attempt(int method) {
    /*
    method = 1 -> keypad
    method = 2 -> rfid tag
    */
    if (method == 1) {
        Serial.println("[-] Invalid Password. ACCESS DENIED!");
    }
    else if (method == 2){
        Serial.println("[-] Unauthorized tag. ACCESS DENIED!");
    }
    
    is_first_time = 1;
}

String rfid_read() {
    String content = "";
    // read the data byte per byte
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();

    return content.substring(1);
}
