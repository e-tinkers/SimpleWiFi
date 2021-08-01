/**
 * @file       SimpleWiFiTemperature.tpp
 * @author     Henry Cheung
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 TinyGSM - Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2021 SimpleWiFi - Henry Cheung
 * @date       July 2021
 */

#ifndef SRC_SIMPLEWIFITEMPERATURE_H_
#define SRC_SIMPLEWIFITEMPERATURE_H_

#include "SimpleWiFiCommon.h"

#define SIMPLE_WIFI_MODEM_HAS_TEMPERATURE

template <class modemType>
class SimpleWiFiTemperature {
 public:
  /*
   * Temperature functions
   */
  float getTemperature() {
    return thisModem().getTemperatureImpl();
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

  float getTemperatureImpl() SIMPLE_WIFI_ATTR_NOT_IMPLEMENTED;
};

#endif  // SRC_SIMPLEWIFITEMPERATURE_H_
