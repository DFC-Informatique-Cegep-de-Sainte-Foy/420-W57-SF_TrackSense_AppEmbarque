#pragma once
#include <Arduino.h>
#include "TSProperties.h"
class IHMC5883L
{
public:
    virtual void tick() = 0;
};
