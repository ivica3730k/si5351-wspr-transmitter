#ifndef TXHARDWARE_H
#define TXHARDWARE_H

#include "JTEncode.h"
#include <stdint.h>

struct TxHardwareTxParameters
{
    uint64_t frequency;
    int32_t correction = 0;
    enum DriveStrength
    {
        LOW_POWER = 1,
        MEDIUM_POWER = 2,
        HIGH_POWER = 3
    } drive_strength = LOW_POWER;
};

class TxHardware
{
public:
    virtual void transmit_wspr_message(TxHardwareTxParameters tx_params, uint8_t *message) = 0;
};

#endif // TXHARDWARE_H
