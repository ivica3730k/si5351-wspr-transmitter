
#ifdef ESP_8266_USE_NTP_TIME
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "WSPRTxSyncESP8266NTP.h"
#include "secrets.h"
#include "TxHardwareSI5351.h"
#include "JTEncode.h"

const char *ssid = WIFI_SSID;
const char *pass = WIFI_PASSWORD;

WSPRTxSyncESP8266NTP tx_sync = WSPRTxSyncESP8266NTP();

#endif

TxHardwareSI5351 tx_hardware = TxHardwareSI5351(0x60);

TxHardwareTxParameters keep_warm_params = {
    .frequency = 1000000,
    .correction = -27000,
    .drive_strength = TxHardwareTxParameters::DriveStrength::LOW_POWER,
};

TxHardwareTxParameters tx_params_40m = {
    .frequency = 14095600 + 1500,
    .correction = -27000,
    .drive_strength = TxHardwareTxParameters::DriveStrength::HIGH_POWER,
};

uint8_t wspr_message[WSPR_SYMBOL_COUNT]; // populate later in setup() so you dont have to keep a global variable of JTEncode.h
const char *callsign = WSPR_CALLSIGN;
const char *gridsquare = WSPR_GRIDSQUARE;
int dbm = WSPR_DBM;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    JTEncode jtencode;
    jtencode.wspr_encode(callsign, gridsquare, dbm, wspr_message);


    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

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