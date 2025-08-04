#ifndef TXPARAMETERSCONTROLLER_H
#define TXPARAMETERSCONTROLLER_H

#include "TxParametersController.h"

#include <Arduino.h>
#include <LittleFS.h>

struct TxParameters
{
    enum class DriveStrength : uint8_t
    {
        LOW_POWER = 1,
        MEDIUM_POWER = 2,
        HIGH_POWER = 3
    };

    uint64_t frequency;
    int32_t correction;
    DriveStrength drive_strength;

    TxParameters() : frequency(0), correction(0), drive_strength(DriveStrength::MEDIUM_POWER) {}

    TxParameters(uint64_t freq, int32_t corr, DriveStrength strength)
        : frequency(freq), correction(corr), drive_strength(strength)
    {
    }
};

class TxParametersController
{
  public:
    virtual String get_callsign() = 0;
    virtual void set_callsign(const String &callsign) = 0;

    virtual String get_locator() = 0;
    virtual void set_locator(const String &locator) = 0;

    virtual int get_tx_power() = 0;
    virtual void set_tx_power(int power) = 0;

    virtual TxParameters get_tx_params_20m() = 0;
    virtual void set_20m_tx_frequency(uint64_t frequency) = 0;
    virtual void set_20m_tx_correction(int32_t correction) = 0;
    virtual void set_20m_tx_drive_strength(TxParameters::DriveStrength strength) = 0;

    virtual TxParameters get_tx_params_15m() = 0;
    virtual void set_15m_tx_frequency(uint64_t frequency) = 0;
    virtual void set_15m_tx_correction(int32_t correction) = 0;
    virtual void set_15m_tx_drive_strength(TxParameters::DriveStrength strength) = 0;

    virtual TxParameters get_tx_params_10m() = 0;
    virtual void set_10m_tx_frequency(uint64_t frequency) = 0;
    virtual void set_10m_tx_correction(int32_t correction) = 0;
    virtual void set_10m_tx_drive_strength(TxParameters::DriveStrength strength) = 0;
};

#endif // TXPARAMETERSCONTROLLER_H
