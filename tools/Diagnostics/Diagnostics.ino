/**************************************************************
 *
 * To run this tool you need StreamDebugger library:
 *   https://github.com/vshymanskyy/StreamDebugger
 *   or from http://librarymanager/all#StreamDebugger
 *
 * TinyGSM Getting Started guide:
 *   https://github.com/e-tinkers/SimpleWiFi
 *
 **************************************************************/

// Select your modem:
#define SIMPLE_WIFI_MODEM_ESP8266
// #define SIMPLE_WIFI_MODEM_XBEE

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

// Add a reception delay - may be needed for a fast processor at a slow baud
// rate #define SIMPLE_WIFI_YIELD() { delay(2); }

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

// Server details
const char server[]   = "httpbin.org";
const char resource[] = "/get";

#include <SimpleWiFiClient.h>


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
SimpleWiFi modem(debugger);
#else
SimpleWiFi modem(SerialAT);
#endif

#ifdef USE_SSL && defined SIMPLE_WIFI_MODEM_HAS_SSL
SimpleWiFiClientSecure client(modem);
const int port = 443;
#else
SimpleWiFiClient client(modem);
const int port = 80;
#endif

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  SerialAT.begin(115200);
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
    SerialMon.println(GF(" Try using File -> Examples -> SimpleWiFi -> tools -> AT_Debug to find correct configuration"));
    SerialMon.println(GF("************************"));
    delay(10000);
    return;
  }
  SerialMon.println(GF(" [OK]"));

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print(GF("Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

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

  modem.networkDisconnect();
  SerialMon.println(GF("WiFi disconnected"));

  SerialMon.println();
  SerialMon.println(GF("************************"));
  SerialMon.print(GF(" Received: "));
  SerialMon.print(bytesReceived);
  SerialMon.println(GF(" bytes"));
  SerialMon.println(GF("************************"));

  // Do nothing forevermore
  while (true) { delay(1000); }
}
