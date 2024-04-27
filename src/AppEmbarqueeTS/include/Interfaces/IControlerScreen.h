#pragma once
#include "TSProperties.h"
#include "Configurations.h"

class IControlerScreen
{
public:
    virtual void tick() = 0;
    virtual void drawOnScreen() = 0;

private:
    /*
        Draw Pages
    */
    virtual void drawInitTSPage() = 0;
    virtual void drawHomePage() = 0;
    virtual void drawCompassPage() = 0;
    virtual void drawRideDirectionPage() = 0;
    virtual void drawRidePage() = 0;
    virtual void drawGlobalStatisticsPage() = 0;
    virtual void drawGoHomePage() = 0;
    virtual void drawRideStatisticsPage() = 0;
    virtual void drawEndingRidePage() = 0;
    virtual void drawErrorPage() = 0;

    // API
};
