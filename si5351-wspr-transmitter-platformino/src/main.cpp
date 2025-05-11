#include "JTEncode.h"
#include "secrets.h"

#include <Arduino.h>

#ifdef ESP8266_USE_NTP_TIME
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32_USE_NTP_TIME
#include <WiFi.h>
#endif

#if defined(ESP8266_USE_NTP_TIME) || defined(ESP32_USE_NTP_TIME)
#include "WSPRTxSyncNTP.h"

const char *ssid = WIFI_SSID;
const char *pass = WIFI_PASSWORD;

WSPRTxSyncNTP tx_sync = WSPRTxSyncNTP();

#elif USE_DUMMY_TIME
#include "WSPRTxSyncDummy.h"
WSPRTxSyncDummy tx_sync = WSPRTxSyncDummy(10000);

#endif

#ifdef USE_SI5351_TX_HARDWARE
#include "TxHardwareSI5351.h"

TxHardwareSI5351 tx_hardware = TxHardwareSI5351(0x60);

#elif USE_DUMMY_TX_HARDWARE
#include "TxHardwareDummy.h"

TxHardwareDummy tx_hardware = TxHardwareDummy(10000);

#endif

TxHardwareTxParameters keep_warm_params(1000000, -27000, TxHardwareTxParameters::LOW_POWER);
TxHardwareTxParameters tx_params_40m(14095600 + 1500, -27000, TxHardwareTxParameters::HIGH_POWER);

uint8_t wspr_message[WSPR_SYMBOL_COUNT];
const char *callsign = WSPR_CALLSIGN;
const char *gridsquare = WSPR_GRIDSQUARE;
int dbm = WSPR_DBM;

void setup()
{
    Serial.begin(115200);
    JTEncode jtencode;
    jtencode.wspr_encode(callsign, gridsquare, dbm, wspr_message);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

#if defined(ESP8266_USE_NTP_TIME) || defined(ESP32_USE_NTP_TIME)

#ifdef DEBUG_WIFI_CONNECTING
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
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
    int miliseconds_to_next_tx = tx_sync.get_miliseconds_to_next_tx_period();
    Serial.print("Milliseconds to next TX: ");
    Serial.println(miliseconds_to_next_tx);
    uint32_t target_millis = millis() + miliseconds_to_next_tx;
    while (millis() < target_millis)
    {
        yield();
    }
    tx_hardware.transmit_wspr_message(tx_params_40m, wspr_message);
}