#ifndef WSPRTX_SYNC_DUMMY_H
#define WSPRTX_SYNC_DUMMY_H

#include "WSPRTxSync.h"

#include <Arduino.h>

class WSPRTxSyncDummy : public WSPRTxSync
{
  public:
    WSPRTxSyncDummy(uint32_t milliseconds_to_next_tx_period)
    {
        this->milliseconds_to_next_tx_period = milliseconds_to_next_tx_period;
    }

    uint32_t get_miliseconds_to_next_tx_period() const override
    {
        return milliseconds_to_next_tx_period;
    }

  private:
    uint32_t milliseconds_to_next_tx_period = 0;
};

#endif // WSPRTX_SYNC_DUMMY_H