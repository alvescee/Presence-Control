/**
 * @author Cauã Alves
 * @link https://github.com/alvescee/Presence-Control
*/

// Imports

#include <SPI.h>
#include <MFRC522.h>
#include <string.h>
#include <set>
#include <stdexcept>

// Get pin

#define RST_PIN 22
#define SS_PIN 5

// On when card read yet
#define READ_LED 2

// On when card was read sucessful
#define SUCESS_LED 4

// Static variables

MFRC522 rfid(SS_PIN, RST_PIN);

String identifier = "";

std::set<String> cards;

// Injection

void setSessionID();
void saveCard();

/**
 * Default functions
*/

void setup() {

    // By default
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();

    // Say info
    pinMode(SUCESS_LED, OUTPUT);
    pinMode(READ_LED, OUTPUT);

}

void loop() {

    // The card already or not have card in the rfid
    if (!(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())) return;

    try {
        // Read card
        setSessionID();

        // Save card in ram
        saveCard();

        digitalWrite(SUCESS_LED, HIGH);
        delay(2000);
        digitalWrite(SUCESS_LED, LOW);
    } 
    
    catch (const std::runtime_error& e) {
        digitalWrite(READ_LED, HIGH);
        delay(2000);
        digitalWrite(READ_LED, LOW);
    }

    // Finish read
    rfid.PICC_HaltA();
    
}

/**
 * Domain functions
*/

void setSessionID () {

    identifier = "";

    for (byte i = 0; i < rfid.uid.size; i++) {
        identifier += rfid.uid.uidByte[i], HEX;
    }

}

void saveCard () {
    // Throw exception if the card already read
    if (cards.find(identifier) != cards.end()) throw std::runtime_error("");
    cards.insert(identifier);
}