#ifndef WSPRTX_SYNC_ESP8266_NTP_H
#define WSPRTX_SYNC_ESP8266_NTP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "WSPRTxSync.h"

class WSPRTxSyncESP8266NTP : public WSPRTxSync
{
public:
    WSPRTxSyncESP8266NTP() {}

    uint32_t get_miliseconds_to_next_tx_period() const override
    {
        IPAddress ntpServerIP;
        resolve_ntp_server(ntpServerIP);

        WiFiUDP udp;
        bind_udp_port(udp);

        byte packetBuffer[48];
        prepare_ntp_request(packetBuffer);

        send_and_wait_for_ntp_response(udp, ntpServerIP, packetBuffer);

        return calculate_milliseconds_to_next_tx(packetBuffer);
    }

private:
    static void resolve_ntp_server(IPAddress &ip)
    {
        const char *ntpServerName = "time.nist.gov";
        while (!WiFi.hostByName(ntpServerName, ip))
        {
#ifdef DEBUG_NTP_TIME_OBTAINING
            Serial.println("DNS lookup failed, retrying...");
#endif
            delay(100);
        }
    }

    static void bind_udp_port(WiFiUDP &udp)
    {
        bool bound = false;
        for (int i = 0; i < 5; ++i)
        {
            unsigned int port = random(10000, 60000);
            if (udp.begin(port))
            {
#ifdef DEBUG_NTP_TIME_OBTAINING
                Serial.print("UDP bound to port: ");
                Serial.println(port);
#endif
                bound = true;
                break;
            }
        }
        if (!bound)
        {
#ifdef DEBUG_NTP_TIME_OBTAINING
            Serial.println("Using fallback UDP port 2390");
#endif
            while (!udp.begin(2390))
            {
#ifdef DEBUG_NTP_TIME_OBTAINING
                Serial.println("Fallback port bind failed, retrying...");
#endif
                delay(100);
            }
        }
    }

    static void prepare_ntp_request(byte *buffer)
    {
        memset(buffer, 0, 48);
        buffer[0] = 0b11100011;
        buffer[1] = 0;
        buffer[2] = 6;
        buffer[3] = 0xEC;
        buffer[12] = 49;
        buffer[13] = 0x4E;
        buffer[14] = 49;
        buffer[15] = 52;
    }

    static void send_and_wait_for_ntp_response(WiFiUDP &udp, IPAddress &ip, byte *buffer)
    {
        while (true)
        {
            udp.beginPacket(ip, 123);
            udp.write(buffer, 48);
            udp.endPacket();

            delay(100);
            if (udp.parsePacket())
            {
                udp.read(buffer, 48);
                break;
            }

#ifdef DEBUG_NTP_TIME_OBTAINING
            Serial.println("No NTP response, retrying...");
#endif
            delay(100);
        }
    }

    static uint32_t calculate_milliseconds_to_next_tx(const byte *buffer)
    {
        unsigned long highWord = word(buffer[40], buffer[41]);
        unsigned long lowWord = word(buffer[42], buffer[43]);
        unsigned long secsSince1900 = (highWord << 16) | lowWord;

        unsigned long highFrac = word(buffer[44], buffer[45]);
        unsigned long lowFrac = word(buffer[46], buffer[47]);
        unsigned long frac = (highFrac << 16) | lowFrac;

        const unsigned long seventyYears = 2208988800UL;
        unsigned long epoch = secsSince1900 - seventyYears;

        int minute = (epoch % 3600) / 60;
        int second = epoch % 60;

        double fractionOfSecond = frac / 4294967296.0; // 2^32
        uint32_t millis = static_cast<uint32_t>(fractionOfSecond * 1000);

        int minutesToWait = ((minute + 1) % 2);
        int secondsToWait = (minutesToWait * 60) + (60 - second);

        uint32_t totalMillisToNextTx = (secondsToWait * 1000UL) - millis;

#ifdef DEBUG_NTP_TIME_OBTAINING
        Serial.print("Current UTC time: ");
        Serial.print((epoch % 86400L) / 3600);
        Serial.print(':');
        if (minute < 10) Serial.print('0');
        Serial.print(minute);
        Serial.print(':');
        if (second < 10) Serial.print('0');
        Serial.print(second);
        Serial.print('.');
        Serial.println(millis);

        Serial.print("Milliseconds to next TX period: ");
        Serial.println(totalMillisToNextTx);
#endif

        return totalMillisToNextTx;
    }

    static int calculate_seconds_to_next_tx(const byte *buffer)
    {
        unsigned long highWord = word(buffer[40], buffer[41]);
        unsigned long lowWord = word(buffer[42], buffer[43]);
        unsigned long secsSince1900 = (highWord << 16) | lowWord;
        const unsigned long seventyYears = 2208988800UL;
        unsigned long epoch = secsSince1900 - seventyYears;

        int minute = (epoch % 3600) / 60;
        int second = epoch % 60;

        int minutesToWait = ((minute + 1) % 2);
        int secondsToWait = (minutesToWait * 60) + (60 - second);

#ifdef DEBUG_NTP_TIME_OBTAINING
        Serial.print("Current UTC time: ");
        Serial.print((epoch % 86400L) / 3600);
        Serial.print(':');
        if (minute < 10)
            Serial.print('0');
        Serial.print(minute);
        Serial.print(':');
        if (second < 10)
            Serial.print('0');
        Serial.println(second);

        Serial.print("Seconds to next TX period: ");
        Serial.println(secondsToWait);
#endif

        return secondsToWait;
    }
};

#endif // WSPRTX_SYNC_ESP8266_NTP_H
