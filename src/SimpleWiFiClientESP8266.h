/**
 * @file       SimpleWiFiClientESP8266.h
 * @author     Henry Cheung
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 TinyGSM - Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2021 SimpleWiFi - Henry Cheung
 * @date       July 2021
 */

#ifndef SRC_SIMPLEWIFICLIENTESP8266_H_
#define SRC_SIMPLEWIFICLIENTESP8266_H_

#define SIMPLE_WIFI_MUX_COUNT 5
#define SIMPLE_WIFI_NO_MODEM_BUFFER

#include "SimpleWiFiModem.tpp"
#include "SimpleWiFiSSL.tpp"
#include "SimpleWiFiTCP.tpp"
#include "SimpleWiFiWifi.tpp"

#define AT_NL "\r\n"

static const char AT_OK[] SIMPLE_WIFI_PROGMEM    = "OK\r\n";
static const char AT_ERROR[] SIMPLE_WIFI_PROGMEM = "ERROR\r\n";
static uint8_t    SIMPLE_WIFI_TCP_KEEP_ALIVE      = 120;

// <stat> status of ESP8266 station interface
// 2 : ESP8266 station connected to an AP and has obtained IP
// 3 : ESP8266 station created a TCP or UDP transmission
// 4 : the TCP or UDP transmission of ESP8266 station disconnected
// 5 : ESP8266 station did NOT connect to an AP
enum RegStatus {
  REG_OK_IP     = 2,
  REG_OK_TCP    = 3,
  REG_OK_NO_TCP = 4,
  REG_DENIED    = 5,
  REG_UNKNOWN   = 6,
};

class SimpleWiFiESP8266 : public SimpleWiFiModem<SimpleWiFiESP8266>,
                       public SimpleWiFiWifi<SimpleWiFiESP8266>,
                       public SimpleWiFiTCP<SimpleWiFiESP8266, SIMPLE_WIFI_MUX_COUNT>,
                       public SimpleWiFiSSL<SimpleWiFiESP8266> {
  friend class SimpleWiFiModem<SimpleWiFiESP8266>;
  friend class SimpleWiFiWifi<SimpleWiFiESP8266>;
  friend class SimpleWiFiTCP<SimpleWiFiESP8266, SIMPLE_WIFI_MUX_COUNT>;
  friend class SimpleWiFiSSL<SimpleWiFiESP8266>;

  /*
   * Inner Client
   */
 public:
  class WiFiClientESP8266 : public WiFiClient {
    friend class SimpleWiFiESP8266;

   public:
    WiFiClientESP8266() {}

    explicit WiFiClientESP8266(SimpleWiFiESP8266& modem, uint8_t mux = 0) {
      init(&modem, mux);
    }

    bool init(SimpleWiFiESP8266* modem, uint8_t mux = 0) {
      this->at       = modem;
      sock_connected = false;

      if (mux < SIMPLE_WIFI_MUX_COUNT) {
        this->mux = mux;
      } else {
        this->mux = (mux % SIMPLE_WIFI_MUX_COUNT);
      }
      at->sockets[this->mux] = this;

      return true;
    }

   public:
    virtual int connect(const char* host, uint16_t port, int timeout_s) {
      stop();
      SIMPLE_WIFI_YIELD();
      rx.clear();
      sock_connected = at->modemConnect(host, port, mux, false, timeout_s);
      return sock_connected;
    }
    SIMPLE_WIFI_CLIENT_CONNECT_OVERRIDES

    void stop(uint32_t maxWaitMs) {
      SIMPLE_WIFI_YIELD();
      at->sendAT(GF("+CIPCLOSE="), mux);
      sock_connected = false;
      at->waitResponse(maxWaitMs);
      rx.clear();
    }
    void stop() override {
      stop(5000L);
    }

    /*
     * Extended API
     */

    String remoteIP() SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;
  };

  /*
   * Inner Secure Client
   */
 public:
  class WiFiClientSecureESP8266 : public WiFiClientESP8266 {
   public:
    WiFiClientSecureESP8266() {}

    explicit WiFiClientSecureESP8266(SimpleWiFiESP8266& modem, uint8_t mux = 0)
        : WiFiClientESP8266(modem, mux) {}

   public:
    int connect(const char* host, uint16_t port, int timeout_s) override {
      stop();
      SIMPLE_WIFI_YIELD();
      rx.clear();
      sock_connected = at->modemConnect(host, port, mux, true, timeout_s);
      return sock_connected;
    }
    SIMPLE_WIFI_CLIENT_CONNECT_OVERRIDES
  };

  /*
   * Constructor
   */
 public:
  explicit SimpleWiFiESP8266(Stream& stream) : stream(stream) {
    memset(sockets, 0, sizeof(sockets));
  }

  /*
   * Basic functions
   */
 protected:
  bool initImpl(const char* pin = NULL) {
    DBG(GF("### SimpleWiFi Version:"), SIMPLEWIFI_VERSION);
    DBG(GF("### SimpleWiFi Module:  SimpleWiFiClientESP8266"));

    if (!testAT()) { return false; }
    sendAT(GF("E0"));  // Echo Off
    if (waitResponse() != 1) { return false; }
    sendAT(GF("+CIPMUX=1"));  // Enable Multiple Connections
    if (waitResponse() != 1) { return false; }
    sendAT(GF("+CWMODE_CUR=1"));  // Put into "station" mode
    if (waitResponse() != 1) { return false; }
    DBG(GF("### Modem:"), getModemName());
    return true;
  }

  String getModemNameImpl() {
    return "ESP8266";
  }

  void setBaudImpl(uint32_t baud) {
    sendAT(GF("+UART_CUR="), baud, "8,1,0,0");
  }

  bool factoryDefaultImpl() {
    sendAT(GF("+RESTORE"));
    return waitResponse() == 1;
  }

  String getModemInfoImpl() {
    sendAT(GF("+GMR"));
    String res;
    if (waitResponse(1000L, res) != 1) { return ""; }
    res.replace("\r\nOK\r\n", "");
    res.replace(AT_NL, " ");
    res.trim();
    return res;
  }

  /*
   * Power functions
   */
 protected:
  bool restartImpl(const char* pin = NULL) {
    if (!testAT()) { return false; }
    sendAT(GF("+RST"));
    if (waitResponse(10000L) != 1) { return false; }
    if (waitResponse(10000L, GF("\r\nready\r\n")) != 1) { return false; }
    delay(500);
    return init(pin);
  }

  bool powerOffImpl() {
    sendAT(GF("+GSLP=0"));  // Power down indefinitely - until manually reset!
    return waitResponse() == 1;
  }

  bool radioOffImpl() SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;

  bool sleepEnableImpl(bool enable = true) SIMPLE_WIFI_ATTR_NOT_AVAILABLE;

  bool setPhoneFunctionalityImpl(uint8_t fun, bool reset = false)
      SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;

  /*
   * Generic network functions
   */
 public:
  RegStatus getRegistrationStatus() {
    sendAT(GF("+CIPSTATUS"));
    if (waitResponse(3000, GF("STATUS:")) != 1) return REG_UNKNOWN;
    int8_t status = waitResponse(GFP(AT_ERROR), GF("2"), GF("3"), GF("4"),
                                 GF("5"));
    waitResponse();  // Returns an OK after the status
    return (RegStatus)status;
  }

 protected:
  int8_t getSignalQualityImpl() {
    sendAT(GF("+CWJAP_CUR?"));
    int8_t res1 = waitResponse(GF("No AP"), GF("+CWJAP_CUR:"));
    if (res1 != 2) {
      waitResponse();
      return 0;
    }
    streamSkipUntil(',');             // Skip SSID
    streamSkipUntil(',');             // Skip BSSID/MAC address
    streamSkipUntil(',');             // Skip Chanel number
    int8_t res2 = stream.parseInt();  // Read RSSI
    waitResponse();                   // Returns an OK after the value
    return res2;
  }

  bool isNetworkConnectedImpl() {
    RegStatus s = getRegistrationStatus();
    if (s == REG_OK_IP || s == REG_OK_TCP) {
      // with these, we're definitely connected
      return true;
    } else if (s == REG_OK_NO_TCP) {
      // with this, we may or may not be connected
      if (getLocalIP() == "") {
        return false;
      } else {
        return true;
      }
    } else {
      return false;
    }
  }

  String getLocalIPImpl() {
    sendAT(GF("+CIPSTA_CUR?"));
    int8_t res1 = waitResponse(GF("ERROR"), GF("+CWJAP_CUR:"));
    if (res1 != 2) { return ""; }
    String res2 = stream.readStringUntil('"');
    waitResponse();
    return res2;
  }

  /*
   * WiFi functions
   */
 protected:
  bool networkConnectImpl(const char* ssid, const char* pwd) {
    sendAT(GF("+CWJAP_CUR=\""), ssid, GF("\",\""), pwd, GF("\""));
    if (waitResponse(30000L, GFP(AT_OK), GF("\r\nFAIL\r\n")) != 1) {
      return false;
    }

    return true;
  }

  bool networkDisconnectImpl() {
    sendAT(GF("+CWQAP"));
    bool retVal = waitResponse(10000L) == 1;
    waitResponse(GF("WIFI DISCONNECT"));
    return retVal;
  }

  /*
   * Client related functions
   */
 protected:
  bool modemConnect(const char* host, uint16_t port, uint8_t mux,
                    bool ssl = false, int timeout_s = 75) {
    uint32_t timeout_ms = ((uint32_t)timeout_s) * 1000;
    if (ssl) {
      sendAT(GF("+CIPSSLSIZE=4096"));
      waitResponse();
    }
    sendAT(GF("+CIPSTART="), mux, ',', ssl ? GF("\"SSL") : GF("\"TCP"),
           GF("\",\""), host, GF("\","), port, GF(","),
           SIMPLE_WIFI_TCP_KEEP_ALIVE);
    // TODO(?): Check mux
    int8_t rsp = waitResponse(timeout_ms, GFP(AT_OK), GFP(AT_ERROR),
                              GF("ALREADY CONNECT"));
    // if (rsp == 3) waitResponse();
    // May return "ERROR" after the "ALREADY CONNECT"
    return (1 == rsp);
  }

  int16_t modemSend(const void* buff, size_t len, uint8_t mux) {
    sendAT(GF("+CIPSEND="), mux, ',', (uint16_t)len);
    if (waitResponse(GF(">")) != 1) { return 0; }
    stream.write(reinterpret_cast<const uint8_t*>(buff), len);
    stream.flush();
    if (waitResponse(10000L, GF("\r\nSEND OK\r\n")) != 1) { return 0; }
    return len;
  }

  bool modemGetConnected(uint8_t mux) {
    sendAT(GF("+CIPSTATUS"));
    if (waitResponse(3000, GF("STATUS:")) != 1) { return false; }
    int8_t status = waitResponse(GFP(AT_ERROR), GF("2"), GF("3"), GF("4"),
                                 GF("5"));
    if (status != 3) {
      // if the status is anything but 3, there are no connections open
      waitResponse();  // Returns an OK after the status
      for (int muxNo = 0; muxNo < SIMPLE_WIFI_MUX_COUNT; muxNo++) {
        if (sockets[muxNo]) { sockets[muxNo]->sock_connected = false; }
      }
      return false;
    }
    bool verified_connections[SIMPLE_WIFI_MUX_COUNT] = {0, 0, 0, 0, 0};
    for (int muxNo = 0; muxNo < SIMPLE_WIFI_MUX_COUNT; muxNo++) {
      uint8_t has_status = waitResponse(GF("+CIPSTATUS:"), GFP(AT_OK),
                                        GFP(AT_ERROR));
      if (has_status == 1) {
        int8_t returned_mux = streamGetIntBefore(',');
        streamSkipUntil(',');   // Skip mux
        streamSkipUntil(',');   // Skip type
        streamSkipUntil(',');   // Skip remote IP
        streamSkipUntil(',');   // Skip remote port
        streamSkipUntil(',');   // Skip local port
        streamSkipUntil('\n');  // Skip client/server type
        verified_connections[returned_mux] = 1;
      }
      if (has_status == 2) break;  // once we get to the ok, stop
    }
    for (int muxNo = 0; muxNo < SIMPLE_WIFI_MUX_COUNT; muxNo++) {
      if (sockets[muxNo]) {
        sockets[muxNo]->sock_connected = verified_connections[muxNo];
      }
    }
    return verified_connections[mux];
  }

  /*
   * Utilities
   */
 public:
  // TODO(vshymanskyy): Optimize this!
  int8_t waitResponse(uint32_t timeout_ms, String& data,
                      FlashConstStr r1 = GFP(AT_OK),
                      FlashConstStr r2 = GFP(AT_ERROR), FlashConstStr r3 = NULL,
                      FlashConstStr r4 = NULL, FlashConstStr r5 = NULL) {
    /*String r1s(r1); r1s.trim();
    String r2s(r2); r2s.trim();
    String r3s(r3); r3s.trim();
    String r4s(r4); r4s.trim();
    String r5s(r5); r5s.trim();
    DBG("### ..:", r1s, ",", r2s, ",", r3s, ",", r4s, ",", r5s);*/
    data.reserve(64);
    uint8_t  index       = 0;
    uint32_t startMillis = millis();
    do {
      SIMPLE_WIFI_YIELD();
      while (stream.available() > 0) {
        SIMPLE_WIFI_YIELD();
        int8_t a = stream.read();
        if (a <= 0) continue;  // Skip 0x00 bytes, just in case
        data += static_cast<char>(a);
        if (r1 && data.endsWith(r1)) {
          index = 1;
          goto finish;
        } else if (r2 && data.endsWith(r2)) {
          index = 2;
          goto finish;
        } else if (r3 && data.endsWith(r3)) {
          index = 3;
          goto finish;
        } else if (r4 && data.endsWith(r4)) {
          index = 4;
          goto finish;
        } else if (r5 && data.endsWith(r5)) {
          index = 5;
          goto finish;
        } else if (data.endsWith(GF("+IPD,"))) {
          int8_t  mux      = streamGetIntBefore(',');
          int16_t len      = streamGetIntBefore(':');
          int16_t len_orig = len;
          if (mux >= 0 && mux < SIMPLE_WIFI_MUX_COUNT && sockets[mux]) {
            if (len > sockets[mux]->rx.free()) {
              DBG("### Buffer overflow: ", len, "received vs",
                  sockets[mux]->rx.free(), "available");
            } else {
              // DBG("### Got Data: ", len, "on", mux);
            }
            while (len--) { moveCharFromStreamToFifo(mux); }
            // TODO(SRGDamia1): deal with buffer overflow/missed characters
            if (len_orig > sockets[mux]->available()) {
              DBG("### Fewer characters received than expected: ",
                  sockets[mux]->available(), " vs ", len_orig);
            }
          }
          data = "";
        } else if (data.endsWith(GF("CLOSED"))) {
          int8_t muxStart =
              SimpleWiFiMax(0, data.lastIndexOf(AT_NL, data.length() - 8));
          int8_t coma = data.indexOf(',', muxStart);
          int8_t mux  = data.substring(muxStart, coma).toInt();
          if (mux >= 0 && mux < SIMPLE_WIFI_MUX_COUNT && sockets[mux]) {
            sockets[mux]->sock_connected = false;
          }
          data = "";
          DBG("### Closed: ", mux);
        }
      }
    } while (millis() - startMillis < timeout_ms);
  finish:
    if (!index) {
      data.trim();
      if (data.length()) { DBG("### Unhandled:", data); }
      data = "";
    }
    // data.replace(AT_NL, "/");
    // DBG('<', index, '>', data);
    return index;
  }

  int8_t waitResponse(uint32_t timeout_ms, FlashConstStr r1 = GFP(AT_OK),
                      FlashConstStr r2 = GFP(AT_ERROR), FlashConstStr r3 = NULL,
                      FlashConstStr r4 = NULL, FlashConstStr r5 = NULL) {
    String data;
    return waitResponse(timeout_ms, data, r1, r2, r3, r4, r5);
  }

  int8_t waitResponse(FlashConstStr r1 = GFP(AT_OK),
                      FlashConstStr r2 = GFP(AT_ERROR), FlashConstStr r3 = NULL,
                      FlashConstStr r4 = NULL, FlashConstStr r5 = NULL) {
    return waitResponse(1000, r1, r2, r3, r4, r5);
  }

 public:
  Stream& stream;

 protected:
  WiFiClientESP8266* sockets[SIMPLE_WIFI_MUX_COUNT];
  const char* wifiLinefeed = AT_NL;
};

#endif  // SRC_SIMPLEWIFICLIENTESP8266_H_
