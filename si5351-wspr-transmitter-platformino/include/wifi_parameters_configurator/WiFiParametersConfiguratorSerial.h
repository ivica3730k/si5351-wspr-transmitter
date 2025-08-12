#ifndef WIFIPARAMETERSCONFIGURATORSERIAL_H
#define WIFIPARAMETERSCONFIGURATORSERIAL_H

#include "wifi_parameters_configurator/WiFiParametersConfigurator.h"
#include "wifi_parameters_controller/WiFiParametersController.h"

class WiFiParametersConfiguratorSerial : public WiFiParametersConfigurator
{
  public:
    void begin(WiFiParametersController &wifiParametersController) override
    {
        this->wifi_parameters_controller = &wifiParametersController;
    }

    void set_ssid() override
    {
        String current_ssid =
            this->wifi_parameters_controller->get_ssid(); // seperate line to avoid serial debug
                                                          // messages interupting users focus
        Serial.println("WiFi SSID Configuration");
        Serial.print("Current SSID is set to: ");
        Serial.println(current_ssid);
        Serial.println("Please enter a new SSID (repeat above if unchanged):");

        Serial.print("Enter SSID: ");
        String ssid;

        while (true)
        {
            if (Serial.available() > 0)
            {
                ssid = Serial.readStringUntil('\n');
                ssid.trim();
                if (ssid.length() > 0)
                {
                    this->wifi_parameters_controller->set_ssid(ssid);
                    Serial.print("SSID set to: ");
                    Serial.println(ssid);
                    break;
                }
                else
                {
                    Serial.println("Please try again.");
                    Serial.print("Enter SSID: ");
                }
            }
        }
    }

    void set_password() override
    {
        String current_password =
            this->wifi_parameters_controller->get_password(); // separate line to avoid serial debug
                                                              // messages interrupting users focus
        Serial.println("WiFi Password Configuration");
        Serial.print("Current Password is set to: ");
        Serial.println(current_password);
        Serial.println("Please enter a new Password (repeat above if unchanged):");

        Serial.print("Enter Password: ");
        String password;

        while (true)
        {
            if (Serial.available() > 0)
            {
                password = Serial.readStringUntil('\n');
                password.trim();
                if (password.length() > 0)
                {
                    this->wifi_parameters_controller->set_password(password);
                    Serial.print("Password set to: ");
                    Serial.println(password);
                    break;
                }
                else
                {
                    Serial.println("Please try again.");
                    Serial.print("Enter Password: ");
                }
            }
        }
    }

  private:
    WiFiParametersController *wifi_parameters_controller;
};

#endif // WIFIPARAMETERSCONFIGURATORSERIAL_H