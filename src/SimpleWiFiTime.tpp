/**
 * @file       SimpleWiFiTime.tpp
 * @author     Volodymyr Shymanskyy
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */

#ifndef SRC_SIMPLEWIFITIME_H_
#define SRC_SIMPLEWIFITIME_H_

#include "SimpleWiFiCommon.h"

#define SIMPLE_WIFI_MODEM_HAS_TIME

enum TinyGSMDateTimeFormat { DATE_FULL = 0, DATE_TIME = 1, DATE_DATE = 2 };

template <class modemType>
class SimpleWiFiTime {
 public:
  /*
   * Time functions
   */
  String getGSMDateTime(TinyGSMDateTimeFormat format) {
    return thisModem().getGSMDateTimeImpl(format);
  }
  bool getNetworkTime(int* year, int* month, int* day, int* hour, int* minute,
                      int* second, float* timezone) {
    return thisModem().getNetworkTimeImpl(year, month, day, hour, minute,
                                          second, timezone);
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
   * Time functions
   */
 protected:
  String getGSMDateTimeImpl(TinyGSMDateTimeFormat format) {
    thisModem().sendAT(F("+CCLK?"));
    if (thisModem().waitResponse(2000L, F("+CCLK: \"")) != 1) { return ""; }

    String res;

    switch (format) {
      case DATE_FULL: res = thisModem().stream.readStringUntil('"'); break;
      case DATE_TIME:
        thisModem().streamSkipUntil(',');
        res = thisModem().stream.readStringUntil('"');
        break;
      case DATE_DATE: res = thisModem().stream.readStringUntil(','); break;
    }
    thisModem().waitResponse();  // Ends with OK
    return res;
  }

  bool getNetworkTimeImpl(int* year, int* month, int* day, int* hour,
                          int* minute, int* second, float* timezone) {
    thisModem().sendAT(F("+CCLK?"));
    if (thisModem().waitResponse(2000L, F("+CCLK: \"")) != 1) { return false; }

    int iyear     = 0;
    int imonth    = 0;
    int iday      = 0;
    int ihour     = 0;
    int imin      = 0;
    int isec      = 0;
    int itimezone = 0;

    // Date & Time
    iyear       = thisModem().streamGetIntBefore('/');
    imonth      = thisModem().streamGetIntBefore('/');
    iday        = thisModem().streamGetIntBefore(',');
    ihour       = thisModem().streamGetIntBefore(':');
    imin        = thisModem().streamGetIntBefore(':');
    isec        = thisModem().streamGetIntLength(2);
    char tzSign = thisModem().stream.read();
    itimezone   = thisModem().streamGetIntBefore('\n');
    if (tzSign == '-') { itimezone = itimezone * -1; }

    // Set pointers
    if (iyear < 2000) iyear += 2000;
    if (year != NULL) *year = iyear;
    if (month != NULL) *month = imonth;
    if (day != NULL) *day = iday;
    if (hour != NULL) *hour = ihour;
    if (minute != NULL) *minute = imin;
    if (second != NULL) *second = isec;
    if (timezone != NULL) *timezone = static_cast<float>(itimezone) / 4.0;

    // Final OK
    thisModem().waitResponse();
    return true;
  }
};

#endif  // SRC_SIMPLEWIFITIME_H_
