#ifndef TXCONTROLLER_H
#define TXCONTROLLER_H

#include "OnboardFilterHardware.h"
#include "time_sync/WSPRTxSync.h"
#include "tx_hardware/TxHardware.h"
#include "tx_parameters_controller/TxParametersController.h"

#include <Arduino.h>
#include <JTEncode.h>

class TxController
{
  public:
    using CallbackFunction = void (*)();

    TxController() {};

    ~TxController()
    {
        if (jt_encoder)
            delete jt_encoder;
    }

    void begin(TxHardware *tx_hardware, OnboardFilterHardware *filter_hardware, WSPRTxSync *tx_sync,
               TxParametersController *tx_parameters_controller,
               CallbackFunction amp_enable_function, CallbackFunction amp_disable_function)
    {
        this->tx_hardware = tx_hardware;
        this->filter_hardware = filter_hardware;
        this->tx_sync = tx_sync;
        this->tx_parameters_controller = tx_parameters_controller;

        this->amp_enable_function = amp_enable_function;
        this->amp_disable_function = amp_disable_function;

        this->jt_encoder = new JTEncode();
        this->keep_warm();
    }

    void transmit_20m_test_tone()
    {
        TxParameters tx_params = this->tx_parameters_controller->get_tx_params_20m();
        Serial.println("TxController: Starting 20m test tone...");

        this->pre_transmit(&OnboardFilterHardware::enable_20m_low_pass_filter);
        this->tx_hardware->output_constant_tone(tx_params);
    }

    void transmit_15m_test_tone()
    {
        TxParameters tx_params = this->tx_parameters_controller->get_tx_params_15m();
        Serial.println("TxController: Starting 15m test tone...");

        this->pre_transmit(&OnboardFilterHardware::enable_15m_low_pass_filter);
        this->tx_hardware->output_constant_tone(tx_params);
    }

    void transmit_10m_test_tone()
    {
        TxParameters tx_params = this->tx_parameters_controller->get_tx_params_10m();
        Serial.println("TxController: Starting 10m test tone...");

        this->pre_transmit(&OnboardFilterHardware::enable_10m_low_pass_filter);
        this->tx_hardware->output_constant_tone(tx_params);
    }

    void stop_all_transmissions()
    {
        Serial.println("TxController: Stopping transmission...");
        this->post_transmit();
    }


    void transmit_wspr_message_on_20m()
    {
        Serial.println("TxController: Scheduled WSPR on 20m");
        this->transmit_wspr_message(&OnboardFilterHardware::enable_20m_low_pass_filter,
                                    this->tx_parameters_controller->get_tx_params_20m());
    }

    void transmit_wspr_message_on_15m()
    {
        Serial.println("TxController: Scheduled WSPR on 15m");
        this->transmit_wspr_message(&OnboardFilterHardware::enable_15m_low_pass_filter,
                                    this->tx_parameters_controller->get_tx_params_15m());
    }

    void transmit_wspr_message_on_10m()
    {
        Serial.println("TxController: Scheduled WSPR on 10m");
        this->transmit_wspr_message(&OnboardFilterHardware::enable_10m_low_pass_filter,
                                    this->tx_parameters_controller->get_tx_params_10m());
    }

  private:
    TxHardware *tx_hardware = nullptr;
    OnboardFilterHardware *filter_hardware = nullptr;
    WSPRTxSync *tx_sync = nullptr;
    TxParametersController *tx_parameters_controller = nullptr;
    JTEncode *jt_encoder = nullptr;
    uint8_t wspr_message_buffer[WSPR_SYMBOL_COUNT];
    CallbackFunction amp_enable_function = nullptr;
    CallbackFunction amp_disable_function = nullptr;

    // ──────────────── REUSABLE TX LOGIC ────────────────

    // Prepares hardware: Stops warm-up -> Sets Filter -> Enables Amp
    void pre_transmit(void (OnboardFilterHardware::*filter_enable_function)())
    {
        this->disable_keep_warm();
        (this->filter_hardware->*filter_enable_function)();

        if (this->amp_enable_function)
        {
            Serial.println("TxController: Enabling Amp...");
            this->amp_enable_function();
        }
    }

    // Safely shuts down: Disables Amp -> Disables Output -> Resumes Warm-up
    void post_transmit()
    {
        if (this->amp_disable_function)
        {
            Serial.println("TxController: Disabling Amp...");
            this->amp_disable_function();
        }

        this->tx_hardware->disable_output();
        this->keep_warm();
    }

    void transmit_wspr_message(void (OnboardFilterHardware::*filter_enable_function)(),
                               const TxParameters &tx_params)
    {
        // 1. Encode
        Serial.println("TxController: Encoding WSPR message...");
        this->jt_encoder->wspr_encode(this->tx_parameters_controller->get_callsign().c_str(),
                                      this->tx_parameters_controller->get_locator().c_str(),
                                      static_cast<int8_t>(tx_params.power_dbm),
                                      this->wspr_message_buffer);

        // 2. Wait
        this->keep_warm();
        this->wait_for_next_tx_period();

        // 3. Setup
        this->pre_transmit(filter_enable_function);

        // 4. Transmit
        Serial.print("TxController: Transmitting on ");
        Serial.print(tx_params.frequency / 1000000.0);
        Serial.println(" MHz...");

        this->tx_hardware->transmit_wspr_message(tx_params, this->wspr_message_buffer);

        // 5. Cleanup
        this->post_transmit();
    }

    void keep_warm()
    {
        if (this->amp_disable_function)
            this->amp_disable_function();

        this->filter_hardware->disable_all_filters();
        this->filter_hardware->enable_sub_megahertz_dump();

        this->tx_hardware->output_constant_tone(
            TxParameters(100000, 0, TxParameters::DriveStrength::MEDIUM_POWER, 0));
    }

    void disable_keep_warm()
    {
        this->filter_hardware->disable_sub_megahertz_dump();
        this->tx_hardware->disable_output();
    }

    void wait_for_next_tx_period()
    {
        uint32_t milliseconds_to_next_tx = this->tx_sync->get_milliseconds_to_next_tx_period();

        Serial.print("TxController: Waiting ");
        Serial.print(milliseconds_to_next_tx / 1000);
        Serial.println("s for WSPR slot...");

        uint32_t target_time = millis() + milliseconds_to_next_tx;
        while (millis() < target_time)
        {
#if defined(ESP8266) || defined(ESP32)
            yield();
#endif
        }
    }
};

#endif // TXCONTROLLER_H