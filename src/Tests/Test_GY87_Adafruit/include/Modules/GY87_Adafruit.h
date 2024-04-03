#pragma once

#include <Wire.h>
#include "Arduino.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include "Interfaces/IHMC5883L.h"

class GY87_Adafruit : public IHMC5883L
{
private:
    TSProperties *_TSProperties;
    Adafruit_MPU6050 *mpu;
    Adafruit_HMC5883_Unified *compass;
    Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;
    sensors_event_t *evt_acce, *evt_gyro, *evt_tem, *evt_compass;
    // float *evt_acce, *evt_gyro, *evt_tem, *evt_compass;

    // Compass offsets et scales
    float _offset[3] = {0., 0., 0.};
    float _scale[3] = {1., 1., 1.};
    int _vCalibrated[3];
    float Xoffset = 0, Yoffset = 0;
    float Kx = 0, Ky = 0;
    float _magneticDeclinationDegrees = 0;

public:
    GY87_Adafruit(TSProperties *);
    ~GY87_Adafruit();
    void read();
    void displayCompassSensorDetails(void);
    void printConfiguration(void);
    void normaliseMPU(float *gyro_X, float *gyro_Y, float *gyro_Z);
    void calibrationHMC5883L(void);
    void clearCalibration();
    void setCalibrationOffsets(float x_offset, float y_offset, float z_offset);
    void setCalibrationScales(float x_scale, float y_scale, float z_scale);
    void setMagneticDeclination(int degrees, uint8_t minutes);
    void tick() override;
};
