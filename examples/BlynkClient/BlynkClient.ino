/**************************************************************
 *
 * For this example, you need to install Blynk library:
 *   https://github.com/blynkkk/blynk-library/releases/latest
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 **************************************************************
 *
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 * Blynk supports many development boards with WiFi, Ethernet,
 * GSM, Bluetooth, BLE, USB/Serial connection methods.
 * See more in Blynk library examples and community forum.
 *
 *                http://www.blynk.io/
 *
 * Change GPRS apm, user, pass, and Blynk auth token to run :)
 **************************************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
// #define BLYNK_HEARTBEAT 30

// Select your modem:
#define SIMPLE_WIFI_MODEM_SIM800
// #define SIMPLE_WIFI_MODEM_SIM808
// #define SIMPLE_WIFI_MODEM_SIM868
// #define SIMPLE_WIFI_MODEM_SIM900
// #define SIMPLE_WIFI_MODEM_SIM7000
// #define SIMPLE_WIFI_MODEM_SIM7000SSL
// #define SIMPLE_WIFI_MODEM_SIM7080
// #define SIMPLE_WIFI_MODEM_SIM5360
// #define SIMPLE_WIFI_MODEM_SIM7600
// #define SIMPLE_WIFI_MODEM_UBLOX
// #define SIMPLE_WIFI_MODEM_SARAR4
// #define SIMPLE_WIFI_MODEM_M95
// #define SIMPLE_WIFI_MODEM_BG96
// #define SIMPLE_WIFI_MODEM_A6
// #define SIMPLE_WIFI_MODEM_A7
// #define SIMPLE_WIFI_MODEM_M590
// #define SIMPLE_WIFI_MODEM_MC60
// #define SIMPLE_WIFI_MODEM_MC60E
// #define SIMPLE_WIFI_MODEM_ESP8266
// #define SIMPLE_WIFI_MODEM_XBEE
// #define SIMPLE_WIFI_MODEM_SEQUANS_MONARCH

#include <SimpleWiFiClient.h>
#include <BlynkSimpleTinyGSM.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif


// Your GPRS credentials, if any
const char apn[]  = "YourAPN";
const char user[] = "";
const char pass[] = "";

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
const char auth[] = "YourAuthToken";

SimpleWiFi modem(SerialAT);

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  Blynk.begin(auth, modem, apn, user, pass);
}

void loop()
{
  Blynk.run();
}
