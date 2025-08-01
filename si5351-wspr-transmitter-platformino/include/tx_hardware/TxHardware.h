#ifndef TXHARDWARE_H
#define TXHARDWARE_H

#include "JTEncode.h"
#include "tx_parameters_controller/TxParametersController.h"

#include <Arduino.h>

class TxHardware
{
  public:
    virtual void transmit_wspr_message(const TxParameters &tx_params, uint8_t *message) = 0;
    virtual void output_constant_tone(const TxParameters &tx_params) = 0;
};

#endif // TXHARDWARE_H
