#include "JTEncode.h"
#include "secrets.h"

#include <Arduino.h>

#ifdef DEBUG_TOOLS_I2C_SCANNER
#include "DebugTools.h"
#endif

#ifdef ESP8266_USE_NTP_TIME
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32_USE_NTP_TIME
#include <WiFi.h>
#endif

#if defined(ESP8266_USE_NTP_TIME) || defined(ESP32_USE_NTP_TIME)
#include "WSPRTxSyncNTP.h"
const char *wifi_ssid = WIFI_SSID;
const char *wifi_password = WIFI_PASSWORD;
WSPRTxSyncNTP tx_sync("uk.pool.ntp.org");
#elif defined(USE_DUMMY_TIME)
#include "WSPRTxSyncDummy.h"
WSPRTxSyncDummy tx_sync(10000);
#endif

#ifdef USE_SI5351_TX_HARDWARE
#include "TxHardwareSI5351.h"
TxHardwareSi5351 tx_hardware(0x60);
#elif defined(USE_DUMMY_TX_HARDWARE)
#include "TxHardwareDummy.h"
TxHardwareDummy tx_hardware(10000);
#endif

TxHardwareTxParameters keep_warm_params(1000000, -27000,
                                        TxHardwareTxParameters::DriveStrength::LOW_POWER);
TxHardwareTxParameters tx_params_40m(14095600 + 1500, -27000,
                                     TxHardwareTxParameters::DriveStrength::HIGH_POWER);

uint8_t wspr_message[WSPR_SYMBOL_COUNT];
const char *wspr_callsign = WSPR_CALLSIGN;
const char *wspr_gridsquare = WSPR_GRIDSQUARE;
int wspr_dbm = WSPR_DBM;

void setup()
{
    Serial.begin(115200);

#ifdef DEBUG_TOOLS_I2C_SCANNER
    debug_tools::scan_i2c_bus();
#endif

    JTEncode encoder;
    encoder.wspr_encode(wspr_callsign, wspr_gridsquare, wspr_dbm, wspr_message);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

#if defined(ESP8266_USE_NTP_TIME) || defined(ESP32_USE_NTP_TIME)
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
}

void loop()
{
    uint32_t milliseconds_to_next_tx = tx_sync.get_milliseconds_to_next_tx_period();
    Serial.print("Milliseconds to next TX: ");
    Serial.println(milliseconds_to_next_tx);

    uint32_t target_time = millis() + milliseconds_to_next_tx;
    while (millis() < target_time)
    {
        yield();
    }
    tx_hardware.transmit_wspr_message(tx_params_40m, wspr_message);
}
