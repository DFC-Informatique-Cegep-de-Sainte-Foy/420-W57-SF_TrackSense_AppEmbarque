#pragma once
#include <Arduino.h>
#include "Interfaces/IMPU6050.h"
#include "Wire.h"

class MyMPU6050 : public IMPU6050
{
private:
    TSProperties *_TSProperties;
    // données brutes
    int16_t m_ax, m_ay, m_az; // acceleration
    int16_t m_gx, m_gy, m_gz; // gyroscope
    int16_t m_tmp;            // temperature

    // données mises à l'échelle en tant que vecteur
    float Axyz[3];
    float Gxyz[3];

    int m_MPU_addr;
    float m_quaternion[4] = {1.0, 0.0, 0.0, 0.0}; // sauvgarder les 4 vector pour Mahony Filter
    float m_Kp = 30.0;                            // Paramètres libres dans le schéma de filtre et de fusion Mahony
    float m_Ki = 0.0;                             // Kp pour feedback proportionnel, Ki pour intégral
    // with MPU-6050, some instability observed at Kp=100 Now set to 30.

    // These are the previously determined offsets and scale factors for accelerometer and gyro for
    // a particular example of an MPU-6050. They are not correct for other examples.
    // The IMU code will NOT work well or at all if these are not correct

    float m_A_cal[6] = {265.0, -80.0, -700.0, 0.994, 1.000, 1.014}; // 0..2 offset xyz, 3..5 scale xyz
    // the code will work, but not as accurately, for an uncalibrated accelerometer. Use this line instead:
    // float m_A_cal[6] = {0.0, 0.0, 0.0, 1.000, 1.000, 1.000}; // 0..2 offset xyz, 3..5 scale xyz

    float m_G_off[3] = {-499.5, -17.7, -82.0};   // raw offsets, determined for gyro at rest
#define gscale ((250. / 32768.0) * (PI / 180.0)) // gyro default 250 LSB per d/s -> rad/s

    // char s[60]; //snprintf buffer, if needed
    // globals for AHRS loop timing
    unsigned long m_now_ms, m_last_ms = 0; // millis() timers
    // print interval
    unsigned long m_print_ms = 200; // print angles every "print_ms" milliseconds
    float yaw, pitch, roll;         // Euler angle output
    void Mahony_update(float ax, float ay, float az, float gx, float gy, float gz, float deltat);

public:
    MyMPU6050(TSProperties *TSProperties, int p_MPU_addr = 0x68);
    ~MyMPU6050();
    void readDonneesBrutes(int16_t *ax, int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz);
    void calibrer();
    bool detectChut();
    void fusionEnPostures();
    void printPosture();
    void tick() override;
};
