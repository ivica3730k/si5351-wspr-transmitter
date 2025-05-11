#ifndef TXHARDWARE_H
#define TXHARDWARE_H

#include "JTEncode.h"

#include <Arduino.h>

struct TxHardwareTxParameters
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

    TxHardwareTxParameters(uint64_t freq, int32_t corr, DriveStrength strength)
    {
        this->frequency = freq;
        this->correction = corr;
        this->drive_strength = strength;
    }
};

class TxHardware
{
  public:
    virtual void transmit_wspr_message(const TxHardwareTxParameters &tx_params,
                                       uint8_t *message) = 0;
};

#endif // TXHARDWARE_H
