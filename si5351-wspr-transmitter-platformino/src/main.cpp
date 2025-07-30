#include "DebugTools.h"
#include "JTEncode.h"
#include "OnboardFilterHardware.h"
#include "TxParameters.h"
#include "secrets.h"

#include <Arduino.h>

#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
OnboardFilterHardware filter_hardware(0x20);
#define DRIVER_AMP_ENABLE_PIN D5
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER

#ifdef TIME_USE_ESP8266_WIFI_NTP
#include <ESP8266WiFi.h>
#endif // TIME_USE_ESP8266_WIFI_NTP

#ifdef TIME_USE_ESP32_WIFI_NTP
#include <WiFi.h>
#endif // TIME_USE_ESP32_WIFI_NTP

#if defined(TIME_USE_ESP8266_WIFI_NTP) || defined(TIME_USE_ESP32_WIFI_NTP)
#include "WSPRTxSyncNTP.h"
const char *wifi_ssid = WIFI_SSID;
const char *wifi_password = WIFI_PASSWORD;
WSPRTxSyncNTP tx_sync("uk.pool.ntp.org");
#elif defined(TIME_USE_DUMMY)
#include "WSPRTxSyncDummy.h"
WSPRTxSyncDummy tx_sync(10000);
#endif // TIME_USE_ESP8266_WIFI_NTP or TIME_USE_ESP32_WIFI_NTP or TIME_USE_DUMMYY

#ifdef TX_HARDWARE_USE_SI5351
#include "TxHardwareSI5351.h"
TxHardwareSi5351 tx_hardware(0x60);
#elif defined(TX_HARDWARE_USE_DUMMY)
#include "TxHardwareDummy.h"
TxHardwareDummy tx_hardware(10000);
#endif // TX_HARDWARE_USE_SI5351 or TX_HARDWARE_USE_DUMMY

uint8_t wspr_message[WSPR_SYMBOL_COUNT];
const char *wspr_callsign = WSPR_CALLSIGN;
const char *wspr_gridsquare = WSPR_GRIDSQUARE;
int wspr_dbm = WSPR_DBM;

#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
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
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER

void setup()
{
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    filter_hardware.begin();
    disable_driver_amplifier();
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    Serial.begin(115200);
    delay(2000); // wait for platformino serial monitor to spawn after upload
    Serial.println("Starting WSPR transmitter...");
    Wire.begin();

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

#if defined(TIME_USE_ESP8266_WIFI_NTP) || defined(TIME_USE_ESP32_WIFI_NTP)
#ifdef DEBUG_WIFI_CONNECTING
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);
#endif // TIME_USE_ESP8266_WIFI_NTP or TIME_USE_ESP32_WIFI_NTP

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
#ifdef DEBUG_WIFI_CONNECTING
        Serial.print(".");
#endif // TIME_USE_ESP8266_WIFI_NTP or TIME_USE_ESP32_WIFI_NTP
    }

#ifdef DEBUG_WIFI_CONNECTING
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif // DEBUG_WIFI_CONNECTING
#endif // TIME_USE_ESP8266_WIFI_NTP or TIME_USE_ESP32_WIFI_NTP

    JTEncode encoder;
    encoder.wspr_encode(wspr_callsign, wspr_gridsquare, wspr_dbm, wspr_message);
}

void wait_for_next_tx_period()
{
    uint32_t milliseconds_to_next_tx = tx_sync.get_milliseconds_to_next_tx_period();
    uint32_t target_time = millis() + milliseconds_to_next_tx;
    while (millis() < target_time)
    {
        yield();
    }
}

void transmit_wspr_message_on_20m()
{
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    filter_hardware.enable_20m_low_pass_filter();
    enable_driver_amplifier();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    Serial.println("Transmitting WSPR message on 20m...");
    tx_hardware.transmit_wspr_message(tx_params_20m, wspr_message);
    Serial.println("WSPR message on 20m transmitted");
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    disable_driver_amplifier();
    filter_hardware.disable_all_filters();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
}

void transmit_wspr_message_on_15m()
{
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    filter_hardware.enable_15m_low_pass_filter();
    enable_driver_amplifier();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    Serial.println("Transmitting WSPR message on 15m...");
    tx_hardware.transmit_wspr_message(tx_params_15m, wspr_message);
    Serial.println("WSPR message on 15m transmitted");
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    disable_driver_amplifier();
    filter_hardware.disable_all_filters();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
}

void transmit_wspr_message_on_10m()
{
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    filter_hardware.enable_10m_low_pass_filter();
    enable_driver_amplifier();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    Serial.println("Transmitting WSPR message on 10m...");
    tx_hardware.transmit_wspr_message(tx_params_10m, wspr_message);
    Serial.println("WSPR message on 10m transmitted");
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    disable_driver_amplifier();
    filter_hardware.disable_all_filters();
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
}

void keep_warm()
{
#ifdef MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
    filter_hardware.disable_all_filters();
    filter_hardware.enable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(keep_warm_params);
#endif // MULTI_BAND_BOARD_WITH_FILTERS_AND_AMPLIFIER
}

void loop()
{
    keep_warm();
    Serial.println("Waiting for next TX period...");
    wait_for_next_tx_period();
    transmit_wspr_message_on_20m();
    keep_warm();
    Serial.println("Waiting for next TX period...");
    wait_for_next_tx_period();
    transmit_wspr_message_on_10m();
}