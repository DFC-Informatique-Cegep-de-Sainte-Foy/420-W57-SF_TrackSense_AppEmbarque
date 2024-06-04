#pragma once
#include <Arduino.h>
#include "TSProperties.h"
class IQMC5883L
{
public:
    virtual void tick() = 0;
};
