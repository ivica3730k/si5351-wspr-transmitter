#ifndef TXCONTROLLER_H
#define TXCONTROLLER_H

#include "OnboardFilterHardware.h"
#include "time_sync/WSPRTxSync.h"
#include "tx_hardware/TxHardware.h"
#include "tx_parameters_controller/TxParametersController.h"

class TxController
{
  public:
    TxController(){};
    ~TxController(){};

    void begin(TxHardware *tx_hardware, OnboardFilterHardware *filter_hardware, WSPRTxSync *tx_sync,
               TxParametersController *tx_parameters_controller)
    {
        this->tx_hardware = tx_hardware;
        this->filter_hardware = filter_hardware;
        this->tx_sync = tx_sync;
        this->tx_parameters_controller = tx_parameters_controller;
        JTEncode encoder;
        encoder.wspr_encode(tx_parameters_controller->get_callsign().c_str(),
                            tx_parameters_controller->get_locator().c_str(),
                            tx_parameters_controller->get_tx_power(), this->wspr_message);
    }

    void attach_pre_tx_function(void (*pre_tx_function)())
    {
        this->pre_tx_function = pre_tx_function;
    }

    void attach_post_tx_function(void (*post_tx_function)())
    {
        this->post_tx_function = post_tx_function;
    }

    void transmit_wspr_message_on_20m()
    {
        Serial.println("Will transmit WSPR message on 20m band next");
        this->transmit_wspr_message(&OnboardFilterHardware::enable_20m_low_pass_filter,
                                    this->filter_hardware,
                                    this->tx_parameters_controller->get_tx_params_20m());
    }

    void transmit_wspr_message_on_15m()
    {
        Serial.println("Will transmit WSPR message on 15m band next");
        this->transmit_wspr_message(&OnboardFilterHardware::enable_15m_low_pass_filter,
                                    this->filter_hardware,
                                    this->tx_parameters_controller->get_tx_params_15m());
    }

    void transmit_wspr_message_on_10m()
    {
        Serial.println("Will transmit WSPR message on 10m band next");
        this->transmit_wspr_message(&OnboardFilterHardware::enable_10m_low_pass_filter,
                                    this->filter_hardware,
                                    this->tx_parameters_controller->get_tx_params_10m());
    }

  private:
    TxHardware *tx_hardware;
    OnboardFilterHardware *filter_hardware;
    WSPRTxSync *tx_sync;
    TxParametersController *tx_parameters_controller;
    uint8_t wspr_message[WSPR_SYMBOL_COUNT];

    void (*pre_tx_function)() = nullptr;
    void (*post_tx_function)() = nullptr;

    void transmit_wspr_message(void (OnboardFilterHardware::*filter_enable_function)(),
                               OnboardFilterHardware *filter_object, const TxParameters &tx_params)
    {
        this->wait_for_next_tx_period();

        if (this->pre_tx_function)
        {
            this->pre_tx_function();
        }

        (filter_object->*filter_enable_function)();
        Serial.print("Transmitting WSPR message on ");
        Serial.print(tx_params.frequency / 1000000);
        Serial.println(" MHz...");
        this->tx_hardware->transmit_wspr_message(tx_params, this->wspr_message);
        Serial.println("WSPR message transmitted");

        if (this->post_tx_function)
        {
            this->post_tx_function();
        }
    }

    void wait_for_next_tx_period()
    {
        uint32_t milliseconds_to_next_tx = this->tx_sync->get_milliseconds_to_next_tx_period();
        uint32_t target_time = millis() + milliseconds_to_next_tx;
        while (millis() < target_time)
        {
#ifdef BOARD_FAMILY_ESPRESSIF
            yield();
#endif
        }
    }
};

#endif // TXCONTROLLER_H
