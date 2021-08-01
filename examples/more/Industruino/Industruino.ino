/**************************************************************
 *
 * This sketch connects to a website and downloads a page.
 * It can be used to perform HTTP/RESTful API calls.
 *
 * For this example, you need to install ArduinoHttpClient library:
 *   https://github.com/arduino-libraries/ArduinoHttpClient
 *   or from http://librarymanager/all#ArduinoHttpClient
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * For more HTTP API examples, see ArduinoHttpClient library
 *
 **************************************************************/

// Industruino uses SIM800H
#define SIMPLE_WIFI_MODEM_SIM800

// Increase RX buffer if needed
#if !defined(SIMPLE_WIFI_RX_BUFFER)
#define SIMPLE_WIFI_RX_BUFFER 512
#endif

#include <SimpleWiFiClient.h>
#include <ArduinoHttpClient.h>

// Uncomment this if you want to see all AT commands
// #define DUMP_AT_COMMANDS

// Uncomment this if you want to use SSL
// #define USE_SSL

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon SerialUSB

// Select Serial1 or Serial depending on your module configuration
#define SerialAT Serial1

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "YourAPN";
const char user[] = "";
const char pass[] = "";

// Server details
const char server[] = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  SimpleWiFi modem(debugger);
#else
  SimpleWiFi modem(SerialAT);
#endif

#ifdef USE_SSL
  SimpleWiFiClientSecure client(modem);
  HttpClient http(client, server, 443);
#else
  SimpleWiFiClient client(modem);
  HttpClient http(client, server, 80);
#endif

void setup() {
  // Turn on modem with 1 second pulse on D6
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  delay(1000);
  digitalWrite(6, LOW);

  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println(GF("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print(GF("Modem: "));
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
}

void loop() {
  SerialMon.print(GF("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  SerialMon.print(GF("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  SerialMon.print(GF("Performing HTTP GET request... "));
  int err = http.get(resource);
  if (err != 0) {
    SerialMon.println(GF("failed to connect"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    //SerialMon.println(headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(GF("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(GF("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(GF("Response:"));
  SerialMon.println(body);

  SerialMon.print(GF("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
  SerialMon.println(GF("Server disconnected"));

  modem.gprsDisconnect();
  SerialMon.println(GF("GPRS disconnected"));

  // Do nothing forevermore
  while (true) {
    delay(1000);
  }
}
