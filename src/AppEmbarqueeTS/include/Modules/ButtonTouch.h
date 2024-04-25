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

public:
    ButtonTouch(TSProperties *);
    ~ButtonTouch();
    int getFinalState() override;
};
