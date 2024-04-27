#include "Modules/ButtonTouch.h"

ButtonTouch::ButtonTouch(TSProperties *p_tsproperty)
    : _TSProperties(p_tsproperty),
      _lastDateChange(0),
      _durationDebounce(BUTTON_DURATIONDEBOUNCE),
      _Xt(-1),
      _Yt(-1)
{
    _touch = new CST816S(SDA, SCL, PIN_CST816_RST, PIN_CST816_INT);
    _touch->begin();
}
ButtonTouch::~ButtonTouch()
{
    ;
}

String ButtonTouch::getTouchGesture()
{
    long actualTime = millis();
    int finalState = 0; // 0 == not pressed    // 1 == short press    // 2 == long press    // 3 == double short press

    // 如果时间间隔大于阈限，则按键有效
    if (actualTime - this->_lastDateChange > this->_durationDebounce)
    {
        this->_lastDateChange = actualTime;

        if (_touch->available())
        {
            // Serial.print(_touch->gesture());
            // Serial.print("\t");
            // Serial.print(_touch->data.points);
            // Serial.print("\t");
            // Serial.print(_touch->data.event);
            // Serial.print("\t");
            // Serial.print(_touch->data.x);
            // Serial.print("\t");
            // Serial.println(_touch->data.y);
            String str = _touch->gesture();
            Serial.println(str);
            if (str != "NONE")
            {
                this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
                this->_Xt = _touch->data.x;
                this->_Yt = _touch->data.y;
            }
            return str;

            // if (str == "SINGLE CLICK")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
            //     return 1;
            // }
            // else if (str == "LONG PRESS")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
            //     return 2;
            // }
            // else if (str == "SWIPE LEFT")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
            //     return -1;
            // }
            // else if (str == "SWIPE RIGHT")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;

            //     return -1;
            // }
            // else if (str == "SWIPE UP")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;

            //     return -1;
            // }
            // else if (str == "SWIPE DOWN")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;

            //     return -1;
            // }
            // else if (str == "DOUBLE CLICK")
            // {
            //     this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;

            //     return -1;
            // }
        }
    }

    return "NONE";
}

int ButtonTouch::getFinalState()
{
    return -1;
}

std::pair<int, int> ButtonTouch::getTouchPosition()
{
    return std::pair<int, int>(_Xt, _Yt);
}
