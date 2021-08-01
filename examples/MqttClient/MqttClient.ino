/**************************************************************
 *
 * For this example, you need to install PubSubClient library:
 *   https://github.com/knolleary/pubsubclient
 *   or from http://librarymanager/all#PubSubClient
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * For more MQTT examples, see PubSubClient library
 *
 **************************************************************
 * This example connects to HiveMQ's showcase broker.
 *
 * You can quickly test sending and receiving messages from the HiveMQ webclient
 * available at http://www.hivemq.com/demos/websocket-client/.
 *
 * Subscribe to the topic WiFiClientTest/ledStatus
 * Publish "toggle" to the topic WiFiClientTest/led and the LED on your board
 * should toggle and you should see a new message published to
 * WiFiClientTest/ledStatus with the newest LED status.
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

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define SIMPLE_WIFI_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define SIMPLE_WIFI_YIELD() { delay(2); }

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
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

// MQTT details
const char* broker = "broker.hivemq.com";

const char* topicLed       = "WiFiClientTest/led";
const char* topicInit      = "WiFiClientTest/init";
const char* topicLedStatus = "WiFiClientTest/ledStatus";

#include <SimpleWiFiClient.h>
#include <PubSubClient.h>

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
SimpleWiFiClient client(modem);
PubSubClient  mqtt(client);

#define LED_PIN 13
int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();

  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_PIN, ledStatus);
    mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("WiFiClientTest");

  // Or, if you want to authenticate MQTT:
  // boolean status = mqtt.connect("WiFiClientName", "mqtt_user", "mqtt_pass");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish(topicInit, "WiFiClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}


void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  SerialMon.println("Wait...");

  // Set GSM module baud rate
  SimpleWiFiAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  // SerialAT.begin(9600);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

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
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) { SerialMon.println("Network connected"); }

#if SIMPLE_WIFI_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(GF("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }
#endif

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }

#if SIMPLE_WIFI_USE_GPRS
    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected()) {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(GF("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) { SerialMon.println("GPRS reconnected"); }
    }
#endif
  }

  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) { lastReconnectAttempt = 0; }
    }
    delay(100);
    return;
  }

  mqtt.loop();
}
