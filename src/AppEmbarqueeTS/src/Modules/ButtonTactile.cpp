#include "Modules/ButtonTactile.h"

ButtonTactile::ButtonTactile(const uint8_t pinButton, TSProperties *TSProperties) : _pinButton(pinButton),
                                                                                    _lastStateButton(LOW),
                                                                                    _lastDateChange(0),
                                                                                    _lastStableStateButton(LOW),
                                                                                    _durationDebounce(25),
                                                                                    _buttonState(LOW),
                                                                                    _TSProperties(TSProperties)
{
    pinMode(this->_pinButton, INPUT);
}

ButtonTactile::~ButtonTactile()
{
}

int ButtonTactile::getFinalState()
{
    this->_buttonState = digitalRead(this->_pinButton);
    long actualTime = millis();
    int finalState = 0; // 0 == not pressed    // 1 == short press    // 2 == long press    // 3 == double short press

    // 如果时间间隔大于阈限，则按键有效
    if (actualTime - this->_lastDateChange > this->_durationDebounce)
    {
        // ajout pour test son bouton
        // 如果按键是从low到high的状态
        if (this->_lastStableStateButton == LOW && this->_buttonState == HIGH)
        {
            // tone(PIN_BUZZER, 450, 50);
            this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
        }

        if (this->_lastStableStateButton == HIGH && this->_buttonState == HIGH && actualTime - this->_lastDateChange >= BUTTON_LONG_PRESS_DURATION_MS)
        {
            // tone(PIN_BUZZER, 550, 100);
            this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
            finalState = 2; // 2 == long press
        }

        if (this->_lastStableStateButton == HIGH && this->_buttonState == LOW && actualTime - this->_lastDateChange < BUTTON_LONG_PRESS_DURATION_MS)
        {
            finalState = 1; // 1 == short press
        }

        /* Manque gestion des double pressions longues et courtes ...  */

#if DEBUG_TS_BUTTONS_HARDCORE == true
        if (this->_pinButton == 33)
        {
            Serial.println("================== START DEBUG BUTTON ===================");
            Serial.print("actualTime = ");
            Serial.println(actualTime);
            Serial.print("_lastDateChange = ");
            Serial.println(this->_lastDateChange);
            Serial.print("actualTime - _lastDateChange = ");
            Serial.println(actualTime - this->_lastDateChange);
            Serial.print("ButtonState = ");
            Serial.println(this->_buttonState);
            Serial.print("lastStableStateButton = ");
            Serial.println(this->_lastStableStateButton);
            Serial.print("finalState = ");
            Serial.println(finalState);
            Serial.println("=================== END DEBUG BUTTON ==================");
        }
#endif

        this->_lastStableStateButton = this->_buttonState;
    }

    if (this->_buttonState != this->_lastStateButton)
    {
        this->_lastDateChange = actualTime;
        this->_lastStateButton = this->_buttonState;
    }

    return finalState;
}

// bool ButtonTactile::getIsPressedButton()
// {
//     return this->_buttonState == HIGH;
// }
