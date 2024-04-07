#pragma once

#include <Wire.h>
#include "Arduino.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include "Interfaces/IGY87.h"

class GY87_Adafruit : public IGY87
{
private:
    TSProperties *_TSProperties;
    Adafruit_MPU6050 *_mpu;
    Adafruit_HMC5883_Unified *_compass;
    Adafruit_Sensor *sensor_temp, *sensor_accel, *sensor_gyro;
    sensor_t *sensor_compass;
    sensors_event_t *evt_acce, *evt_gyro, *evt_tem, *evt_compass;

    // Compass offsets et scales
    float _offset[3] = {0., 0., 0.};
    float _scale[3] = {1., 1., 1.};
    int _vCalibrated[3];
    float Xoffset = 0, Yoffset = 0;
    float Kx = 0, Ky = 0;
    float _magneticDeclinationDegrees = 0;

    //
    float GyroMeasError = PI * (60.0f / 180.0f);
    float beta = sqrt(3.0f / 4.0f) * GyroMeasError; // compute beta
    float GyroMeasDrift = PI * (1.0f / 180.0f);     // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
    float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift; // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
    float deltat = 0.0f;                            // integration interval for both filter schemes
    int lastUpdate, firstUpdate, bootTime, Now;     // used to calculate integration interval
    int delt_t;                                     // used to control display output rate
    int count;                                      // used to control display output rate
    // Euler angles
    float pitch, yaw, roll;
    float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}; // vector to hold quaternion

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

    void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
    void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
    void quaternionToEuler();
};
