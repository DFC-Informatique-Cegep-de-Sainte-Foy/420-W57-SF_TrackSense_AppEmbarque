#include "Modules/Buzzer.h"

Buzzer::Buzzer(TSProperties *TSProperties) : _TSProperties(TSProperties)
{
    pinMode(PIN_BUZZER, OUTPUT);
}

Buzzer::~Buzzer()
{
    ;
}

void Buzzer::tick()
{
    // Serial.println("2---Buzzer --> tick");

    if (this->_TSProperties->PropertiesBuzzer.IsBuzzerOn)
    {
        tone(PIN_BUZZER, 500, 100);
        this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = false;
    }
}
