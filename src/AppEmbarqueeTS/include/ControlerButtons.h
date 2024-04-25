#pragma once
#include <Arduino.h>
#include "Configurations.h"
#include "TSProperties.h"
#include "Interfaces/IButton.h"
#include "Modules/ButtonTactile.h"
#include "Modules/ButtonTouch.h"
#include <UUID.h>

class ControlerButtons
{
private:
    TSProperties *_TSProperties;
    IButton *_button1;
    IButton *_button2;

    UUID *_guidGenerator;

    // bool _isPressedButton1;
    // bool _isPressedButton2;

    long _lastDateChangementStateButtons;
    int _finalStateButton1;
    int _finalStateButton2;

    void changePageUp();
    void changePageDown();
    void startRide();
    void finishRide();
    void pauseRide();
    void restartRide();
    void makeNoiseBuzzer();
    void goHome();

public:
    ControlerButtons(TSProperties *TSProperties);
    ~ControlerButtons();

    void tick();
    void resetLastDateChangementStateButtons();
};
