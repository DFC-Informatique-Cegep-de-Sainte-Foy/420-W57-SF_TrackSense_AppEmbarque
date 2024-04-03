#pragma once
#include <Arduino.h>
#include "TSProperties.h"
class IGY87
{
public:
    virtual void tick() = 0;
};
