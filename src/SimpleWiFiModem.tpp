/**
 * @file       SimpleWiFiModem.tpp
 * @author     Henry Cheung
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 TinyGSM - Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2021 SimpleWiFi - Henry Cheung
 * @date       July 2021
 */

#ifndef SRC_SIMPLEWIFIMODEM_H_
#define SRC_SIMPLEWIFIMODEM_H_

#include "SimpleWiFiCommon.h"

template <class modemType>
class SimpleWiFiModem {
 public:
  /*
   * Basic functions
   */
  bool begin(const char* pin = NULL) {
    return thisModem().initImpl(pin);
  }
  bool init(const char* pin = NULL) {
    return thisModem().initImpl(pin);
  }
  template <typename... Args>
  inline void sendAT(Args... cmd) {
    thisModem().streamWrite("AT", cmd..., thisModem().crlf);
    thisModem().stream.flush();
    SIMPLE_WIFI_YIELD(); /* DBG("### AT:", cmd...); */
  }
  void setBaud(uint32_t baud) {
    thisModem().setBaudImpl(baud);
  }
  // Test response to AT commands
  bool testAT(uint32_t timeout_ms = 10000L) {
    return thisModem().testATImpl(timeout_ms);
  }

  // Asks for modem information via the V.25TER standard ATI command
  // NOTE:  The actual value and style of the response is quite varied
  String getModemInfo() {
    return thisModem().getModemInfoImpl();
  }
  // Gets the modem name (as it calls itself)
  String getModemName() {
    return thisModem().getModemNameImpl();
  }
  bool factoryDefault() {
    return thisModem().factoryDefaultImpl();
  }

  /*
   * Power functions
   */
  bool restart(const char* pin = NULL) {
    return thisModem().restartImpl(pin);
  }
  bool poweroff() {
    return thisModem().powerOffImpl();
  }
  bool radioOff() {
    return thisModem().radioOffImpl();
  }
  bool sleepEnable(bool enable = true) {
    return thisModem().sleepEnableImpl(enable);
  }
  bool setPhoneFunctionality(uint8_t fun, bool reset = false) {
    return thisModem().setPhoneFunctionalityImpl(fun, reset);
  }

  /*
   * Generic network functions
   */
  // RegStatus getRegistrationStatus() {}
  bool isNetworkConnected() {
    return thisModem().isNetworkConnectedImpl();
  }
  // Waits for network attachment
  bool waitForNetwork(uint32_t timeout_ms = 60000L, bool check_signal = false) {
    return thisModem().waitForNetworkImpl(timeout_ms, check_signal);
  }
  // Gets signal quality report
  int16_t getSignalQuality() {
    return thisModem().getSignalQualityImpl();
  }
  String getLocalIP() {
    return thisModem().getLocalIPImpl();
  }
  IPAddress localIP() {
    return thisModem().SimpleWiFiIpFromString(thisModem().getLocalIP());
  }

  /*
   * CRTP Helper
   */
 protected:
  inline const modemType& thisModem() const {
    return static_cast<const modemType&>(*this);
  }
  inline modemType& thisModem() {
    return static_cast<modemType&>(*this);
  }

  /*
   * Basic functions
   */
 protected:
  void setBaudImpl(uint32_t baud) {
    thisModem().sendAT(F("+IPR="), baud);
    thisModem().waitResponse();
  }

  bool testATImpl(uint32_t timeout_ms = 10000L) {
    for (uint32_t start = millis(); millis() - start < timeout_ms;) {
      thisModem().sendAT(F(""));
      if (thisModem().waitResponse(200) == 1) { return true; }
      delay(100);
    }
    return false;
  }

  String getModemInfoImpl() {
    thisModem().sendAT(F("I"));
    String res;
    if (thisModem().waitResponse(1000L, res) != 1) { return ""; }
    // Do the replaces twice so we cover both \r and \r\n type endings
    res.replace("\r\nOK\r\n", "");
    res.replace("\rOK\r", "");
    res.replace("\r\n", " ");
    res.replace("\r", " ");
    res.trim();
    return res;
  }

  String getModemNameImpl() {
    thisModem().sendAT(F("+CGMI"));
    String res1;
    if (thisModem().waitResponse(1000L, res1) != 1) { return "unknown"; }
    res1.replace("\r\nOK\r\n", "");
    res1.replace("\rOK\r", "");
    res1.trim();

    thisModem().sendAT(F("+GMM"));
    String res2;
    if (thisModem().waitResponse(1000L, res2) != 1) { return "unknown"; }
    res2.replace("\r\nOK\r\n", "");
    res2.replace("\rOK\r", "");
    res2.trim();

    String name = res1 + String(' ') + res2;
    DBG("### Modem:", name);
    return name;
  }

  bool factoryDefaultImpl() {
    thisModem().sendAT(F("&FZE0&W"));  // Factory + Reset + Echo Off + Write
    thisModem().waitResponse();
    thisModem().sendAT(F("+IPR=0"));  // Auto-baud
    thisModem().waitResponse();
    thisModem().sendAT(F("&W"));  // Write configuration
    return thisModem().waitResponse() == 1;
  }

  /*
   * Power functions
   */
 protected:
  bool radioOffImpl() {
    if (!thisModem().setPhoneFunctionality(0)) { return false; }
    delay(3000);
    return true;
  }

  bool sleepEnableImpl(bool enable = true) SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;

  bool setPhoneFunctionalityImpl(uint8_t fun, bool reset = false)
      SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;

  /*
   * Generic network functions
   */
 protected:
  // Gets the modem's registration status via CREG/CGREG/CEREG
  // CREG = Generic network registration
  // CGREG = GPRS service registration
  // CEREG = EPS registration for LTE modules
  int8_t getRegistrationStatusXREG(const char* regCommand) {
    thisModem().sendAT('+', regCommand, '?');
    // check for any of the three for simplicity
    int8_t resp = thisModem().waitResponse(F("+CREG:"), F("+CGREG:"),
                                           F("+CEREG:"));
    if (resp != 1 && resp != 2 && resp != 3) { return -1; }
    thisModem().streamSkipUntil(','); /* Skip format (0) */
    int status = thisModem().stream.parseInt();
    thisModem().waitResponse();
    return status;
  }

  bool waitForNetworkImpl(uint32_t timeout_ms   = 60000L,
                          bool     check_signal = false) {
    for (uint32_t start = millis(); millis() - start < timeout_ms;) {
      if (check_signal) { thisModem().getSignalQuality(); }
      if (thisModem().isNetworkConnected()) { return true; }
      delay(250);
    }
    return false;
  }

  // Gets signal quality report according to 3GPP TS command AT+CSQ
  int8_t getSignalQualityImpl() {
    thisModem().sendAT(F("+CSQ"));
    if (thisModem().waitResponse(F("+CSQ:")) != 1) { return 99; }
    int8_t res = thisModem().streamGetIntBefore(',');
    thisModem().waitResponse();
    return res;
  }

  String getLocalIPImpl() {
    thisModem().sendAT(F("+CGPADDR=1"));
    if (thisModem().waitResponse(F("+CGPADDR:")) != 1) { return ""; }
    thisModem().streamSkipUntil(',');  // Skip context id
    String res = thisModem().stream.readStringUntil('\r');
    if (thisModem().waitResponse() != 1) { return ""; }
    return res;
  }

  static inline IPAddress SimpleWiFiIpFromString(const String& strIP) {
    int Parts[4] = {
        0,
    };
    int Part = 0;
    for (uint8_t i = 0; i < strIP.length(); i++) {
      char c = strIP[i];
      if (c == '.') {
        Part++;
        if (Part > 3) { return IPAddress(0, 0, 0, 0); }
        continue;
      } else if (c >= '0' && c <= '9') {
        Parts[Part] *= 10;
        Parts[Part] += c - '0';
      } else {
        if (Part == 3) break;
      }
    }
    return IPAddress(Parts[0], Parts[1], Parts[2], Parts[3]);
  }

  /*
   Utilities
   */
 public:
  // Utility templates for writing/skipping characters on a stream
  template <typename T>
  inline void streamWrite(T last) {
    thisModem().stream.print(last);
  }

  template <typename T, typename... Args>
  inline void streamWrite(T head, Args... tail) {
    thisModem().stream.print(head);
    thisModem().streamWrite(tail...);
  }

  inline void streamClear() {
    while (thisModem().stream.available()) {
      thisModem().waitResponse(50, NULL, NULL);
    }
  }

 protected:
  inline bool streamGetLength(char* buf, int8_t numChars,
                              const uint32_t timeout_ms = 1000L) {
    if (!buf) { return false; }

    int8_t   numCharsReady = -1;
    uint32_t startMillis   = millis();
    while (millis() - startMillis < timeout_ms &&
           (numCharsReady = thisModem().stream.available()) < numChars) {
      SIMPLE_WIFI_YIELD();
    }

    if (numCharsReady >= numChars) {
      thisModem().stream.readBytes(buf, numChars);
      return true;
    }

    return false;
  }

  inline int16_t streamGetIntLength(int8_t         numChars,
                                    const uint32_t timeout_ms = 1000L) {
    char buf[numChars + 1];
    if (streamGetLength(buf, numChars, timeout_ms)) {
      buf[numChars] = '\0';
      return atoi(buf);
    }

    return -9999;
  }

  inline int16_t streamGetIntBefore(char lastChar) {
    char   buf[7];
    size_t bytesRead = thisModem().stream.readBytesUntil(
        lastChar, buf, static_cast<size_t>(7));
    // if we read 7 or more bytes, it's an overflow
    if (bytesRead && bytesRead < 7) {
      buf[bytesRead] = '\0';
      int16_t res    = atoi(buf);
      return res;
    }

    return -9999;
  }

  inline float streamGetFloatLength(int8_t         numChars,
                                    const uint32_t timeout_ms = 1000L) {
    char buf[numChars + 1];
    if (streamGetLength(buf, numChars, timeout_ms)) {
      buf[numChars] = '\0';
      return atof(buf);
    }

    return -9999.0F;
  }

  inline float streamGetFloatBefore(char lastChar) {
    char   buf[16];
    size_t bytesRead = thisModem().stream.readBytesUntil(
        lastChar, buf, static_cast<size_t>(16));
    // if we read 16 or more bytes, it's an overflow
    if (bytesRead && bytesRead < 16) {
      buf[bytesRead] = '\0';
      float res      = atof(buf);
      return res;
    }

    return -9999.0F;
  }

  inline bool streamSkipUntil(const char c, const uint32_t timeout_ms = 1000L) {
    uint32_t startMillis = millis();
    while (millis() - startMillis < timeout_ms) {
      while (millis() - startMillis < timeout_ms &&
             !thisModem().stream.available()) {
        SIMPLE_WIFI_YIELD();
      }
      if (thisModem().stream.read() == c) { return true; }
    }
    return false;
  }
};

#endif  // SRC_SIMPLEWIFIMODEM_H_
