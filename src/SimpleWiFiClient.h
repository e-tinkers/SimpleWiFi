/**
 * @file       SimpleWiFiClient.h
 * @author     Henry Cheung (Based on original work from TinyGSM by Volodymyr Shymanskyy)
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 TinyGSM - Volodymyr Shymanskyy
 * @copyright  Copyright (c) 2021 SimpleWiFi - Henry Cheung
 * @date       July 2021
 */

#ifndef SRC_SIMPLEWIFICLIENT_H_
#define SRC_SIMPLEWIFICLIENT_H_

#if defined(SIMPLE_WIFI_MODEM_ESP8266)
#define SIMPLE_WIFI_MODEM_HAS_WIFI
#include "SimpleWiFiClientESP8266.h"
typedef SimpleWiFiESP8266                         SimpleWiFi;
typedef SimpleWiFiESP8266::WiFiClientESP8266       SimpleWiFiClient;
typedef SimpleWiFiESP8266::WiFiClientSecureESP8266 SimpleWiFiClientSecure;

#elif defined(SIMPLE_WIFI_MODEM_XBEE)
#define SIMPLE_WIFI_MODEM_HAS_WIFI
#include "SimpleWiFiClientXBee.h"
typedef SimpleWiFiXBee                      SimpleWiFi;
typedef SimpleWiFiXBee::WiFiClientXBee       SimpleWiFiClient;
typedef SimpleWiFiXBee::WiFiClientSecureXBee SimpleWiFiClientSecure;

#else
#error "Please define GSM modem model"
#endif

#endif  // SRC_SIMPLEWIFICLIENT_H_
