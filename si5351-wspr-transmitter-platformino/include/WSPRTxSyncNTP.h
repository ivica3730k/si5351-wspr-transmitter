#ifndef WSPRTX_SYNC_ESP8266_NTP_H
#define WSPRTX_SYNC_ESP8266_NTP_H

#define NTP_PACKET_SIZE 48

#include "WSPRTxSync.h"

#include <Arduino.h>
#include <WiFiUdp.h>

#ifdef TIME_USE_ESP8266_WIFI_NTP
#include <ESP8266WiFi.h>
#elif defined TIME_USE_ESP32_WIFI_NTP
#include <WiFi.h>
#endif

class WSPRTxSyncNTP : public WSPRTxSync
{
  public:
    WSPRTxSyncNTP()
    {
        this->ntp_server_fqdn = "time.nist.gov";
    }

    WSPRTxSyncNTP(const char *dns_name)
    {
        this->ntp_server_fqdn = dns_name;
    }

    uint32_t get_milliseconds_to_next_tx_period() const override
    {
        IPAddress ntp_server_ip;
        resolve_ntp_server(ntp_server_ip);

        WiFiUDP udp;
        uint16_t port = return_usable_udp_port(udp);
        udp.begin(port);

        byte ntp_buffer[NTP_PACKET_SIZE];
        prepare_ntp_request(ntp_buffer);

        send_and_wait_for_ntp_response(udp, ntp_server_ip, ntp_buffer);

        return calculate_milliseconds_to_next_tx(ntp_buffer);
    }

  private:
    const char *ntp_server_fqdn;

    void resolve_ntp_server(IPAddress &ip) const
    {
        while (!WiFi.hostByName(this->ntp_server_fqdn, ip))
        {
#ifdef DEBUG_NTP_TIME_OBTAINING
            Serial.println("DNS lookup failed, retrying...");
#endif
            delay(100);
        }
    }

    static uint16_t return_random_port_number()
    {
        return random(10000, 60000);
    }

    static uint16_t return_usable_udp_port(WiFiUDP &udp)
    {
        for (int i = 0; i < 5; ++i)
        {
            uint16_t port = return_random_port_number();
            if (udp.begin(port))
            {
#ifdef DEBUG_NTP_TIME_OBTAINING
                Serial.print("UDP bound to port: ");
                Serial.println(port);
#endif
                return port;
            }
        }

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

        return 2390;
    }

    static void prepare_ntp_request(byte *buffer)
    {
        memset(buffer, 0, NTP_PACKET_SIZE);
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
            udp.write(buffer, NTP_PACKET_SIZE);
            udp.endPacket();

            delay(100);
            if (udp.parsePacket())
            {
                udp.read(buffer, NTP_PACKET_SIZE);
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
        unsigned long high_word = word(buffer[40], buffer[41]);
        unsigned long low_word = word(buffer[42], buffer[43]);
        unsigned long secs_since_1900 = (high_word << 16) | low_word;

        unsigned long high_frac = word(buffer[44], buffer[45]);
        unsigned long low_frac = word(buffer[46], buffer[47]);
        unsigned long frac = (high_frac << 16) | low_frac;

        const unsigned long seventy_years = 2208988800UL;
        unsigned long epoch = secs_since_1900 - seventy_years;

        int minute = (epoch % 3600) / 60;
        int second = epoch % 60;

        double fraction = frac / 4294967296.0;
        uint32_t millis = static_cast<uint32_t>(fraction * 1000);

        int minutes_to_wait = ((minute + 1) % 2);
        int seconds_to_wait = (minutes_to_wait * 60) + (60 - second);

        uint32_t total_millis_to_next_tx = (seconds_to_wait * 1000UL) - millis;

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
        Serial.print(second);
        Serial.print('.');
        Serial.println(millis);

        Serial.print("Milliseconds to next TX period: ");
        Serial.println(total_millis_to_next_tx);
#endif

        return total_millis_to_next_tx;
    }
};

#endif // WSPRTX_SYNC_ESP8266_NTP_H
