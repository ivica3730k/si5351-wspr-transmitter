#ifndef TXPARAMETERSCONTROLLERLITTLEFS_H
#define TXPARAMETERSCONTROLLERLITTLEFS_H

#include "TxParametersController.h"

#include <Arduino.h>
#include <LittleFS.h>

// ──────────────── File Path Constants ────────────────
#define FILE_CALLSIGN "call"
#define FILE_LOCATOR "loc"

// Band 20m
#define FILE_20_FREQ "20f"
#define FILE_20_CORR "20c"
#define FILE_20_DRIVE "20d"
#define FILE_20_POWER "20p"

// Band 15m
#define FILE_15_FREQ "15f"
#define FILE_15_CORR "15c"
#define FILE_15_DRIVE "15d"
#define FILE_15_POWER "15p"

// Band 10m
#define FILE_10_FREQ "10f"
#define FILE_10_CORR "10c"
#define FILE_10_DRIVE "10d"
#define FILE_10_POWER "10p"

class TxParametersControllerLittleFs : public TxParametersController
{
  public:
    void begin()
    {
        LittleFS.begin();
    }

    // ──────────────── WIPE STORAGE ────────────────
    void wipe_all_storage()
    {
        const char *files[] = {FILE_CALLSIGN, FILE_LOCATOR,  FILE_20_FREQ, FILE_20_CORR,
                               FILE_20_DRIVE, FILE_20_POWER, FILE_15_FREQ, FILE_15_CORR,
                               FILE_15_DRIVE, FILE_15_POWER, FILE_10_FREQ, FILE_10_CORR,
                               FILE_10_DRIVE, FILE_10_POWER};

        for (const char *path : files)
        {
            if (LittleFS.exists(path))
            {
                debug_log("TxParametersControllerLittleFs: Deleting file from LittleFS: " + String(path));
                LittleFS.remove(path);
            }
        }
    }

    // ──────────────── CALLSIGN & LOCATOR ────────────────
    String get_callsign() override
    {
        return read_string(FILE_CALLSIGN, "AAAAA/A");
    }

    void set_callsign(const String &callsign) override
    {
        write_string(FILE_CALLSIGN, callsign);
    }

    String get_locator() override
    {
        return read_string(FILE_LOCATOR, "IO91");
    }

    void set_locator(const String &locator) override
    {
        write_string(FILE_LOCATOR, locator);
    }

    // ──────────────── 20m BAND ────────────────
    TxParameters get_tx_params_20m() override
    {
        return TxParameters(
            read_uint64(FILE_20_FREQ, 14095600),
            read_int32(FILE_20_CORR, 0),
            static_cast<TxParameters::DriveStrength>(
                read_uint8(FILE_20_DRIVE,
                           static_cast<uint8_t>(TxParameters::DriveStrength::MEDIUM_POWER))),
            read_uint8(FILE_20_POWER, 20));
    }

    void set_20m_tx_frequency(uint64_t frequency) override
    {
        write_uint64(FILE_20_FREQ, frequency);
    }

    void set_20m_tx_correction(int32_t correction) override
    {
        write_int32(FILE_20_CORR, correction);
    }

    void set_20m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_uint8(FILE_20_DRIVE, static_cast<uint8_t>(strength));
    }

    void set_20m_tx_power_dbm(uint8_t power_dbm) override
    {
        write_uint8(FILE_20_POWER, power_dbm);
    }

    // ──────────────── 15m BAND ────────────────
    TxParameters get_tx_params_15m() override
    {
        return TxParameters(
            read_uint64(FILE_15_FREQ, 21095600),
            read_int32(FILE_15_CORR, 0),
            static_cast<TxParameters::DriveStrength>(
                read_uint8(FILE_15_DRIVE,
                           static_cast<uint8_t>(TxParameters::DriveStrength::MEDIUM_POWER))),
            read_uint8(FILE_15_POWER, 20));
    }

    void set_15m_tx_frequency(uint64_t frequency) override
    {
        write_uint64(FILE_15_FREQ, frequency);
    }

    void set_15m_tx_correction(int32_t correction) override
    {
        write_int32(FILE_15_CORR, correction);
    }

    void set_15m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_uint8(FILE_15_DRIVE, static_cast<uint8_t>(strength));
    }

    void set_15m_tx_power_dbm(uint8_t power_dbm) override
    {
        write_uint8(FILE_15_POWER, power_dbm);
    }

    // ──────────────── 10m BAND ────────────────
    TxParameters get_tx_params_10m() override
    {
        return TxParameters(
            read_uint64(FILE_10_FREQ, 28124600),
            read_int32(FILE_10_CORR, 0),
            static_cast<TxParameters::DriveStrength>(
                read_uint8(FILE_10_DRIVE,
                           static_cast<uint8_t>(TxParameters::DriveStrength::MEDIUM_POWER))),
            read_uint8(FILE_10_POWER, 20));
    }

    void set_10m_tx_frequency(uint64_t frequency) override
    {
        write_uint64(FILE_10_FREQ, frequency);
    }

    void set_10m_tx_correction(int32_t correction) override
    {
        write_int32(FILE_10_CORR, correction);
    }

    void set_10m_tx_drive_strength(TxParameters::DriveStrength strength) override
    {
        write_uint8(FILE_10_DRIVE, static_cast<uint8_t>(strength));
    }

    void set_10m_tx_power_dbm(uint8_t power_dbm) override
    {
        write_uint8(FILE_10_POWER, power_dbm);
    }

  private:
    // ──────────────── DEBUG HELPER ────────────────
    void debug_log(const String &msg)
    {
#ifdef DEBUG_TX_PARAMETERS_CONTROLLER_LITTLEFS
        Serial.println(msg);
#else
        (void)msg;
#endif
    }

    // ──────────────── PRIVATE I/O HELPERS ────────────────

    // --- String ---
    void write_string(const char *path, const String &val)
    {
        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.print(val);
            file.close();
        }
    }

    String read_string(const char *path, const char *default_val)
    {
        if (!LittleFS.exists(path))
            return String(default_val);

        File file = LittleFS.open(path, "r");
        if (file)
        {
            String value = file.readStringUntil('\n');
            file.close();
            value.trim();

            debug_log("TxParametersControllerLittleFs: Successfully read string from LittleFS for file " +
                      String(path) + ": " + value);

            return value;
        }

        debug_log("TxParametersControllerLittleFs: Failed to read string from LittleFS for file " +
                  String(path) + ", returning default value of " + String(default_val));

        return String(default_val);
    }

    // --- uint8_t ---
    void write_uint8(const char *path, uint8_t val)
    {   
        if (read_uint8(path, val) == val) {
            debug_log("TxParametersControllerLittleFs: uint8_t value is unchanged, not writing to LittleFS for file " + String(path) + ".");
            return;
        }


        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.write(val);
            file.close();
        }
    }

    uint8_t read_uint8(const char *path, uint8_t default_val)
    {
        if (!LittleFS.exists(path))
            return default_val;

        File file = LittleFS.open(path, "r");
        if (file)
        {
            uint8_t val;
            if (file.read(&val, 1) == 1)
            {
                file.close();

                debug_log("TxParametersControllerLittleFs: Successfully read uint8_t from LittleFS for file " +
                          String(path) + ": " + String(val));

                return val;
            }
            file.close();
        }

        debug_log("TxParametersControllerLittleFs: Failed to read uint8_t from LittleFS for file " +
                  String(path) + ", returning default value of " + String(default_val));

        return default_val;
    }

    // --- int32_t ---
    void write_int32(const char *path, int32_t val)
    {
        if (read_int32(path, val) == val) {
            debug_log("TxParametersControllerLittleFs: int32_t value is unchanged, not writing to LittleFS for file " + String(path) + ".");
            return;
        }

        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.write((uint8_t *)&val, sizeof(val));
            file.close();
        }
    }

    int32_t read_int32(const char *path, int32_t default_val)
    {
        if (!LittleFS.exists(path))
            return default_val;

        File file = LittleFS.open(path, "r");
        if (file)
        {
            int32_t val;
            if (file.read((uint8_t *)&val, sizeof(val)) == sizeof(val))
            {
                file.close();

                debug_log("TxParametersControllerLittleFs: Successfully read int32_t from LittleFS for file " +
                          String(path) + ": " + String(val));

                return val;
            }
            file.close();
        }

        debug_log("TxParametersControllerLittleFs: Failed to read int32_t from LittleFS for file " +
                  String(path) + ", returning default value of " + String(default_val));

        return default_val;
    }

    // --- uint64_t ---
    void write_uint64(const char *path, uint64_t val)
    {
        if (read_uint64(path, val) == val) {
            debug_log("TxParametersControllerLittleFs: uint64_t value is unchanged, not writing to LittleFS for file " + String(path) + ".");
            return;
        }

        File file = LittleFS.open(path, "w");
        if (file)
        {
            file.write((uint8_t *)&val, sizeof(val));
            file.close();
        }
    }

    uint64_t read_uint64(const char *path, uint64_t default_val)
    {
        if (!LittleFS.exists(path))
            return default_val;

        File file = LittleFS.open(path, "r");
        if (file)
        {
            uint64_t val;
            if (file.read((uint8_t *)&val, sizeof(val)) == sizeof(val))
            {
                file.close();

                debug_log("TxParametersControllerLittleFs: Successfully read uint64_t from LittleFS for file " +
                          String(path) + ": " + String(val));

                return val;
            }
            file.close();
        }

        debug_log("TxParametersControllerLittleFs: Failed to read uint64_t from LittleFS for file " +
                  String(path) + ", returning default value of " + String(default_val));

        return default_val;
    }
};

#endif // TXPARAMETERSCONTROLLERLITTLEFS_H
