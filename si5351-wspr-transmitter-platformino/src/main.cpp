#include <Arduino.h>
#include <Wire.h>

#if defined(TIME_USE_ESP_WIFI)
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "TIME_USE_ESP_WIFI is defined but no ESP8266/ESP32 family was specified."
#endif
#endif

#include "OnboardFilterHardware.h"
#include "TxController.h"
#include "tx_hardware/TxHardwareDummy.h"
#include "tx_hardware/TxHardwareSI5351.h"
#include "tx_parameters_controller/TxParametersControllerLittleFS.h"
#include "secrets.h"

#define DRIVER_AMP_ENABLE_PIN D5

OnboardFilterHardware filter_hardware(0x20);

#if defined(TIME_USE_ESP_WIFI)
#include "time_sync/WSPRTxSyncNTP.h"

WSPRTxSyncNTP tx_sync("uk.pool.ntp.org");
#elif defined(TIME_USE_DUMMY)
#include "time_sync/WSPRTxSyncDummy.h"
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

TxController tx_controller;
TxParametersControllerLittleFs tx_parameters_controller;

static inline void disable_amplifier()
{
    pinMode(DRIVER_AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(DRIVER_AMP_ENABLE_PIN, LOW);
}

static inline void enable_amplifier()
{
    pinMode(DRIVER_AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(DRIVER_AMP_ENABLE_PIN, HIGH);
}

void setup()
{
    Serial.begin(115200);
    delay(1000); // Allow time for serial monitor to connect
    Wire.begin();

    tx_hardware.begin();
    filter_hardware.begin();
    tx_parameters_controller.begin();
    // tx_parameters_controller.wipe_all_storage(); // For testing purposes only; remove in production
    // tx_parameters_controller.set_callsign(WSPR_CALLSIGN);
    // tx_parameters_controller.set_locator(WSPR_GRIDSQUARE);
    // tx_parameters_controller.set_20m_tx_frequency(14095600);
    // tx_parameters_controller.set_20m_tx_correction(13000);
    // tx_parameters_controller.set_20m_tx_drive_strength(TxParameters::DriveStrength::HIGH_POWER);
    // tx_parameters_controller.set_20m_tx_power_dbm(20);

    // tx_parameters_controller.set_15m_tx_frequency(21095600);
    // tx_parameters_controller.set_15m_tx_correction(60000);
    // tx_parameters_controller.set_15m_tx_drive_strength(TxParameters::DriveStrength::MEDIUM_POWER);
    // tx_parameters_controller.set_15m_tx_power_dbm(17);

    // tx_parameters_controller.set_10m_tx_frequency(28124600);
    // tx_parameters_controller.set_10m_tx_correction(12500);
    // tx_parameters_controller.set_10m_tx_drive_strength(TxParameters::DriveStrength::MEDIUM_POWER);
    // tx_parameters_controller.set_10m_tx_power_dbm(17);

    tx_controller.begin(&tx_hardware, &filter_hardware, &tx_sync, &tx_parameters_controller,
                        &enable_amplifier, &disable_amplifier);

#if defined(TIME_USE_ESP_WIFI)
    Serial.println("Connecting to WiFi...");
#if defined(ESP8266) || defined(BOARD_FAMILY_ESP8266)
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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
}

void loop()
{
    tx_controller.transmit_wspr_message_on_20m();
    // tx_controller.transmit_wspr_message_on_15m();
    tx_controller.transmit_wspr_message_on_10m();
}
