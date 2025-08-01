#include "DebugTools.h"
#include "JTEncode.h"
#include "OnboardFilterHardware.h"
#include "TxController.h"
#include "secrets.h"
#include "tx_parameters_controller/TxParametersControllerLittleFS.h"

#include <Arduino.h>

OnboardFilterHardware filter_hardware(0x20);
#define DRIVER_AMP_ENABLE_PIN D5

#if defined(TIME_USE_ESP_WIFI)
#include "../lib/WiFiManager/WiFiManager.h"
#include "time_sync/WSPRTxSyncNTP.h"
const char *wifi_ssid = WIFI_SSID;
const char *wifi_password = WIFI_PASSWORD;
WSPRTxSyncNTP tx_sync("uk.pool.ntp.org");
#elif defined(TIME_USE_DUMMY)
#include "time_sync/WSPRTxSyncDummy.h"
WSPRTxSyncDummy tx_sync(10000);
#endif // TIME_USE_ESP_WIFI or TIME_USE_DUMMY

#ifdef TX_HARDWARE_USE_SI5351
#include "tx_hardware/TxHardwareSI5351.h"
TxHardwareSi5351 tx_hardware(0x60);
#elif defined(TX_HARDWARE_USE_DUMMY)
#include "#include " tx_hardware / TxHardwareDummy.h ""
TxHardwareDummy tx_hardware(10000);
#endif // TX_HARDWARE_USE_SI5351 or TX_HARDWARE_USE_DUMMY

TxParametersControllerLittleFs tx_parameters_controller;
TxController tx_controller;

uint8_t wspr_message[WSPR_SYMBOL_COUNT];
const char *wspr_callsign = WSPR_CALLSIGN;
const char *wspr_gridsquare = WSPR_GRIDSQUARE;
int wspr_dbm = WSPR_DBM;

void disable_driver_amplifier()
{
    pinMode(DRIVER_AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(DRIVER_AMP_ENABLE_PIN, LOW);
}

void enable_driver_amplifier()
{
    pinMode(DRIVER_AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(DRIVER_AMP_ENABLE_PIN, HIGH);
}

void keep_warm()
{
    filter_hardware.disable_all_filters();
    filter_hardware.enable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(
        TxParameters(100000, -91200, TxParameters::DriveStrength::MEDIUM_POWER));
}

void disable_keep_warm()
{
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.disable_output();
}

void setup()
{
    Serial.begin(115200);
    delay(5000); // wait for platformino serial monitor to spawn after upload
    Wire.begin();

    filter_hardware.begin();
    tx_parameters_controller.begin();
    Serial.print("Keeping warm");
    keep_warm();
    Serial.println("Starting WSPR transmitter...");

#ifdef DEBUG_TOOLS_I2C_SCANNER
    debug_tools::scan_i2c_bus();
#endif // DEBUG_TOOLS_I2C_SCANNER

#ifdef TEST_SUB_MEGAHERTZ_DUMP
    filter_hardware.enable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(keep_warm_params);
    block_forever();
#endif // TEST_SUB_MEGAHERTZ_DUMP

#ifdef TEST_20M_LOW_PASS_FILTER_WITH_20M_SIGNAL
    enable_driver_amplifier();
    filter_hardware.enable_20m_low_pass_filter();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(tx_params_20m);
    block_forever();
#endif // TEST_20M_LOW_PASS_FILTER_WITH_20M_SIGNAL

#ifdef TEST_15M_LOW_PASS_FILTER_WITH_15M_SIGNAL
    enable_driver_amplifier();
    filter_hardware.enable_15m_low_pass_filter();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(tx_params_15m);
    block_forever();
#endif // TEST_15M_LOW_PASS_FILTER_WITH_15M_SIGNAL

#ifdef TEST_10M_LOW_PASS_FILTER_WITH_10M_SIGNAL
    enable_driver_amplifier();
    filter_hardware.enable_10m_low_pass_filter();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(tx_params_10m);
    block_forever();
#endif // TEST_10M_LOW_PASS_FILTER_WITH_10M_SIGNAL

#if defined(TIME_USE_ESP_WIFI)
    WiFiManager wifi_manager;
    wifi_manager.autoConnect();

#endif // TIME_USE_ESP_WIFI
    JTEncode encoder;
    encoder.wspr_encode(wspr_callsign, wspr_gridsquare, wspr_dbm, wspr_message);

    tx_parameters_controller.set_20m_tx_correction(-91200);
    tx_controller.begin(&tx_hardware, &filter_hardware, &tx_sync, &tx_parameters_controller,
                        wspr_message);
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
}

void loop()
{
    tx_controller.transmit_wspr_message_on_20m();
    tx_controller.transmit_wspr_message_on_15m();
    tx_controller.transmit_wspr_message_on_10m();
}