#include "Modules/MPU6050.h"
#include <TSProperties.h>

MPU6050::MPU6050(TSProperties *TSProperties, int p_MPU_addr)
    : m_MPU_addr(p_MPU_addr),
      _TSProperties(TSProperties)
{
    Wire.begin();
    Serial.begin(9600);
    Serial.println("starting setup MPU");
    // initialize sensor
    // defaults for gyro and accel sensitivity are 250 dps and +/- 2 g
    Wire.beginTransmission(m_MPU_addr);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Serial.println("CTOR MPU");
}

bool MPU6050::detectChut()
{
}

void MPU6050::fusionEnPostures()
{
    // Compute Tait-Bryan angles. Strictly valid only for approximately level movement

    // In this coordinate system, the positive z-axis is up, X north, Y west.
    // Yaw is the angle between Sensor x-axis and Earth magnetic North
    // (or true North if corrected for local declination, looking down on the sensor
    // positive yaw is counterclockwise, which is not conventional for NED navigation.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the
    // Earth is positive, up toward the sky is negative. Roll is angle between
    // sensor y-axis and Earth ground plane, y-axis up is positive roll. These
    // arise from the definition of the homogeneous rotation matrix constructed
    // from quaternions. Tait-Bryan angles as well as Euler angles are
    // non-commutative; that is, the get the correct orientation the rotations
    // must be applied in the correct order which for this configuration is yaw,
    // pitch, and then roll.
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // which has additional links.

    // WARNING: This angular conversion is for DEMONSTRATION PURPOSES ONLY. It WILL
    // MALFUNCTION for certain combinations of angles! See https://en.wikipedia.org/wiki/Gimbal_lock

    roll = atan2((m_quaternion[0] * m_quaternion[1] + m_quaternion[2] * m_quaternion[3]), 0.5 - (m_quaternion[1] * m_quaternion[1] + m_quaternion[2] * m_quaternion[2]));
    pitch = asin(2.0 * (m_quaternion[0] * m_quaternion[2] - m_quaternion[1] * m_quaternion[3]));
    // conventional yaw increases clockwise from North. Not that the MPU-6050 knows where North is.
    yaw = -atan2((m_quaternion[1] * m_quaternion[2] + m_quaternion[0] * m_quaternion[3]), 0.5 - (m_quaternion[2] * m_quaternion[2] + m_quaternion[3] * m_quaternion[3]));
    // to degrees
    yaw *= 180.0 / PI;
    if (yaw < 0)
        yaw += 360.0; // compass circle
    pitch *= 180.0 / PI;
    roll *= 180.0 / PI;
}

void MPU6050::printPosture()
{
    // print angles for serial plotter...
    Serial.print("yaw: ");
    Serial.print(this->yaw, 0);
    Serial.print(", ");
    Serial.print("pitch: ");
    Serial.print(this->pitch, 0);
    Serial.print(", ");
    Serial.print("roll: ");
    Serial.println(this->roll, 0);

    // 显示当前的状态
    // 根据yaw判断航向-指南针
    // 根据pitch判断俯仰
    // 根据roll判断翻滚
    if (this->yaw == 0 || this->yaw == 360)
    {
        Serial.print("North  ");
    }
    else if (this->yaw == 180)
    {
        Serial.print("sud  ");
    }
    else if (this->yaw == 90)
    {
        Serial.print("East  ");
    }
    else if (this->yaw == 270)
    {
        Serial.print("Ouest  ");
    }
    else if (this->yaw > 0 && this->yaw < 90)
    {
        Serial.print("North-Eath  ");
    }
    else if (this->yaw > 90 && this->yaw < 180)
    {
        Serial.print("Sud-East  ");
    }
    else if (this->yaw > 180 && this->yaw < 270)
    {
        Serial.print("Sud-Ouest  ");
    }
    else if (this->yaw > 270 && this->yaw < 360)
    {
        Serial.print("North-Ouest  ");
    }
}
void MPU6050::Mahony_update(float ax, float ay, float az, float gx, float gy, float gz, float deltat)
{
    float recipNorm;
    float vx, vy, vz;
    float ex, ey, ez; // error terms
    float qa, qb, qc;
    static float ix = 0.0, iy = 0.0, iz = 0.0; // integral feedback terms
    float tmp;

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    tmp = ax * ax + ay * ay + az * az;
    if (tmp > 0.0)
    {

        // Normalise accelerometer (assumed to measure the direction of gravity in body frame)
        recipNorm = 1.0 / sqrt(tmp);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Estimated direction of gravity in the body frame (factor of two divided out)
        vx = m_quaternion[1] * m_quaternion[3] - m_quaternion[0] * m_quaternion[2]; // to normalize these terms, multiply each by 2.0
        vy = m_quaternion[0] * m_quaternion[1] + m_quaternion[2] * m_quaternion[3];
        vz = m_quaternion[0] * m_quaternion[0] - 0.5f + m_quaternion[3] * m_quaternion[3];

        // Error is cross product between estimated and measured direction of gravity in body frame
        // (half the actual magnitude)
        ex = (ay * vz - az * vy);
        ey = (az * vx - ax * vz);
        ez = (ax * vy - ay * vx);

        // Compute and apply to gyro term the integral feedback, if enabled
        if (m_Ki > 0.0f)
        {
            ix += m_Ki * ex * deltat; // integral error scaled by Ki
            iy += m_Ki * ey * deltat;
            iz += m_Ki * ez * deltat;
            gx += ix; // apply integral feedback
            gy += iy;
            gz += iz;
        }

        // Apply proportional feedback to gyro term
        gx += m_Kp * ex;
        gy += m_Kp * ey;
        gz += m_Kp * ez;
    }

    // Integrate rate of change of quaternion, q cross gyro term
    deltat = 0.5 * deltat;
    gx *= deltat; // pre-multiply common factors
    gy *= deltat;
    gz *= deltat;
    qa = m_quaternion[0];
    qb = m_quaternion[1];
    qc = m_quaternion[2];
    m_quaternion[0] += (-qb * gx - qc * gy - m_quaternion[3] * gz);
    m_quaternion[1] += (qa * gx + qc * gz - m_quaternion[3] * gy);
    m_quaternion[2] += (qa * gy - qb * gz + m_quaternion[3] * gx);
    m_quaternion[3] += (qa * gz + qb * gy - qc * gx);

    // renormalise quaternion
    recipNorm = 1.0 / sqrt(m_quaternion[0] * m_quaternion[0] + m_quaternion[1] * m_quaternion[1] + m_quaternion[2] * m_quaternion[2] + m_quaternion[3] * m_quaternion[3]);
    m_quaternion[0] = m_quaternion[0] * recipNorm;
    m_quaternion[1] = m_quaternion[1] * recipNorm;
    m_quaternion[2] = m_quaternion[2] * recipNorm;
    m_quaternion[3] = m_quaternion[3] * recipNorm;
}
void MPU6050::calibrer()
{
}

MPU6050::~MPU6050()
{
    ;
}

void MPU6050::readDonneesBrutes(int16_t *ax, int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz)
{
    Wire.beginTransmission(m_MPU_addr);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(m_MPU_addr, 14); // request a total of 14 registers
    int t = Wire.read() << 8;
    *ax = t | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    t = Wire.read() << 8;
    *ay = t | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    t = Wire.read() << 8;
    *az = t | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    t = Wire.read() << 8;
    m_tmp = t | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    t = Wire.read() << 8;
    *gx = t | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    t = Wire.read() << 8;
    *gy = t | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    t = Wire.read() << 8;
    *gz = t | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

    Axyz[0] = (float)*ax;
    Axyz[1] = (float)*ay;
    Axyz[2] = (float)*az;
    // apply offsets and scale factors from Magneto
    for (int i = 0; i < 3; i++)
        Axyz[i] = (Axyz[i] - m_A_cal[i]) * m_A_cal[i + 3];
    Gxyz[0] = ((float)*gx - m_G_off[0]) * gscale; // 250 LSB(d/s) default to radians/s
    Gxyz[1] = ((float)*gy - m_G_off[1]) * gscale;
    Gxyz[2] = ((float)*gz - m_G_off[2]) * gscale;
}
void MPU6050::tick()
{
    static float deltat = 0;                // loop time in seconds
    static unsigned long now = 0, last = 0; // micros() timers
    readDonneesBrutes(&this->m_ax, &this->m_ay, &this->m_az, &this->m_gx, &this->m_gy, &this->m_gz);
    // raw data
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t Tmp; // temperature

    // scaled data as vector
    float Axyz[3];
    float Gxyz[3];

    Wire.beginTransmission(m_MPU_addr);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(m_MPU_addr, 14); // request a total of 14 registers
    int t = Wire.read() << 8;
    ax = t | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    t = Wire.read() << 8;
    ay = t | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    t = Wire.read() << 8;
    az = t | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    t = Wire.read() << 8;
    Tmp = t | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    t = Wire.read() << 8;
    gx = t | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    t = Wire.read() << 8;
    gy = t | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    t = Wire.read() << 8;
    gz = t | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

    Axyz[0] = (float)ax;
    Axyz[1] = (float)ay;
    Axyz[2] = (float)az;

    // apply offsets and scale factors from Magneto
    for (int i = 0; i < 3; i++)
        Axyz[i] = (Axyz[i] - m_A_cal[i]) * m_A_cal[i + 3];

    Gxyz[0] = ((float)gx - m_G_off[0]) * gscale; // 250 LSB(d/s) default to radians/s
    Gxyz[1] = ((float)gy - m_G_off[1]) * gscale;
    Gxyz[2] = ((float)gz - m_G_off[2]) * gscale;

    //  snprintf(s,sizeof(s),"mpu raw %d,%d,%d,%d,%d,%d",ax,ay,az,gx,gy,gz);
    //  Serial.println(s);

    now = micros();
    deltat = (now - last) * 1.0e-6; // seconds since last update
    last = now;
    Mahony_update(Axyz[0], Axyz[1], Axyz[2], Gxyz[0], Gxyz[1], Gxyz[2], deltat);

    // Compute Tait-Bryan angles. Strictly valid only for approximately level movement

    // In this coordinate system, the positive z-axis is up, X north, Y west.
    // Yaw is the angle between Sensor x-axis and Earth magnetic North
    // (or true North if corrected for local declination, looking down on the sensor
    // positive yaw is counterclockwise, which is not conventional for NED navigation.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the
    // Earth is positive, up toward the sky is negative. Roll is angle between
    // sensor y-axis and Earth ground plane, y-axis up is positive roll. These
    // arise from the definition of the homogeneous rotation matrix constructed
    // from quaternions. Tait-Bryan angles as well as Euler angles are
    // non-commutative; that is, the get the correct orientation the rotations
    // must be applied in the correct order which for this configuration is yaw,
    // pitch, and then roll.
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // which has additional links.

    // WARNING: This angular conversion is for DEMONSTRATION PURPOSES ONLY. It WILL
    // MALFUNCTION for certain combinations of angles! See https://en.wikipedia.org/wiki/Gimbal_lock

    roll = atan2((m_quaternion[0] * m_quaternion[1] + m_quaternion[2] * m_quaternion[3]), 0.5 - (m_quaternion[1] * m_quaternion[1] + m_quaternion[2] * m_quaternion[2]));
    pitch = asin(2.0 * (m_quaternion[0] * m_quaternion[2] - m_quaternion[1] * m_quaternion[3]));
    // conventional yaw increases clockwise from North. Not that the MPU-6050 knows where North is.
    yaw = -atan2((m_quaternion[1] * m_quaternion[2] + m_quaternion[0] * m_quaternion[3]), 0.5 - (m_quaternion[2] * m_quaternion[2] + m_quaternion[3] * m_quaternion[3]));
    // to degrees
    yaw *= 180.0 / PI;
    if (yaw < 0)
        yaw += 360.0; // compass circle
    pitch *= 180.0 / PI;
    roll *= 180.0 / PI;

    // MAJ TSPropertiesPosture
    this->_TSProperties->PropertiesPosture.PitchAngle = pitch;
    this->_TSProperties->PropertiesPosture.RollAngle = roll;
    this->_TSProperties->PropertiesPosture.YawAngle = yaw;

    // detecer chut
    if (abs(roll) >= roll_threshold || abs(pitch) >= pitch_threshold)
    {
        Serial.println("Chute détectée");
        this->_TSProperties->PropertiesPosture.isChute = true;
    }
    else
    {
        this->_TSProperties->PropertiesPosture.isChute = false;
    }

    m_now_ms = millis(); // time to print?
    if (m_now_ms - m_last_ms >= m_print_ms)
    {
        m_last_ms = m_now_ms;
        // printPosture();
        // print angles for serial plotter...
        Serial.print("yaw: ");
        Serial.print(this->yaw, 0);
        Serial.print(", ");
        Serial.print("pitch: ");
        Serial.print(this->pitch, 0);
        Serial.print(", ");
        Serial.print("roll: ");
        Serial.println(this->roll, 0);

        if (yaw == 0 || yaw == 360)
        {
            Serial.print("North  ");
        }
        else if (yaw == 180)
        {
            Serial.print("sud  ");
        }
        else if (yaw == 90)
        {
            Serial.print("East  ");
        }
        else if (yaw == 270)
        {
            Serial.print("Ouest  ");
        }
        else if (yaw > 0 && yaw < 90)
        {
            Serial.print("North-Eath  ");
        }
        else if (yaw > 90 && yaw < 180)
        {
            Serial.print("Sud-East  ");
        }
        else if (yaw > 180 && yaw < 270)
        {
            Serial.print("Sud-Ouest  ");
        }
        else if (yaw > 270 && yaw < 360)
        {
            Serial.print("North-Ouest  ");
        }
    }
}
