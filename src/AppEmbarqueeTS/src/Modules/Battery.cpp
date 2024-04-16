#include "Modules/Battery.h"

Battery::Battery(TSProperties *TSProperties) : _TSProperties(TSProperties),
                                               _batteryPercentage(0),
                                               _batteryVoltage(0),
                                               _voltageReads(BATTERY_NB_READS),
                                               vref(BATTERY_VREF)
{
    pinMode(PIN_BATTERY, INPUT);
}

Battery::~Battery()
{
    ;
}

void Battery::tick()
{
    this->readBatteryVoltage();
    this->readBatteryPercentage();

#if DEBUG_TS_BATTERY == true
    Serial.print("Battery Voltage : ");
    Serial.println(_batteryVoltage); // 3.0v - 4.2v
    Serial.print("Battery Percentage : ");
    Serial.print(_batteryPercentage);
    Serial.println("%"); // 0-100%
#endif

    this->_TSProperties->PropertiesBattery.BatteryLevelPourcentage = this->_batteryPercentage;
}

void Battery::readBatteryPercentage()
{
    this->_batteryPercentage = map_battery_voltage_to_percentage(this->_batteryVoltage, BATTERY_VOLTAGE_MIN, BATTERY_VOLTAGE_MAX, BATTERY_PERCENTAGE_MIN, BATTERY_PERCENTAGE_MAX); // get battery voltage as a percentage 0-100%

    if (this->_batteryPercentage < BATTERY_PERCENTAGE_MIN)
    {
        this->_batteryPercentage = BATTERY_PERCENTAGE_MIN;
    }

    if (this->_batteryPercentage > BATTERY_PERCENTAGE_MAX)
    {
        this->_batteryPercentage = BATTERY_PERCENTAGE_MAX;
    }
}

void Battery::readBatteryVoltage()
{
    float voltageBuffer[_voltageReads];
    uint32_t Read_buffer = 0;

    for (int x = 0; x < _voltageReads; x++)
    {
        for (int i = 0; i < _voltageReads; i++)
        {
            voltageBuffer[i] = (uint32_t)analogRead(PIN_BATTERY);
        }
        this->sortData(voltageBuffer, this->_voltageReads);
        Read_buffer += (voltageBuffer[(this->_voltageReads - 1) / 2]);
    }

    this->_batteryVoltage = ((float)(Read_buffer / this->_voltageReads) / 4096.0) * 2.0 * 3.3 * (this->vref / 1000.0);
}

float Battery::map_battery_voltage_to_percentage(float voltage, float min_voltage, float max_voltage, float min_percentage, float max_percentage)
{
    return (voltage - min_voltage) * (max_percentage - min_percentage) / (max_voltage - min_voltage) + min_percentage;
}

void Battery::sortData(float data[], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (data[j] > data[j + 1])
            {
                float temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}