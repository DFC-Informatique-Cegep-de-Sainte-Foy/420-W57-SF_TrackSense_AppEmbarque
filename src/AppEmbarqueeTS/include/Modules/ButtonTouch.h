#pragma once
#include <Arduino.h>
#include "TSProperties.h"
#include "Interfaces/IButton.h"
#include "Configurations.h"
#include "CST816S.h"
class ButtonTouch : public IButton
{
private:
    TSProperties *_TSProperties;
    CST816S *_touch;
    long _lastDateChange;
    uint16_t _durationDebounce;
    int _Xt, _Yt;

public:
    ButtonTouch(TSProperties *);
    ~ButtonTouch();
    String getTouchGesture() override;
    int getFinalState() override;
    std::pair<int, int> getTouchPosition() override;
};
