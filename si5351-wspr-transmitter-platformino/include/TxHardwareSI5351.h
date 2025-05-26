#ifndef TXHARDWARE_SI5351_H
#define TXHARDWARE_SI5351_H

#define TONE_SPACING 146 // ~1.46 Hz

#include "TxHardware.h"
#include "si5351.h"

class TxHardwareSi5351 : public TxHardware
{
  public:
    TxHardwareSi5351(uint8_t i2c_address)
    {
        this->si5351 = new Si5351(i2c_address);

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.println("Initializing Si5351...");
#endif
        this->si5351->init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    }

    ~TxHardwareSi5351()
    {
        delete this->si5351;
    }

    void transmit_wspr_message(const TxHardwareTxParameters &tx_params, uint8_t *message) override
    {
#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.println("Enabling Si5351 clock output");
#endif
        this->si5351->output_enable(this->clock_output, true);
        this->configure_for_tx(tx_params);

        const uint32_t symbol_period_us = 683000;
        uint32_t start_time = micros();

        for (int i = 0; i < WSPR_SYMBOL_COUNT; i++)
        {
            uint64_t tone_mhz = this->calculate_tone_freq_mhz(tx_params.frequency, message[i]);

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
            Serial.print("Setting tone [");
            Serial.print(i);
            Serial.print("] to frequency (millihertz): ");
            Serial.println(tone_mhz);
#endif

            this->si5351->set_freq(tone_mhz, this->clock_output);

            uint32_t target_time = start_time + (i + 1) * symbol_period_us;
            while (micros() < target_time)
            {
#ifdef BOARD_FAMILY_ESPRESSIF
                yield();
#endif
            }
        }

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.println("Disabling Si5351 clock output");
#endif
        this->disable_output();
    }

    void disable_output()
    {
#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.println("Disabling Si5351 output");
#endif
        this->si5351->output_enable(this->clock_output, false);
    }

    void output_constant_tone(const TxHardwareTxParameters &tx_params)
    {
        this->configure_for_tx(tx_params);

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.println("Enabling Si5351 clock output for constant tone");
#endif
        this->si5351->output_enable(this->clock_output, true);

        uint64_t tone_mhz = tx_params.frequency * 100;

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.print("Setting constant tone frequency (millihertz): ");
        Serial.println(tone_mhz);
#endif

        uint8_t output_code = this->si5351->set_freq(tone_mhz, this->clock_output);

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.print("Si5351 output code: ");
        Serial.println(output_code);
#endif
    }

  private:
    Si5351 *si5351;
    si5351_clock clock_output = SI5351_CLK0;

    void configure_for_tx(const TxHardwareTxParameters &tx_params)
    {
#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.println("Configuring Si5351 for transmission...");
        Serial.print("Applying correction: ");
        Serial.println(tx_params.correction);
#endif

        this->si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_XO);
        this->si5351->set_correction(tx_params.correction, SI5351_PLL_INPUT_CLKIN);

        si5351_drive drive = this->get_drive_power_from_enum(tx_params.drive_strength);

#ifdef DEBUG_TX_HARDWARE_USE_SI5351
        Serial.print("Setting drive strength: ");
        Serial.println(drive);
#endif

        this->si5351->drive_strength(this->clock_output, drive);
    }

    uint64_t calculate_tone_freq_mhz(uint64_t base_frequency_hz, uint8_t symbol_index)
    {
        return (base_frequency_hz * 100) + (symbol_index * TONE_SPACING);
    }

    si5351_drive get_drive_power_from_enum(TxHardwareTxParameters::DriveStrength strength)
    {
        switch (strength)
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
