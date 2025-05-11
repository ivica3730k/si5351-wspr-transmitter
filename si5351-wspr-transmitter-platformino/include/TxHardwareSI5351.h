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
        this->_si5351 = new Si5351(i2c_addr);

#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Initializing Si5351...");
#endif
        this->_si5351->init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    }

    ~TxHardwareSI5351()
    {
        delete this->_si5351;
    }

    void transmit_wspr_message(TxHardwareTxParameters tx_params, uint8_t *message) override
    {
#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Enabling Si5351 clock output");
#endif
        this->_si5351->output_enable(this->_clock_output, true);

        this->_configure_for_tx(tx_params);

        const uint32_t SYMBOL_PERIOD_US = 683000;
        uint32_t start_time = micros();

        for (int i = 0; i < WSPR_SYMBOL_COUNT; i++)
        {
            uint64_t tone_mhz = this->_calculate_tone_freq_mhz(tx_params.frequency, message[i]);

#ifdef DEBUG_TX_HARDWARE_SI5351
            Serial.print("Setting tone [");
            Serial.print(i);
            Serial.print("] to frequency (millihertz): ");
            Serial.println(tone_mhz);
#endif

            this->_si5351->set_freq(tone_mhz, this->_clock_output);

            uint32_t target_time = start_time + (i + 1) * SYMBOL_PERIOD_US;
            while (micros() < target_time)
            {
#ifdef BOARD_FAMILY_ESPRESSIF
                yield();
#endif
            }
        }

#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Disabling Si5351 clock output");
#endif
        this->_si5351->output_enable(this->_clock_output, false);
    }

    void disable_output()
    {
#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Disabling Si5351 output");
#endif
        this->_si5351->output_enable(this->_clock_output, false);
    }

    void output_constant_tone(TxHardwareTxParameters tx_params)
    {
        this->_configure_for_tx(tx_params);

#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Enabling Si5351 clock output for constant tone");
#endif
        this->_si5351->output_enable(this->_clock_output, true);

        uint64_t tone_mhz = tx_params.frequency * 100;

#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.print("Setting constant tone frequency (millihertz): ");
        Serial.println(tone_mhz);
#endif

        uint8_t output_code = this->_si5351->set_freq(tone_mhz, this->_clock_output);

#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.print("Si5351 output code: ");
        Serial.println(output_code);
#endif
    }

  private:
    Si5351 *_si5351;
    si5351_clock _clock_output = SI5351_CLK0;

    void _configure_for_tx(const TxHardwareTxParameters &tx_params)
    {
#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.println("Configuring Si5351 for transmission...");
        Serial.print("Applying correction: ");
        Serial.println(tx_params.correction);
#endif

        this->_si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_XO);
        this->_si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_CLKIN);

        si5351_drive power = this->_get_drive_power_from_enum(tx_params.drive_strength);

#ifdef DEBUG_TX_HARDWARE_SI5351
        Serial.print("Setting drive strength: ");
        Serial.println(power);
#endif

        this->_si5351->drive_strength(this->_clock_output, power);
    }

    /**
     * Calculate the tone frequency in millihertz (mHz).
     * This multiplies the base frequency by 100 and adds symbol offset.
     */
    uint64_t _calculate_tone_freq_mhz(uint64_t base_frequency_hz, uint8_t symbol_index)
    {
        return (base_frequency_hz * 100) + (symbol_index * TONE_SPACING);
    }

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
