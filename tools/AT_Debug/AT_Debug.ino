/**************************************************************
 *
 * This script tries to auto-detect the baud rate
 * and allows direct AT commands access
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 **************************************************************/

// Select your modem:
#define SIMPLE_WIFI_MODEM_SIM800
// #define SIMPLE_WIFI_MODEM_SIM900
// #define SIMPLE_WIFI_MODEM_SIM808
// #define SIMPLE_WIFI_MODEM_SIM868
// #define SIMPLE_WIFI_MODEM_UBLOX
// #define SIMPLE_WIFI_MODEM_M95
// #define SIMPLE_WIFI_MODEM_BG96
// #define SIMPLE_WIFI_MODEM_A6
// #define SIMPLE_WIFI_MODEM_A7
// #define SIMPLE_WIFI_MODEM_M590
// #define SIMPLE_WIFI_MODEM_MC60
// #define SIMPLE_WIFI_MODEM_MC60E
// #define SIMPLE_WIFI_MODEM_ESP8266
// #define SIMPLE_WIFI_MODEM_XBEE

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

#define SIMPLE_WIFI_DEBUG SerialMon

#include <SimpleWiFiClient.h>

// Module baud rate
uint32_t rate = 0; // Set to 0 for Auto-Detect

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(6000);
}

void loop() {

  if (!rate) {
    rate = SimpleWiFiAutoBaud(SerialAT);
  }

  if (!rate) {
    SerialMon.println(GF("***********************************************************"));
    SerialMon.println(GF(" Module does not respond!"));
    SerialMon.println(GF("   Check your Serial wiring"));
    SerialMon.println(GF("   Check the module is correctly powered and turned on"));
    SerialMon.println(GF("***********************************************************"));
    delay(30000L);
    return;
  }

  SerialAT.begin(rate);

  // Access AT commands from Serial Monitor
  SerialMon.println(GF("***********************************************************"));
  SerialMon.println(GF(" You can now send AT commands"));
  SerialMon.println(GF(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  SerialMon.println(GF(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  SerialMon.println(GF("***********************************************************"));

  while(true) {
    if (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    if (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
    delay(0);
  }
}
