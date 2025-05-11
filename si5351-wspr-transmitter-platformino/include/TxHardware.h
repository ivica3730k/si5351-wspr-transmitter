#ifndef TXHARDWARE_H
#define TXHARDWARE_H

#include "JTEncode.h"

#include <stdint.h>

struct TxHardwareTxParameters
{
    enum DriveStrength
    {
        LOW_POWER = 1,
        MEDIUM_POWER = 2,
        HIGH_POWER = 3
    };

    uint64_t frequency;
    int32_t correction;
    DriveStrength drive_strength;

    // Constructor
    TxHardwareTxParameters(uint64_t freq, int32_t corr, DriveStrength power)
        : frequency(freq), correction(corr), drive_strength(power)
    {
    }
};

class TxHardware
{
  public:
    virtual void transmit_wspr_message(TxHardwareTxParameters tx_params, uint8_t *message) = 0;
};

#endif // TXHARDWARE_H
