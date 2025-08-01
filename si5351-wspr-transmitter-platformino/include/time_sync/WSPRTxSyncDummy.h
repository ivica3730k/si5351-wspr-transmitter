#ifndef WSPRTX_SYNC_DUMMY_H
#define WSPRTX_SYNC_DUMMY_H

#include "time_sync/WSPRTxSync.h"

#include <Arduino.h>

class WSPRTxSyncDummy : public WSPRTxSync
{
  public:
    WSPRTxSyncDummy(uint32_t millis_to_next_tx)
    {
        this->milliseconds_to_next_tx = millis_to_next_tx;
    }

    uint32_t get_milliseconds_to_next_tx_period() const override
    {
        return milliseconds_to_next_tx;
    }

  private:
    uint32_t milliseconds_to_next_tx = 0;
};

#endif // WSPRTX_SYNC_DUMMY_H
