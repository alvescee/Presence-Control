/**
 * @author Cauã Alves
 * @link https://github.com/alvescee/Presence-Control
*/

// Imports

#include <SPI.h>
#include <MFRC522.h>
#include <string.h>

// Get pin

#define RST_PIN 22
#define SS_PIN 5

// Static variables

MFRC522 rfid(SS_PIN, RST_PIN);

String identifier = "";

// Injection

void setSessionID();

/**
 * Default functions
*/

void setup() {

    // By default
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();

}

void loop() {

    // The card already or not have card in the rfid
    if (!(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())) return;

    // Read card and save in session
    setSessionID();
    Serial.println(identifier);

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