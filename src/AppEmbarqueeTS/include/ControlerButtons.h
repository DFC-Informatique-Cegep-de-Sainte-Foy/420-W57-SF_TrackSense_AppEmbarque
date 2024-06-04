#pragma once
#include <Arduino.h>
#include "Configurations.h"
#include "TSProperties.h"
#include "Interfaces/IButton.h"
#include "Modules/ButtonTactile.h"
#include "Modules/ButtonTouch.h"
#include <UUID.h>
#include "Modules/SDCard.h"

class ControlerButtons
{
private:
    TSProperties *_TSProperties;
    IButton *_button1;
    IButton *_button2;
    ISDCard *_sd;

    UUID *_guidGenerator;

    // bool _isPressedButton1;
    // bool _isPressedButton2;

    long _lastDateChangementStateButtons;
    int _finalStateButton1;
    int _finalStateButton2;
    String _finalGesture;
    std::pair<int, int> position;

    void changePageUp();
    void changePageDown();
    void startRide(int p_index);
    void finishRide();
    void pauseRide();
    void restartRide();
    void makeNoiseBuzzer();
    void goHome();

public:
    ControlerButtons(TSProperties *TSProperties, ISDCard *sdCard);
    ~ControlerButtons();

    void tick();
    void resetLastDateChangementStateButtons();
};
