#ifndef TXHARDWARE_DUMMY_H
#define TXHARDWARE_DUMMY_H

#include "TxHardware.h"

class TxHardwareDummy : public TxHardware
{
  public:
    TxHardwareDummy(uint32_t tx_duration_ms)
    {
        this->tx_duration_millis = tx_duration_ms;
    }
    void transmit_wspr_message(const TxHardwareTxParameters &tx_params, uint8_t *message) override
    {
#ifdef DEBUG_TX_HARDWARE_USE_DUMMY
        Serial.print("Dummy TxHardware: Simulating transmission for ");
        Serial.print(tx_duration_millis);
        Serial.println(" milliseconds.");
#endif
        uint32_t start_time = millis();
        while (millis() - start_time < tx_duration_millis)
        {
#ifdef BOARD_FAMILY_ESPRESSIF
            yield();
#endif
        }
#ifdef DEBUG_TX_HARDWARE_USE_DUMMY
        Serial.println("Dummy TxHardware: Transmission complete.");
#endif
    }

  private:
    uint32_t tx_duration_millis = 0;
};

#endif // TXHARDWARE_DUMMY_H
