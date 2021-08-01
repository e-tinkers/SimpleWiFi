#include <Arduino.h>
/**************************************************************
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * NOTE:
 * Some of the functions may be unavailable for your modem.
 * Just comment them out.
 *
 **************************************************************/

// Select your modem:
#define SIMPLE_WIFI_MODEM_ESP8266
// #define SIMPLE_WIFI_MODEM_XBEE

// Use Hardware Serial if not ATmega328P-based MCU
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

// Define the serial console for debug prints, if needed
#define SIMPLE_WIFI_DEBUG Serial

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 57600

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define SIMPLE_WIFI_YIELD() { delay(2); }

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

// Server details to test TCP/SSL
const char server[]   = "httpbin.org";
const char resource[] = "/get";

#include <SimpleWiFiClient.h>

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
