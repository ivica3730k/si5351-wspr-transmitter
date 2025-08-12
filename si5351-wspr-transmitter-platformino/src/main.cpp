#include <Arduino.h>
#include <Wire.h>

#if defined(TIME_USE_ESP_WIFI)
#if defined(BOARD_FAMILY_ESP8266) || defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(BOARD_FAMILY_ESP32) || defined(ESP32)
#include <WiFi.h>
#else
#error "TIME_USE_ESP_WIFI is defined but no ESP8266/ESP32 family was specified."
#endif
#endif

#include "OnboardFilterHardware.h"
#include "TxController.h"
#include "time_sync/WSPRTxSyncDummy.h"
#include "time_sync/WSPRTxSyncNTP.h"
#include "tx_hardware/TxHardwareDummy.h"
#include "tx_hardware/TxHardwareSI5351.h"
#include "tx_parameters_controller/TxParametersControllerLittleFS.h"
#include "wifi_parameters_controller/WifiParametersControllerLittleFS.h"

#define CONFIGURATION_PIN D6
#define DRIVER_AMP_ENABLE_PIN D5

OnboardFilterHardware filter_hardware(0x20);

#if defined(TIME_USE_ESP_WIFI)
WSPRTxSyncNTP tx_sync("uk.pool.ntp.org");
#elif defined(TIME_USE_DUMMY)
WSPRTxSyncDummy tx_sync(1);
#else
#error "Select a time sync method (e.g., TIME_USE_ESP_WIFI or TIME_USE_DUMMY)."
#endif

#ifdef TX_HARDWARE_USE_SI5351
TxHardwareSi5351 tx_hardware(0x60);
#elif defined(TX_HARDWARE_USE_DUMMY)
TxHardwareDummy tx_hardware(10000);
#else
#error "Select a TX hardware (e.g., TX_HARDWARE_USE_SI5351 or TX_HARDWARE_USE_DUMMY)."
#endif

TxParametersControllerLittleFs tx_parameters_controller;
WiFiParametersControllerLittleFS wifi_parameters_controller;
TxController tx_controller;

static inline void disable_driver_amplifier()
{
    pinMode(DRIVER_AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(DRIVER_AMP_ENABLE_PIN, LOW);
}

static inline void enable_driver_amplifier()
{
    pinMode(DRIVER_AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(DRIVER_AMP_ENABLE_PIN, HIGH);
}

static inline void keep_warm()
{
    filter_hardware.disable_all_filters();
    filter_hardware.enable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(
        TxParameters(100000, 0, TxParameters::DriveStrength::MEDIUM_POWER));
}

static void disable_keep_warm()
{
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.disable_output();
}

void setup()
{
    Serial.begin(115200);
    pinMode(CONFIGURATION_PIN, INPUT_PULLUP);
    delay(5000);

    Serial.print("Read of configuration pin: ");
    Serial.println(digitalRead(CONFIGURATION_PIN));

    Wire.begin();

    filter_hardware.begin();
    tx_parameters_controller.begin();

    Serial.println("Keeping warm");
    keep_warm();
    Serial.println("Starting WSPR transmitter...");

    tx_controller.begin(&tx_hardware, &filter_hardware, &tx_sync, &tx_parameters_controller);

    tx_controller.attach_pre_tx_function(
        []()
        {
            disable_keep_warm();
            enable_driver_amplifier();
            Serial.println("Pre-transmission function: Driver amplifier enabled.");
        });

    tx_controller.attach_post_tx_function(
        []()
        {
            disable_driver_amplifier();
            keep_warm();
            Serial.println("Post-transmission function: Driver amplifier disabled.");
        });

    if (digitalRead(CONFIGURATION_PIN) == LOW)
    {
        uint32_t start_time = millis();
        while (digitalRead(CONFIGURATION_PIN) == LOW)
        {
            if (millis() - start_time > 2000)
            {
                Serial.println("Entering configuration mode...");
                Serial.println("Configuration mode finished.");
                break;
            }
            yield();
        }
    }

#if defined(TIME_USE_ESP_WIFI)
    wifi_parameters_controller.begin();
    wifi_parameters_controller.set_ssid("VM8667336");
    wifi_parameters_controller.set_password("tqZtgcnn8xvq");
    Serial.println("Connecting to WiFi...");
#if defined(ESP8266) || defined(BOARD_FAMILY_ESP8266)
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin(wifi_parameters_controller.get_ssid(), wifi_parameters_controller.get_password());

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        yield();
    }
    Serial.println();
    Serial.println("Connected to WiFi.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif

    tx_parameters_controller.set_20m_tx_correction(13500);
}

void loop()
{
    tx_controller.transmit_wspr_message_on_20m();
    // tx_controller.transmit_wspr_message_on_15m();
    // tx_controller.transmit_wspr_message_on_10m();
}
