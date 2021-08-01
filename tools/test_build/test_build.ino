/**************************************************************
 *
 *  DO NOT USE THIS - this is just a compilation test!
 *  This is NOT an example for use of this library!
 *
 **************************************************************/
#include <SimpleWiFiClient.h>

SimpleWiFi modem(Serial);

void setup() {
  Serial.begin(115200);
  delay(6000);
}

void loop() {
  // Test the basic functions
  modem.begin();
  modem.begin("1234");
  modem.init();
  modem.init("1234");
  modem.setBaud(115200);
  modem.testAT();

  modem.getModemInfo();
  modem.getModemName();
  modem.factoryDefault();

  // Test Power functions
  modem.restart();
  // modem.sleepEnable();  // Not available for all modems
  // modem.radioOff();  // Not available for all modems
  modem.poweroff();

  // Test generic network functions
  modem.getRegistrationStatus();
  modem.isNetworkConnected();
  modem.waitForNetwork();
  modem.waitForNetwork(15000L);
  modem.waitForNetwork(15000L, true);
  modem.getSignalQuality();
  modem.getLocalIP();
  modem.localIP();

// Test WiFi Functions
#if defined(SIMPLE_WIFI_MODEM_HAS_WIFI)
  modem.networkConnect("mySSID", "mySSIDPassword");
  modem.networkDisconnect();
#endif

  // Test TCP functions
  modem.maintain();
  SimpleWiFiClient client;
  SimpleWiFiClient client2(modem);
  SimpleWiFiClient client3(modem, 1);
  client.init(&modem);
  client.init(&modem, 1);

  char server[]   = "httpbin.org";
  char resource[] = "/get";

  client.connect(server, 80);

  // Make a HTTP GET request:
  client.print(String("GET ") + resource + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  uint32_t timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    while (client.available()) {
      client.read();
      timeout = millis();
    }
  }

  client.stop();

#if defined(SIMPLE_WIFI_MODEM_HAS_SSL)
  // modem.addCertificate();  // not yet impemented
  // modem.deleteCertificate();  // not yet impemented
  SimpleWiFiClientSecure client_secure(modem);
  SimpleWiFiClientSecure client_secure2(modem);
  SimpleWiFiClientSecure client_secure3(modem, 1);
  client_secure.init(&modem);
  client_secure.init(&modem, 1);

  client_secure.connect(server, 443);

  // Make a HTTP GET request:
  client_secure.print(String("GET ") + resource + " HTTP/1.0\r\n");
  client_secure.print(String("Host: ") + server + "\r\n");
  client_secure.print("Connection: close\r\n\r\n");

  timeout = millis();
  while (client_secure.connected() && millis() - timeout < 10000L) {
    while (client_secure.available()) {
      client_secure.read();
      timeout = millis();
    }
  }

  client_secure.stop();
#endif

// Test the Network time function
#if defined(SIMPLE_WIFI_MODEM_HAS_NTP) && not defined(__AVR_ATmega32U4__)
  modem.NTPServerSync("pool.ntp.org", 3);
#endif

// Test the Network time function
#if defined(SIMPLE_WIFI_MODEM_HAS_TIME) && not defined(__AVR_ATmega32U4__)
  modem.getGSMDateTime(DATE_FULL);
  int   year3    = 0;
  int   month3   = 0;
  int   day3     = 0;
  int   hour3    = 0;
  int   min3     = 0;
  int   sec3     = 0;
  float timezone = 0;
  modem.getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3, &timezone);
#endif

// Test Battery functions
#if defined(SIMPLE_WIFI_MODEM_HAS_BATTERY)
  uint8_t  chargeState   = 0;
  int8_t   chargePercent = 0;
  uint16_t milliVolts    = 0;
  modem.getBattStats(chargeState, chargePercent, milliVolts);
#endif

// Test the temperature function
#if defined(SIMPLE_WIFI_MODEM_HAS_TEMPERATURE)
  modem.getTemperature();
#endif
}
