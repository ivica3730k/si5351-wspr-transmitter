#ifndef TXHARDWARE_SI5351_H
#define TXHARDWARE_SI5351_H

#define TONE_SPACING 146 // ~1.46 Hz
#include "TxHardware.h"
#include "si5351.h"

class TxHardwareSI5351 : public TxHardware
{
public:
    TxHardwareSI5351(uint8_t i2c_addr)
    {
        _si5351 = new Si5351(i2c_addr);
        _si5351->init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    }

    ~TxHardwareSI5351()
    {
        delete _si5351;
    }

    void transmit_wspr_message(TxHardwareTxParameters tx_params, uint8_t *message) override
    {
        _si5351->output_enable(_clock_output, true);
        _si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_XO);
        _si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_CLKIN);
        _si5351->drive_strength(_clock_output, _get_drive_power_from_enum(tx_params.drive_strength));

        const uint32_t SYMBOL_PERIOD_US = 683000; // 683 ms in microseconds
        uint32_t start_time = micros();

        for (int i = 0; i < WSPR_SYMBOL_COUNT; i++)
        {
            uint64_t tone = (tx_params.frequency * 100) + (message[i] * TONE_SPACING);
            _si5351->set_freq(tone, _clock_output);

            uint32_t target_time = start_time + (i + 1) * SYMBOL_PERIOD_US;

            while (micros() < target_time)
            {
                #ifdef BOARD_FAMILY_ESPRESSIF
                yield();
                #endif
            }
        }
        _si5351->output_enable(_clock_output, false);
    }

    void disable_output()
    {
        _si5351->output_enable(_clock_output, false);
    }

    void output_constant_tone(TxHardwareTxParameters tx_params)
    {
        #ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Setting correction");
        #endif
        _si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_XO);

        #ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Setting correction CLKIN");
        #endif
        _si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_CLKIN);

        #ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Setting drive strength");
        #endif
        _si5351->drive_strength(_clock_output, _get_drive_power_from_enum(tx_params.drive_strength));

        #ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.print("Setting frequency to: ");
        Serial.println(tx_params.frequency);
        #endif
        _si5351->output_enable(_clock_output, true);
        _si5351->set_freq(tx_params.frequency * 100, _clock_output);

        #ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.print("Output code: ");
        Serial.println(output_code);
        #endif
    }

private:
    Si5351* _si5351;
    si5351_clock _clock_output = SI5351_CLK0;

    si5351_drive _get_drive_power_from_enum(TxHardwareTxParameters::DriveStrength drive_strength)
    {
        switch (drive_strength)
        {
        case TxHardwareTxParameters::DriveStrength::LOW_POWER:
            return SI5351_DRIVE_2MA;
        case TxHardwareTxParameters::DriveStrength::MEDIUM_POWER:
            return SI5351_DRIVE_4MA;
        case TxHardwareTxParameters::DriveStrength::HIGH_POWER:
            return SI5351_DRIVE_8MA;
        }
        return SI5351_DRIVE_2MA;
    }
};

#endif // TXHARDWARE_SI5351_H
