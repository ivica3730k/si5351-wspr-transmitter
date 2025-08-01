#ifndef TXPARAMETERSCONTROLLERLITTLEFS_H
#define TXPARAMETERSCONTROLLERLITTLEFS_H

#include "tx_parameters_controller/TxParametersController.h"

#include <Arduino.h>
#include <LittleFS.h>

class TxParametersControllerLittleFs : public TxParametersController
{
  public:
    TxParametersControllerLittleFs() {}
    ~TxParametersControllerLittleFs() {}

    void begin()
    {
        if (!LittleFS.begin())
        {
            Serial.println("Failed to mount LittleFS");
            return;
        }

        Serial.println("TxParametersController initialized with LittleFS");
    }

    TxParameters get_tx_params_20m() override
    {
        return read_band_params("20f", "20c", "20d", 14095600, -91200,
                                TxParameters::DriveStrength::MEDIUM_POWER);
    }

    TxParameters get_tx_params_15m() override
    {
        return read_band_params("15f", "15c", "15d", 21095600, -91200,
                                TxParameters::DriveStrength::MEDIUM_POWER);
    }

    TxParameters get_tx_params_10m() override
    {
        return read_band_params("10f", "10c", "10d", 28124600, -39250,
                                TxParameters::DriveStrength::MEDIUM_POWER);
    }

    void set_20m_tx_frequency(uint64_t frequency) override
    {
        write_param("20f", frequency);
    }
    void set_20m_tx_correction(int32_t correction) override
    {
        write_param("20c", correction);
    }
    void set_20m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_param("20d", static_cast<uint8_t>(strength));
    }

    void set_15m_tx_frequency(uint64_t frequency) override
    {
        write_param("15f", frequency);
    }
    void set_15m_tx_correction(int32_t correction) override
    {
        write_param("15c", correction);
    }
    void set_15m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_param("15d", static_cast<uint8_t>(strength));
    }

    void set_10m_tx_frequency(uint64_t frequency) override
    {
        write_param("10f", frequency);
    }
    void set_10m_tx_correction(int32_t correction) override
    {
        write_param("10c", correction);
    }
    void set_10m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_param("10d", static_cast<uint8_t>(strength));
    }

  private:
    TxParameters read_band_params(const char *freqPath, const char *corrPath, const char *drivePath,
                                  uint64_t defaultFreq, int32_t defaultCorr,
                                  TxParameters::DriveStrength defaultDrive)
    {
        TxParameters params;

        File file = LittleFS.open(freqPath, "r");
        if (file)
        {
            params.frequency = file.parseInt();
            file.close();
            Serial.print("Successfully read frequency: ");
            Serial.print(params.frequency);
            Serial.print(" from LittleFS for file:");
            Serial.println(freqPath);
        }
        else
        {
            Serial.print("Failed to open ");
            Serial.print(freqPath);
            Serial.println(" for reading, using default frequency");
            params.frequency = defaultFreq;
        }

        file = LittleFS.open(corrPath, "r");
        if (file)
        {
            params.correction = file.parseInt();
            file.close();
            Serial.print("Successfully read correction: ");
            Serial.print(params.correction);
            Serial.print(" from LittleFS for file:");
            Serial.println(corrPath);
        }
        else
        {
            Serial.print("Failed to open ");
            Serial.print(corrPath);
            Serial.println(" for reading, using default correction");
            params.correction = defaultCorr;
        }

        file = LittleFS.open(drivePath, "r");
        if (file)
        {
            params.drive_strength = static_cast<TxParameters::DriveStrength>(file.parseInt());
            file.close();
            Serial.print("Successfully read drive strength: ");
            Serial.print(static_cast<uint8_t>(params.drive_strength));
            Serial.print(" from LittleFS for file:");
            Serial.println(drivePath);
        }
        else
        {
            Serial.print("Failed to open ");
            Serial.print(drivePath);
            Serial.println(" for reading, using default drive strength");
            params.drive_strength = defaultDrive;
        }

        return params;
    }

    template <typename T> void write_param(const char *path, T value)
    {
        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.print(value);
            file.close();
        }
        else
        {
            Serial.print("Failed to open ");
            Serial.print(path);
            Serial.println(" for writing");
        }
    }
};

#endif // TXPARAMETERSCONTROLLERLITTLEFS_H
