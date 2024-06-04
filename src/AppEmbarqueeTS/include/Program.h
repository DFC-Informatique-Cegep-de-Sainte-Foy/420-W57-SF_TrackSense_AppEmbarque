#pragma once
#include <Arduino.h>
#include "Configurations.h"
#include "TSProperties.h"
#include "ControlerButtons.h"
#include "ControlerScreen.h"

#include "Modules/GSMTiny.h"
// #include "Modules/MyTinyGsm.h"
#include "Modules/SDCard.h"
#include "Modules/GyroscopeMPU6050.h"
#include "Modules/CompassHMC5883L.h"
#include "Modules/AccelerometerMPU6050.h"
#include "Modules/BLE.h"
#include "Modules/ScreenGC9A01.h"
#include "Modules/Buzzer.h"
#include "Modules/Battery.h"
#include "Modules/GY87_Adafruit.h"
#include "StringQueue.h"
class Program
{
private:
    TSProperties *_TSProperties;
    StringQueue *_trajetsSD;
    IGSM *_gsm;
    ISDCard *_sdCard;
    IGyroscope *_gyroscope;
    ICompass *_compass;
    IAccelerometer *_accelerometer;
    IBLE *_ble;
    IBuzzer *_buzzer;
    IBattery *_battery;
    ControlerButtons *_controlerButtons;
    IControlerScreen *_controlerScreen;
    IGY87 *_gy87;

public:
    Program();
    ~Program();

    void executeCore1();
    void executeCore0();
};
