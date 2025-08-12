#ifndef WIFIPARAMETERSCONTROLLER_H
#define WIFIPARAMETERSCONTROLLER_H

#include <Arduino.h>

class WiFiParametersController
{
  public:
    virtual String get_ssid() = 0;
    virtual String get_password() = 0;
    virtual void set_ssid(const String &ssid) = 0;
    virtual void set_password(const String &password) = 0;
};

#endif // WIFIPARAMETERSCONTROLLER_H