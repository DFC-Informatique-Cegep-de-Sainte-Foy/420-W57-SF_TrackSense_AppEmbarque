#pragma once

#include <Wire.h>
#include "Arduino.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>

class GY87_Adafruit
{
private:
    Adafruit_MPU6050 *mpu;
    Adafruit_HMC5883_Unified *compass;
    Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;
    sensors_event_t *evt_acce, *evt_gyro, *evt_tem, *evt_compass;
    // float *evt_acce, *evt_gyro, *evt_tem, *evt_compass;

public:
    GY87_Adafruit();
    ~GY87_Adafruit();
    void _read();

    void displayCompassSensorDetails(void);
    void printConfiguration(void);
    void normaliseMPU(float *gyro_X, float *gyro_Y, float *gyro_Z);
};
