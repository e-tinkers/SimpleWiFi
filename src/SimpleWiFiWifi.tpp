/**
 * @file       SimpleWiFiWifi.tpp
 * @author     Henry Cheung
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 TinyGSM - Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2021 SimpleWiFi - Henry Cheung
 * @date       July 2021
 */

#ifndef SRC_SIMPLEWIFIWIFI_H_
#define SRC_SIMPLEWIFIWIFI_H_

#include "SimpleWiFiCommon.h"

#define SIMPLE_WIFI_MODEM_HAS_WIFI

template <class modemType>
class SimpleWiFiWifi {
 public:
  /*
   * WiFi functions
   */
  bool networkConnect(const char* ssid, const char* pwd) {
    return thisModem().networkConnectImpl(ssid, pwd);
  }
  bool networkDisconnect() {
    return thisModem().networkDisconnectImpl();
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
   * WiFi functions
   */

  bool networkConnectImpl(const char* ssid,
                          const char* pwd) SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;
  bool networkDisconnectImpl() SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;
};

#endif  // SRC_SIMPLEWIFIWIFI_H_
