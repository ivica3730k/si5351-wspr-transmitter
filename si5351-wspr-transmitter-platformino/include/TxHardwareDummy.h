#ifndef TXHARDWARE_DUMMY_H
#define TXHARDWARE_DUMMY_H

#include "TxHardware.h"

class TxHardwareDummy : public TxHardware
{
  public:
    TxHardwareDummy(uint32_t dummy_tx_time_millis)
    {
        this->dummy_tx_time_millis = dummy_tx_time_millis;
    }

    void transmit_wspr_message(TxHardwareTxParameters tx_params, uint8_t *message) override
    {
#ifdef DEBUG_TX_HARDWARE_DUMMY
        Serial.println("Dummy TxHardware: Simulating transmission for");
        Serial.print(dummy_tx_time_millis);
        Serial.println(" milliseconds.");
#endif

        uint32_t start_time = millis();
        while (millis() - start_time < dummy_tx_time_millis)
        {
#ifdef BOARD_FAMILY_ESPRESSIF
            yield(); // Prevent watchdog reset
#endif
        }

#ifdef DEBUG_TX_HARDWARE_DUMMY
        Serial.println("Dummy TxHardware: Transmission complete.");
#endif
    }

  private:
    uint32_t dummy_tx_time_millis = 0;
};

#endif // TXHARDWARE_DUMMY_H
