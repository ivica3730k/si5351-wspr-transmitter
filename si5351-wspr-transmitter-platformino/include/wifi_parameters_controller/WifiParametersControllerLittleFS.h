#ifndef WIFIPARAMETERSCONTROLLERLITTLEFS_H
#define WIFIPARAMETERSCONTROLLERLITTLEFS_H

#include "wifi_parameters_controller/WiFiParametersController.h"

#include <Arduino.h>
#include <LittleFS.h>

#define FILE_WIFI_SSID "wssid"
#define FILE_WIFI_PASSWORD "wpass"

class WiFiParametersControllerLittleFS : public WiFiParametersController
{
  public:
    void begin()
    {
        if (!LittleFS.begin())
        {
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.println("Failed to mount LittleFS");
#endif
            return;
        }
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
        Serial.println("WiFiParametersController initialized with LittleFS");
#endif
    }

    String get_ssid() override
    {
        return this->read_string(FILE_WIFI_SSID, "default_ssid");
    }

    String get_password() override
    {
        return this->read_string(FILE_WIFI_PASSWORD, "default_password");
    }

    void set_ssid(const String &ssid) override
    {
        if (ssid == this->get_ssid())
        {
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.println("SSID is unchanged, not writing to file.");
#endif
            return;
        }
        this->write_string(FILE_WIFI_SSID, ssid);
    }

    void set_password(const String &password) override
    {
        if (password == this->get_password())
        {
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.println("Password is unchanged, not writing to file.");
#endif
            return;
        }
        this->write_string(FILE_WIFI_PASSWORD, password);
    }

  private:
    void write_string(const char *path, const String &val)
    {
        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.print(val);
            file.close();
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Successfully wrote string to ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println(val);
#endif
        }
        else
        {
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Failed to open ");
            Serial.print(path);
            Serial.println(" for writing");
#endif
        }
    }
    String read_string(const char *path, const char *default_val)
    {
        File file = LittleFS.open(path, "r");
        if (file)
        {
            String value = file.readStringUntil('\n');
            file.close();
            value.trim();
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Read string from ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println(value);
#endif
            return value;
        }
#ifdef DEBUG_WIFI_PARAMETERS_CONTROLLER_LITTLE_FS
        Serial.print("Failed to open ");
        Serial.print(path);
        Serial.println(", using default string");
#endif
        return String(default_val);
    }
};
#endif // WIFIPARAMETERSCONTROLLERLITTLEFS_H