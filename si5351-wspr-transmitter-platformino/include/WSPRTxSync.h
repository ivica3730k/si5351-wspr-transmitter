#ifndef WSPRTX_SYNC_H
#define WSPRTX_SYNC_H

class WSPRTxSync
{
public:
    virtual int get_seconds_to_next_tx_period() const = 0;
};

#endif // WSPRTX_SYNC_H