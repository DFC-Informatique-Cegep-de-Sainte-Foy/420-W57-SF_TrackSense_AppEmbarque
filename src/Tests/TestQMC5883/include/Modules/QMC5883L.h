#pragma once
#include <Wire.h>
#include <Arduino.h>
#include <QMC5883LCompass.h>
#include "Interfaces/IQMC5883L.h"

class QMC5883L : public IQMC5883L
{
private:
    TSProperties *_TSProperties;
    QMC5883LCompass *_compass;
    int _QMCaddr;
    // data brutes
    int _xyzRaw[3] = {0, 0, 0};
    int _azimuth;
    byte _bearing;
    char monOrientation[3];
    // calibration
    double caliOffset[3] = {0., 0., 0.};
    double caliScale[3] = {1., 1., 1.};

    //
    const char _bearings[16][3] = {
        {' ', ' ', 'N'},
        {'N', 'N', 'E'},
        {' ', 'N', 'E'},
        {'E', 'N', 'E'},
        {' ', ' ', 'E'},
        {'E', 'S', 'E'},
        {' ', 'S', 'E'},
        {'S', 'S', 'E'},
        {' ', ' ', 'S'},
        {'S', 'S', 'W'},
        {' ', 'S', 'W'},
        {'W', 'S', 'W'},
        {' ', ' ', 'W'},
        {'W', 'N', 'W'},
        {' ', 'N', 'W'},
        {'N', 'N', 'W'},
    };

public:
    QMC5883L(TSProperties *, int p_QMC_addr = 0x0D);
    ~QMC5883L();
    void tick() override;
    int obtenirAzimuth();
    byte obtenirBearing();
    void obtenirDirection();
    void printData();
    void obtenirXYZ();
};
