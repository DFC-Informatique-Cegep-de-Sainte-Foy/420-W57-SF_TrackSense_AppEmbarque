#pragma once
#include <Arduino.h>
#include "Interfaces/IBattery.h"
#include "Configurations.h"
#include "TSProperties.h"

class Battery : public IBattery
{
private:
    TSProperties *_TSProperties;
    int _batteryPercentage; // = 0;
    float _batteryVoltage; // = 0;
    const int _voltageReads; // = 15; // number of voltage readings before choosing a median, keep this number odd
    int vref; // = 1100;       // ADC reference voltage, change this to 1100 if using ESP32 ADC

    void readBatteryVoltage();
    void readBatteryPercentage();

    float map_battery_voltage_to_percentage(float voltage, float min_voltage, float max_voltage, float min_percentage, float max_percentage);
    void sortData(float data[], int size);

public:
    Battery(TSProperties *TSProperties);
    ~Battery();

    void tick() override;
};