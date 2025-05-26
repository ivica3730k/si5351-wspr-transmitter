#include "DebugTools.h"
#include "JTEncode.h"
#include "OnboardFilterHardware.h"
#include "TxParameters.h"
#include "secrets.h"

#include <Arduino.h>

OnboardFilterHardware filter_hardware(0x20);
#define AMP_ENABLE_PIN D5

#ifdef TIME_USE_ESP8266_WIFI_NTP
#include <ESP8266WiFi.h>
#endif

#ifdef TIME_USE_ESP32_WIFI_NTP
#include <WiFi.h>
#endif

#if defined(TIME_USE_ESP8266_WIFI_NTP) || defined(TIME_USE_ESP32_WIFI_NTP)
#include "WSPRTxSyncNTP.h"
const char *wifi_ssid = WIFI_SSID;
const char *wifi_password = WIFI_PASSWORD;
WSPRTxSyncNTP tx_sync("uk.pool.ntp.org");
#elif defined(TIME_USE_DUMMYY)
#include "WSPRTxSyncDummy.h"
WSPRTxSyncDummy tx_sync(10000);
#endif

#ifdef TX_HARDWARE_USE_SI5351
#include "TxHardwareSI5351.h"
TxHardwareSi5351 tx_hardware(0x60);
#elif defined(TX_HARDWARE_USE_DUMMY)
#include "TxHardwareDummy.h"
TxHardwareDummy tx_hardware(10000);
#endif

uint8_t wspr_message[WSPR_SYMBOL_COUNT];
const char *wspr_callsign = WSPR_CALLSIGN;
const char *wspr_gridsquare = WSPR_GRIDSQUARE;
int wspr_dbm = WSPR_DBM;

void disable_amplifier()
{
    pinMode(AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(AMP_ENABLE_PIN, LOW);
}

void enable_amplifier()
{
    pinMode(AMP_ENABLE_PIN, OUTPUT);
    digitalWrite(AMP_ENABLE_PIN, HIGH);
}

void setup()
{
    disable_amplifier();
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    Serial.begin(115200);
    delay(2000); // wait for platformino serial monitor to spawn after upload
    Wire.begin();

#ifdef DEBUG_TOOLS_I2C_SCANNER
    debug_tools::scan_i2c_bus();
#endif
    filter_hardware.begin();

#ifdef TEST_SUB_MEGAHERTZ_DUMP
    filter_hardware.enable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(keep_warm_params);
    block_forever();
#endif

#ifdef TEST_20M_LOW_PASS_FILTER_WITH_20M_SIGNAL
    enable_amplifier();
    filter_hardware.enable_20m_low_pass_filter();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(tx_params_20m);
    block_forever();
#endif

#ifdef TEST_15M_LOW_PASS_FILTER_WITH_15M_SIGNAL
    enable_amplifier();
    filter_hardware.enable_15m_low_pass_filter();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(tx_params_15m);
    block_forever();
#endif

#ifdef TEST_10M_LOW_PASS_FILTER_WITH_10M_SIGNAL
    enable_amplifier();
    filter_hardware.enable_10m_low_pass_filter();
    filter_hardware.disable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(tx_params_10m);
    block_forever();
#endif

#if defined(TIME_USE_ESP8266_WIFI_NTP) || defined(TIME_USE_ESP32_WIFI_NTP)
#ifdef DEBUG_WIFI_CONNECTING
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);
#endif

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
#ifdef DEBUG_WIFI_CONNECTING
        Serial.print(".");
#endif
    }

#ifdef DEBUG_WIFI_CONNECTING
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif
#endif

    JTEncode encoder;
    encoder.wspr_encode(wspr_callsign, wspr_gridsquare, wspr_dbm, wspr_message);
}

void wait_for_next_tx_period()
{
    uint32_t milliseconds_to_next_tx = tx_sync.get_milliseconds_to_next_tx_period();
    Serial.print("Milliseconds to next TX: ");
    Serial.println(milliseconds_to_next_tx);

    uint32_t target_time = millis() + milliseconds_to_next_tx;
    while (millis() < target_time)
    {
        yield();
    }
}

void transmit_wspr_message_on_20m()
{
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    filter_hardware.enable_20m_low_pass_filter();
    enable_amplifier();
    Serial.println("Transmitting WSPR message on 20m...");
    tx_hardware.transmit_wspr_message(tx_params_20m, wspr_message);
    Serial.println("WSPR message on 20m transmitted");
    disable_amplifier();
    filter_hardware.disable_all_filters();
}

void transmit_wspr_message_on_15m()
{
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    filter_hardware.enable_15m_low_pass_filter();
    enable_amplifier();
    Serial.println("Transmitting WSPR message on 15m...");
    tx_hardware.transmit_wspr_message(tx_params_15m, wspr_message);
    Serial.println("WSPR message on 15m transmitted");
    disable_amplifier();
    filter_hardware.disable_all_filters();
}

void transmit_wspr_message_on_10m()
{
    filter_hardware.disable_all_filters();
    filter_hardware.disable_sub_megahertz_dump();
    filter_hardware.enable_10m_low_pass_filter();
    enable_amplifier();
    Serial.println("Transmitting WSPR message on 10m...");
    tx_hardware.transmit_wspr_message(tx_params_10m, wspr_message);
    Serial.println("WSPR message on 10m transmitted");
    disable_amplifier();
    filter_hardware.disable_all_filters();
}

void keep_warm()
{
    filter_hardware.disable_all_filters();
    filter_hardware.enable_sub_megahertz_dump();
    tx_hardware.output_constant_tone(keep_warm_params);
}

void loop()
{
    keep_warm();
    Serial.println("Waiting for next TX period...");
    wait_for_next_tx_period();
    transmit_wspr_message_on_20m();
}