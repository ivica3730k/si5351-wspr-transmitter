#ifndef TXHARDWARESERIALCONFIGURATOR_H
#define TXHARDWARESERIALCONFIGURATOR_H

#include "tx_hardware/TxHardware.h"
#include "tx_hardware_configurator/TxHardwareConfigurator.h"
#include "tx_parameters_controller/TxParametersController.h"

class TxHardwareSerialConfigurator : public TxHardwareConfigurator
{
  public:
    void begin(TxHardware &txHardware, TxParametersController &txParametersController) override
    {
        this->tx_hardware = &txHardware;
        this->tx_parameters_controller = &txParametersController;
    }

    void set_20m_tx_frequency() override
    {
        Serial.print("Enter 20m TX frequency in Hz: ");
        uint64_t frequency;

        while (true)
        {
            if (Serial.available() > 0)
            {
                String input = Serial.readStringUntil('\n');
                input.trim();
                if (input.length() > 0)
                {
                    frequency = input.toInt();
                    if (frequency > 0)
                    {
                        this->tx_parameters_controller->set_20m_tx_frequency(frequency);
                        Serial.print("20m TX frequency set to: ");
                        Serial.println(frequency);
                        break;
                    }
                    else
                    {
                        Serial.println("Invalid frequency, please try again.");
                        Serial.print("Enter 20m TX frequency in Hz: ");
                    }
                }
            }
        }
    }

  private:
    TxHardware *tx_hardware;
    TxParametersController *tx_parameters_controller;
};

#endif // SERIALCONFIGURATOR_H