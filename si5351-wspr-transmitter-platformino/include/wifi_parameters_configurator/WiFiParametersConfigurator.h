#ifndef WIFIPARAMETERSCONFIGURATOR_H
#define WIFIPARAMETERSCONFIGURATOR_H

#include "wifi_parameters_controller/WiFiParametersController.h"

#include <Arduino.h>

class WiFiParametersConfigurator
{
  public:
    virtual void begin(WiFiParametersController &wifiParametersController) = 0;

    virtual void set_ssid() = 0;
    virtual void set_password() = 0;
};

#endif // WIFIPARAMETERSCONFIGURATOR_H