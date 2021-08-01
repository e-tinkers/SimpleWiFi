/**************************************************************
 *
 * This script tries to auto-detect the baud rate
 * and allows direct AT commands access
 *
 * TinyGSM Getting Started guide:
 *   https://github.com/e-tinkers/SimpleWiFi
 *
 **************************************************************/

// Select your modem:
#define SIMPLE_WIFI_MODEM_ESP8266
// #define SIMPLE_WIFI_MODEM_XBEE

// Set serial for AT command interface
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

#define SIMPLE_WIFI_DEBUG Serial

#include <SimpleWiFiClient.h>

// Module baud rate
uint32_t rate = 0; // Set to 0 for Auto-Detect

void setup() {
  Serial.begin(115200);
  delay(2000);
}

void loop() {

  if (!rate) {
    rate = SimpleWiFiAutoBaud(SerialAT);
  }

  if (!rate) {
    Serial.println(GF("***********************************************************"));
    Serial.println(GF(" Module does not respond!"));
    Serial.println(GF("   Check your Serial wiring"));
    Serial.println(GF("   Check the module is correctly powered and turned on"));
    Serial.println(GF("***********************************************************"));
    delay(30000L);
    return;
  }

  SerialAT.begin(rate);

  // Access AT commands from Serial Monitor
  Serial.println(GF("***********************************************************"));
  Serial.println(GF(" You can now send AT commands"));
  Serial.println(GF(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  Serial.println(GF(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  Serial.println(GF("***********************************************************"));

  while(true) {
    if (SerialAT.available()) {
      Serial.write(SerialAT.read());
    }
    if (Serial.available()) {
      SerialAT.write(Serial.read());
    }
    delay(1);
  }
}
