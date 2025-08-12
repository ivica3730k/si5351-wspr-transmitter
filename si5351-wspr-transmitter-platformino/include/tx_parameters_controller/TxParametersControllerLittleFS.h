#ifndef TXPARAMETERSCONTROLLERLITTLEFS_H
#define TXPARAMETERSCONTROLLERLITTLEFS_H

#include "tx_parameters_controller/TxParametersController.h"

#include <Arduino.h>
#include <LittleFS.h>

// ──────────────── File Path Constants ────────────────
#define FILE_CALLSIGN "call"
#define FILE_LOCATOR "loc"
#define FILE_TX_POWER "dbm"

// Band 20m
#define FILE_20_FREQ "20f"
#define FILE_20_CORR "20c"
#define FILE_20_DRIVE "20d"
#define FILE_20_ENABLE "20e"

// Band 15m
#define FILE_15_FREQ "15f"
#define FILE_15_CORR "15c"
#define FILE_15_DRIVE "15d"
#define FILE_15_ENABLE "15e"

// Band 10m
#define FILE_10_FREQ "10f"
#define FILE_10_CORR "10c"
#define FILE_10_DRIVE "10d"
#define FILE_10_ENABLE "10e"

class TxParametersControllerLittleFs : public TxParametersController
{
  public:
    // Not in abstract class (init helper), keep at top for convenience
    void begin()
    {
        if (!LittleFS.begin())
        {
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.println("Failed to mount LittleFS");
#endif
            return;
        }
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
        Serial.println("TxParametersController initialized with LittleFS");
#endif
    }

    // ──────────────── EXACT ORDER AS ABSTRACT CLASS ────────────────
    // 1) Callsign
    String get_callsign() override
    {
        return read_string(FILE_CALLSIGN, "AAAAA/A");
    }
    void set_callsign(const String &callsign) override
    {
        write_string(FILE_CALLSIGN, callsign);
    }

    // 2) Locator
    String get_locator() override
    {
        return read_string(FILE_LOCATOR, "IO91");
    }
    void set_locator(const String &locator) override
    {
        write_string(FILE_LOCATOR, locator);
    }

    // 3) TX power
    int get_tx_power() override
    {
        return read_int(FILE_TX_POWER, 20);
    }
    void set_tx_power(int power) override
    {
        write_param(FILE_TX_POWER, power);
    }

    // 4) 20m
    TxParameters get_tx_params_20m() override
    {
        return TxParameters(
            read_uint64(FILE_20_FREQ, 14095600), get_20m_correction(),
            static_cast<TxParameters::DriveStrength>(
                read_int(FILE_20_DRIVE, static_cast<int>(TxParameters::DriveStrength::LOW_POWER))));
    }
    void set_20m_tx_frequency(uint64_t frequency) override
    {
        write_param(FILE_20_FREQ, frequency);
    }
    void set_20m_tx_correction(int32_t correction) override
    {
        correction_20m = correction;
        correction_20m_loaded = true;
    }
    void set_20m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_param(FILE_20_DRIVE, static_cast<uint8_t>(strength));
    }
    void enable_20m_tx() override
    {
        write_param(FILE_20_ENABLE, 1);
    }
    void disable_20m_tx() override
    {
        write_param(FILE_20_ENABLE, 0);
    }
    bool is_20m_tx_enabled() override
    {
        return read_int(FILE_20_ENABLE, 0) != 0;
    }

    // 5) 15m
    TxParameters get_tx_params_15m() override
    {
        return TxParameters(
            read_uint64(FILE_15_FREQ, 21095600), get_15m_correction(),
            static_cast<TxParameters::DriveStrength>(
                read_int(FILE_15_DRIVE, static_cast<int>(TxParameters::DriveStrength::LOW_POWER))));
    }
    void set_15m_tx_frequency(uint64_t frequency) override
    {
        write_param(FILE_15_FREQ, frequency);
    }
    void set_15m_tx_correction(int32_t correction) override
    {
        correction_15m = correction;
        correction_15m_loaded = true;
    }
    void set_15m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_param(FILE_15_DRIVE, static_cast<uint8_t>(strength));
    }
    void enable_15m_tx() override
    {
        write_param(FILE_15_ENABLE, 1);
    }
    void disable_15m_tx() override
    {
        write_param(FILE_15_ENABLE, 0);
    }
    bool is_15m_tx_enabled() override
    {
        return read_int(FILE_15_ENABLE, 0) != 0;
    }

    // 6) 10m
    TxParameters get_tx_params_10m() override
    {
        return TxParameters(
            read_uint64(FILE_10_FREQ, 28124600), get_10m_correction(),
            static_cast<TxParameters::DriveStrength>(
                read_int(FILE_10_DRIVE, static_cast<int>(TxParameters::DriveStrength::LOW_POWER))));
    }
    void set_10m_tx_frequency(uint64_t frequency) override
    {
        write_param(FILE_10_FREQ, frequency);
    }
    void set_10m_tx_correction(int32_t correction) override
    {
        correction_10m = correction;
        correction_10m_loaded = true;
    }
    void set_10m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_param(FILE_10_DRIVE, static_cast<uint8_t>(strength));
    }
    void enable_10m_tx() override
    {
        write_param(FILE_10_ENABLE, 1);
    }
    void disable_10m_tx() override
    {
        write_param(FILE_10_ENABLE, 0);
    }
    bool is_10m_tx_enabled() override
    {
        return read_int(FILE_10_ENABLE, 0) != 0;
    }

    // Optional explicit saves if you want to persist cached corrections immediately
    void save_20m_tx_correction()
    {
        if (correction_20m_loaded)
            write_param(FILE_20_CORR, correction_20m);
    }
    void save_15m_tx_correction()
    {
        if (correction_15m_loaded)
            write_param(FILE_15_CORR, correction_15m);
    }
    void save_10m_tx_correction()
    {
        if (correction_10m_loaded)
            write_param(FILE_10_CORR, correction_10m);
    }

  private:
    int32_t correction_20m = 0;
    int32_t correction_15m = 0;
    int32_t correction_10m = 0;

    bool correction_20m_loaded = false;
    bool correction_15m_loaded = false;
    bool correction_10m_loaded = false;

    template <typename T> void write_param(const char *path, T value)
    {
        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.print(value);
            file.close();
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Successfully wrote to ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println(value);
#endif
        }
        else
        {
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Failed to open ");
            Serial.print(path);
            Serial.println(" for writing");
#endif
        }
    }

    void write_string(const char *path, const String &val)
    {
        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.print(val);
            file.close();
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Successfully wrote string to ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println(val);
#endif
        }
        else
        {
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Failed to open ");
            Serial.print(path);
            Serial.println(" for writing");
#endif
        }
    }

    int read_int(const char *path, int default_val)
    {
        File file = LittleFS.open(path, "r");
        if (file)
        {
            int val = file.parseInt();
            file.close();
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Read int from ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println(val);
#endif
            return val;
        }
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
        Serial.print("Failed to open ");
        Serial.print(path);
        Serial.println(", using default int");
#endif
        return default_val;
    }

    uint64_t read_uint64(const char *path, uint64_t default_val)
    {
        File file = LittleFS.open(path, "r");
        if (file)
        {
            String valStr = file.readStringUntil('\n');
            file.close();
            valStr.trim();
            uint64_t val = parse_uint64(valStr);
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Read uint64 from ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println((unsigned long)val);
#endif
            return val;
        }
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
        Serial.print("Failed to open ");
        Serial.print(path);
        Serial.println(", using default uint64");
#endif
        return default_val;
    }

    uint64_t parse_uint64(const String &s)
    {
        uint64_t result = 0;
        for (int i = 0; i < s.length(); ++i)
        {
            char c = s[i];
            if (c < '0' || c > '9')
                break;
            result = result * 10 + (c - '0');
        }
        return result;
    }

    String read_string(const char *path, const char *default_val)
    {
        File file = LittleFS.open(path, "r");
        if (file)
        {
            String value = file.readStringUntil('\n');
            file.close();
            value.trim();
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
            Serial.print("Read string from ");
            Serial.print(path);
            Serial.print(": ");
            Serial.println(value);
#endif
            return value;
        }
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLE_FS
        Serial.print("Failed to open ");
        Serial.print(path);
        Serial.println(", using default string");
#endif
        return String(default_val);
    }

    // ──────────────── Correction Handling (Cached) ────────────────
    int32_t get_20m_correction()
    {
        if (!correction_20m_loaded)
        {
            correction_20m = read_int(FILE_20_CORR, 0);
            correction_20m_loaded = true;
        }
        return correction_20m;
    }

    int32_t get_15m_correction()
    {
        if (!correction_15m_loaded)
        {
            correction_15m = read_int(FILE_15_CORR, 0);
            correction_15m_loaded = true;
        }
        return correction_15m;
    }

    int32_t get_10m_correction()
    {
        if (!correction_10m_loaded)
        {
            correction_10m = read_int(FILE_10_CORR, 0);
            correction_10m_loaded = true;
        }
        return correction_10m;
    }
};

#endif // TXPARAMETERSCONTROLLERLITTLEFS_H
