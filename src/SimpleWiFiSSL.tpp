/**
 * @file       SimpleWiFiSSL.tpp
 * @author     Henry Cheung
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 TinyGSM - Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2021 SimpleWiFi - Henry Cheung
 * @date       July 2021
 */

#ifndef SRC_SIMPLEWIFISSL_H_
#define SRC_SIMPLEWIFISSL_H_

#include "SimpleWiFiCommon.h"

#define SIMPLE_WIFI_MODEM_HAS_SSL


template <class modemType>
class SimpleWiFiSSL {
 public:
  /*
   * SSL functions
   */
  bool addCertificate(const char* filename) {
    return thisModem().addCertificateImpl(filename);
  }
  bool deleteCertificate() {
    return thisModem().deleteCertificateImpl();
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
   * Inner Secure Client
   */
  /*
 public:
  class WiFiClientSecure : public WiFiClient {
   public:
    WiFiClientSecureSim800() {}

    explicit WiFiClientSecureSim800(SimpleWiFiSim800& modem, uint8_t mux = 0)
        : WiFiClientSim800(modem, mux) {}

   public:
    int connect(const char* host, uint16_t port, int timeout_s) overide {
      stop();
      SIMPLE_WIFI_YIELD();
      rx.clear();
      sock_connected = at->modemConnect(host, port, mux, true, timeout_s);
      return sock_connected;
    }
  };*/

  /*
   * SSL functions
   */
 protected:
  bool addCertificateImpl(const char* filename) SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;
  bool deleteCertificateImpl() SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;
};

#endif  // SRC_SIMPLEWIFISSL_H_
