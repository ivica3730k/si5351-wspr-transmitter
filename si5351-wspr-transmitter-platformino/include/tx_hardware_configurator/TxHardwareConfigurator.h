#ifndef TXHARDWARECONFIGURATOR_H
#define TXHARDWARECONFIGURATOR_H

#include "tx_hardware/TxHardware.h"
#include "tx_parameters_controller/TxParametersController.h"

#include <Arduino.h>

class TxHardwareConfigurator
{
  public:
    virtual void begin(TxHardware &txHardware, TxParametersController &txParametersController) = 0;

    virtual void set_20m_tx_frequency() = 0;
};

#endif // CONFIGURATOR_H