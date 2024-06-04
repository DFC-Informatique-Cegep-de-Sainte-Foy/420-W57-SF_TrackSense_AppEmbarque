#pragma once
#include <Arduino.h>
#include "Configurations.h"
#include "TSProperties.h"
#include <Wire.h>
// #include "ControlerButtons.h"
// #include "ControlerScreen.h"

// #include "Modules/LTE.h"
// #include "Modules/GSMTiny.h"
// #include "Modules/SDCard.h"
// #include "Modules/GyroscopeMPU6050.h"
// #include "Modules/CompassHMC5883L.h"
// #include "Modules/AccelerometerMPU6050.h"
// #include "Modules/BLE.h"
// #include "Modules/ScreenGC9A01.h"
// #include "Modules/Buzzer.h"
#include "Modules/MyMPU6050.h"
#include "Modules/QMC5883L.h"
class Program
{
private:
    TSProperties *_TSProperties;
    // ILTE *_lte;
    // IGSM *_gsm;
    // ISDCard *_sdCard;
    // IGyroscope *_gyroscope;
    // ICompass *_compass;
    // IAccelerometer *_accelerometer;
    // IBLE *_ble;
    // IBuzzer *_buzzer;
    // ControlerButtons *_controlerButtons;
    // IControlerScreen *_controlerScreen;
    // rajouter 6050
    MyMPU6050 *_MPU6050;
    QMC5883L *_QMC5883L;
    // Timer
    unsigned long m_now_ms, m_last_ms = 0; // millis() timers
    unsigned long m_print_ms = 200;        // print angles every "print_ms" milliseconds
public:
    Program();
    ~Program();

    void execute();
};
