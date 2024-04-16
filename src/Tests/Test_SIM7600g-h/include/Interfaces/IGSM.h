#pragma once
#include "TSProperties.h"



class IGSM
{
public:
    virtual void init() = 0;
    virtual void tick() = 0;
    virtual bool isInitialized() = 0;
};
