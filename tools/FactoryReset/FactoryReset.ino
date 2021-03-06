/**************************************************************
 *
 * To run this tool you need StreamDebugger library:
 *   https://github.com/vshymanskyy/StreamDebugger
 *   or from http://librarymanager/all#StreamDebugger
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 **************************************************************/

// Select your modem:
#define SIMPLE_WIFI_MODEM_ESP8266
// #define SIMPLE_WIFI_MODEM_XBEE

#include <SimpleWiFiClient.h>

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
SimpleWiFi modem(debugger);

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(9600);
  delay(6000);

  if (!modem.init()) {
    SerialMon.println(GF("***********************************************************"));
    SerialMon.println(GF(" Cannot initialize modem!"));
    SerialMon.println(GF("   Use File -> Examples -> SimpleWiFi -> tools -> AT_Debug"));
    SerialMon.println(GF("   to find correct configuration"));
    SerialMon.println(GF("***********************************************************"));
    return;
  }

  bool ret = modem.factoryDefault();

  SerialMon.println(GF("***********************************************************"));
  SerialMon.print  (GF(" Return settings to Factory Defaults: "));
  SerialMon.println((ret) ? "OK" : "FAIL");
  SerialMon.println(GF("***********************************************************"));
}

void loop() {

}
