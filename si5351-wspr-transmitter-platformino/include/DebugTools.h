#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include <Arduino.h>
#include <Wire.h>

namespace debug_tools
{

constexpr uint8_t I2C_MIN_ADDRESS = 1;
constexpr uint8_t I2C_MAX_ADDRESS = 127;
constexpr unsigned long SCAN_DELAY_MS = 1000;

inline void scan_i2c_bus()
{
    Wire.begin();
    Serial.println("Starting I2C bus scan...");

    for (uint8_t address = I2C_MIN_ADDRESS; address < I2C_MAX_ADDRESS; ++address)
    {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    Serial.println("I2C scan complete.");
}

} // namespace debug_tools

#endif // DEBUG_TOOLS_H
