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

#include <WiFi.h>
#include <time.h>

// Get pin

#define RST_PIN 22
#define SS_PIN 5

// On when card read yet
#define READ_LED 2

// On when card was read sucessful
#define SUCESS_LED 4

// Static variables

const char* ssid = "";
const char* password = "";

MFRC522 rfid(SS_PIN, RST_PIN);

String identifier = "";

std::set<String> cards;

// 100 minutes * millis (6.000.000)
const int millisDurationClass = 6000000;

// 604.800.000 millis in one week - 100 minutes (598.800.000)
const int millisBetweenClassClass = 604800000 - millisDurationClass;

// Time it class arrive in minute 15:20
const int arriveClass = 920;

// Time it class finish in minute 17:00
const int finishClass = 1020;

/**
 * Injection
*/

void setSessionID();
void saveCard();
void waitTime(int time);

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

    // Conect to wifi

    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) delay(1000);

    // Config hour now
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    // Put to waiting until friday in the arrive of the class

    struct tm timeinfo;

    while (!getLocalTime(&timeinfo, 1000)) delay(200);

    const int timeToNextFriday = (timeinfo.tm_wday * 24 * 60) + (timeinfo.tm_hour * 60) + timeinfo.tm_min;

    // The class already pass in this week
    if (timeToNextFriday > 8220) {
        // Aprox time to sunday 00:00 and add time to class, after get milisseconds
        waitTime((10080 - timeToNextFriday + 8120) * 60000);
    }

    // The class not passed in this week
    else if (timeToNextFriday < 8120) {
        // Find time in millis to next class, after get milisseconds
        waitTime((8120 - timeToNextFriday) * 60000);
    }

}

void loop() {

    waitTime(millisBetweenClassClass);

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

void waitTime (int time) {

    // Time now
    struct tm timeinfo;

    while (!getLocalTime(&timeinfo, 1000)) delay(200);

    // Calculate minutes total of the day now
    int totalHourNow = timeinfo.tm_hour * 60 + timeinfo.tm_min;

    // Today is friday?
    // Now is between 15-17?
    if (timeinfo.tm_wday == 5 && totalHourNow > arriveClass && totalHourNow < finishClass) return;

    cards.clear();

    delay(time);
}