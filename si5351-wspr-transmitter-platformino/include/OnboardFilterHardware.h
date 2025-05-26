#ifndef FILTER_HARDWARE_H
#define FILTER_HARDWARE_H

#include "MCP23017.h"

#include <Arduino.h>
#include <Wire.h>

class OnboardFilterHardware
{
  public:
    OnboardFilterHardware(uint8_t i2c_address)
    {
        this->address = i2c_address;
    }
    ~OnboardFilterHardware()
    {
        delete this->mcp;
    }

    void begin()
    {
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.print("OnboardFilterHardware: Begining with I2C address: ");
        Serial.println(this->address, HEX);
        #endif
        this->mcp = new MCP23017(this->address);
        this->mcp->begin(this->address);
        this->mcp->init();
        // pinmodes for built in low frequency dummy load
        this->mcp->pinMode(MCP23017Pin::GPA7, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB7, OUTPUT);
        // pinmodes for 20m low pass filter
        this->mcp->pinMode(MCP23017Pin::GPA0, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB0, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPA1, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB1, OUTPUT);
        // pinmodes for 10m low pass filter
        this->mcp->pinMode(MCP23017Pin::GPA2, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB2, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPA3, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB3, OUTPUT);
        // pinmodes for 15m low pass filter
        this->mcp->pinMode(MCP23017Pin::GPA4, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB4, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPA5, OUTPUT);
        this->mcp->pinMode(MCP23017Pin::GPB5, OUTPUT);
    }

    void enable_sub_megahertz_dump()
    {
        this->disable_all_filters();
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Enabling sub-megahertz dump.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA7, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB7, LOW);
    }

    void disable_sub_megahertz_dump()
    {
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Disabling sub-megahertz dump.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA7, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB7, HIGH);
    }

    void disable_all_filters()
    {   
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Disabling all filters.");
        #endif
        this->disable_sub_megahertz_dump();
        this->disable_20m_low_pass_filter();
        this->disable_10m_low_pass_filter();
        this->disable_15m_low_pass_filter();
    }

    void enable_20m_low_pass_filter()
    {
        this->disable_all_filters();
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Enabling 20m low pass filter.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA0, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB0, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPA1, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB1, LOW);
    }

    void disable_20m_low_pass_filter()
    {
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Disabling 20m low pass filter.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA0, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB0, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPA1, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB1, HIGH);
    }

    void enable_10m_low_pass_filter()
    {
        this->disable_all_filters();
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Enabling 10m low pass filter.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA2, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB2, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPA3, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB3, LOW);
    }

    void disable_10m_low_pass_filter()
    {
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Disabling 10m low pass filter.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA2, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB2, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPA3, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB3, HIGH);
    }

    void enable_15m_low_pass_filter()
    {
        this->disable_all_filters();
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Enabling 15m low pass filter.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA4, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB4, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPA5, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB5, LOW);
    }

    void disable_15m_low_pass_filter()
    {
        #ifdef DEBUG_FILTER_HARDWARE
        Serial.println("OnboardFilterHardware: Disabling 15m low pass filter.");
        #endif
        this->mcp->digitalWrite(MCP23017Pin::GPA4, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPB4, LOW);
        this->mcp->digitalWrite(MCP23017Pin::GPA5, HIGH);
        this->mcp->digitalWrite(MCP23017Pin::GPB5, HIGH);
    }

  private:
    uint8_t address;
    MCP23017 *mcp;
};

#endif // FILTER_HARDWRE_H