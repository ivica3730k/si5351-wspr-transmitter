#ifndef TX_PARAMETERS_H
#define TX_PARAMETERS_H

#define CALIBRATION_OFFSET -91200

#include "TxHardware.h"
TxHardwareTxParameters tx_params_20m(14095600, -91200,
                                     TxHardwareTxParameters::DriveStrength::MEDIUM_POWER);
TxHardwareTxParameters tx_params_15m(21095600, CALIBRATION_OFFSET,
                                     TxHardwareTxParameters::DriveStrength::MEDIUM_POWER);
TxHardwareTxParameters tx_params_10m(28124600, -39250,
                                     TxHardwareTxParameters::DriveStrength::MEDIUM_POWER);
TxHardwareTxParameters keep_warm_params(100000, CALIBRATION_OFFSET,
                                        TxHardwareTxParameters::DriveStrength::LOW_POWER);
#endif // TX_PARAMETERS_H