#pragma once
#include "TSProperties.h"
#include "Trajet.h"
class ISDCard
{
public:
    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void SaveTrajet(String p_path, String p_jsonStr) = 0;
    virtual Trajet ReadTrajet(String p_path, String p_fileName) = 0;
};
