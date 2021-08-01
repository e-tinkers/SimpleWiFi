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

// Set serial for debug console (to the Serial Monitor, default speed 115200)
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

// Increase RX buffer to capture the entire response
// Chips without internal buffering (A6/A7, ESP8266, M590)
// need enough space in the buffer for the entire response
// else data will be lost (and the http library will fail).
#ifndef SIMPLE_WIFI_RX_BUFFER
#define SIMPLE_WIFI_RX_BUFFER 1024
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define SIMPLE_WIFI_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay - may be needed for a fast processor at a slow baud
// rate #define SIMPLE_WIFI_YIELD() { delay(2); }

// Uncomment this if you want to use SSL
// #define USE_SSL

#define SIMPLE_WIFI_USE_GPRS true
#define SIMPLE_WIFI_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]      = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

// Server details
const char server[]   = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";

#include <SimpleWiFiClient.h>

// Just in case someone defined the wrong thing..
#if SIMPLE_WIFI_USE_GPRS && not defined SIMPLE_WIFI_MODEM_HAS_GPRS
#undef SIMPLE_WIFI_USE_GPRS
#undef SIMPLE_WIFI_USE_WIFI
#define SIMPLE_WIFI_USE_GPRS false
#define SIMPLE_WIFI_USE_WIFI true
#endif
#if SIMPLE_WIFI_USE_WIFI && not defined SIMPLE_WIFI_MODEM_HAS_WIFI
#undef SIMPLE_WIFI_USE_GPRS
#undef SIMPLE_WIFI_USE_WIFI
#define SIMPLE_WIFI_USE_GPRS true
#define SIMPLE_WIFI_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
SimpleWiFi        modem(debugger);
#else
SimpleWiFi        modem(SerialAT);
#endif

#ifdef USE_SSL&& defined SIMPLE_WIFI_MODEM_HAS_SSL
SimpleWiFiClientSecure      client(modem);
const int                port = 443;
#else
SimpleWiFiClient  client(modem);
const int      port = 80;
#endif

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  SerialMon.println("Wait...");

  // Set GSM module baud rate
  SimpleWiFiAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  // SerialAT.begin(9600);
  delay(6000);
}

void loop() {
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.print("Initializing modem...");
  if (!modem.restart()) {
    // if (!modem.init()) {
    SerialMon.println(GF(" [fail]"));
    SerialMon.println(GF("************************"));
    SerialMon.println(GF(" Is your modem connected properly?"));
    SerialMon.println(GF(" Is your serial speed (baud rate) correct?"));
    SerialMon.println(GF(" Is your modem powered on?"));
    SerialMon.println(GF(" Do you use a good, stable power source?"));
    SerialMon.println(GF(" Try using File -> Examples -> TinyGSM -> tools -> AT_Debug to find correct configuration"));
    SerialMon.println(GF("************************"));
    delay(10000);
    return;
  }
  SerialMon.println(GF(" [OK]"));

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if SIMPLE_WIFI_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
#endif

#if SIMPLE_WIFI_USE_WIFI
  // Wifi connection parameters must be set before waiting for the network
  SerialMon.print(GF("Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");
#endif

#if SIMPLE_WIFI_USE_GPRS && defined SIMPLE_WIFI_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork(
          600000L)) {  // You may need lengthen this in poor service areas
    SerialMon.println(GF(" [fail]"));
    SerialMon.println(GF("************************"));
    SerialMon.println(GF(" Is your sim card locked?"));
    SerialMon.println(GF(" Do you have a good signal?"));
    SerialMon.println(GF(" Is antenna attached?"));
    SerialMon.println(GF(" Does the SIM card work with your phone?"));
    SerialMon.println(GF("************************"));
    delay(10000);
    return;
  }
  SerialMon.println(GF(" [OK]"));

#if SIMPLE_WIFI_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print("Connecting to ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(GF(" [fail]"));
    SerialMon.println(GF("************************"));
    SerialMon.println(GF(" Is GPRS enabled by network provider?"));
    SerialMon.println(GF(" Try checking your card balance."));
    SerialMon.println(GF("************************"));
    delay(10000);
    return;
  }
  SerialMon.println(GF(" [OK]"));
#endif

  IPAddress local = modem.localIP();
  SerialMon.print("Local IP: ");
  SerialMon.println(local);

  SerialMon.print(GF("Connecting to "));
  SerialMon.print(server);
  if (!client.connect(server, port)) {
    SerialMon.println(GF(" [fail]"));
    delay(10000);
    return;
  }
  SerialMon.println(GF(" [OK]"));

  // Make a HTTP GET request:
  client.print(String("GET ") + resource + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  // Wait for data to arrive
  while (client.connected() && !client.available()) {
    delay(100);
    SerialMon.print('.');
  };
  SerialMon.println();

  // Skip all headers
  client.find("\r\n\r\n");

  // Read data
  uint32_t timeout       = millis();
  uint32_t bytesReceived = 0;
  while (client.connected() && millis() - timeout < 10000L) {
    while (client.available()) {
      char c = client.read();
      // SerialMon.print(c);
      bytesReceived += 1;
      timeout = millis();
    }
  }

  client.stop();
  SerialMon.println(GF("Server disconnected"));

#if SIMPLE_WIFI_USE_WIFI
  modem.networkDisconnect();
  SerialMon.println(GF("WiFi disconnected"));
#endif
#if SIMPLE_WIFI_USE_GPRS
  modem.gprsDisconnect();
  SerialMon.println(GF("GPRS disconnected"));
#endif

  SerialMon.println();
  SerialMon.println(GF("************************"));
  SerialMon.print(GF(" Received: "));
  SerialMon.print(bytesReceived);
  SerialMon.println(GF(" bytes"));
  SerialMon.print(GF(" Test:     "));
  SerialMon.println((bytesReceived == 121) ? "PASSED" : "FAILED");
  SerialMon.println(GF("************************"));

  // Do nothing forevermore
  while (true) { delay(1000); }
}
