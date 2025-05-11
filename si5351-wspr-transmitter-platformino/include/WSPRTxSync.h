#ifndef WSPRTX_SYNC_H
#define WSPRTX_SYNC_H

class WSPRTxSync
{
public:
    virtual uint32_t get_miliseconds_to_next_tx_period() const = 0;
};

#endif // WSPRTX_SYNC_H