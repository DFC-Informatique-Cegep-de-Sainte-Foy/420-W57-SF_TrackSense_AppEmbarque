#ifndef __MUMPU9250_HPP__
#define __MUMPU9250_HPP__

#include <Arduino.h>
#include "mbed.h"
#include "math.h"

// Magnetometer registers
#define AK8963_ADDRESS 0x0C << 1
#define WHO_AM_I_AK8963 0x00 // should return 0x48
#define INFO 0x01
#define AK8963_ST1 0x02    // data ready status bit 0
#define AK8963_XOUT_L 0x03 // data
#define AK8963_XOUT_H 0x04
#define AK8963_YOUT_L 0x05
#define AK8963_YOUT_H 0x06
#define AK8963_ZOUT_L 0x07
#define AK8963_ZOUT_H 0x08
#define AK8963_ST2 0x09    // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL 0x0A   // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC 0x0C   // Self test control
#define AK8963_I2CDIS 0x0F // I2C disable
#define AK8963_ASAX 0x10   // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY 0x11   // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ 0x12   // Fuse ROM z-axis sensitivity adjustment value

// MPU9250 registers
#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define XG_OFFSET_H 0x13 // User-defined trim values for gyroscope
#define XG_OFFSET_L 0x14
#define YG_OFFSET_H 0x15
#define YG_OFFSET_L 0x16
#define ZG_OFFSET_H 0x17
#define ZG_OFFSET_L 0x18

#define SMPLRT_DIV 0x19

#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_CONFIG2 0x1D

#define LP_ACCEL_ODR 0x1E
#define WOM_THR 0x1F

#define FIFO_EN 0x23
#define I2C_MST_CTRL 0x24
#define I2C_SLV0_ADDR 0x25
#define I2C_SLV0_REG 0x26
#define I2C_SLV0_CTRL 0x27
#define I2C_SLV1_ADDR 0x28
#define I2C_SLV1_REG 0x29
#define I2C_SLV1_CTRL 0x2A
#define I2C_SLV2_ADDR 0x2B
#define I2C_SLV2_REG 0x2C
#define I2C_SLV2_CTRL 0x2D
#define I2C_SLV3_ADDR 0x2E
#define I2C_SLV3_REG 0x2F
#define I2C_SLV3_CTRL 0x30
#define I2C_SLV4_ADDR 0x31
#define I2C_SLV4_REG 0x32
#define I2C_SLV4_DO 0x33
#define I2C_SLV4_CTRL 0x34
#define I2C_SLV4_DI 0x35
#define I2C_MST_STATUS 0x36
#define INT_PIN_CFG 0x37
#define INT_ENABLE 0x38

#define INT_STATUS 0x3A
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO 0x63
#define I2C_SLV1_DO 0x64
#define I2C_SLV2_DO 0x65
#define I2C_SLV3_DO 0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET 0x68
#define MOT_DETECT_CTRL 0x69
#define USER_CTRL 0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1 0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2 0x6C
#define DMP_BANK 0x6D   // Activates a specific bank in the DMP
#define DMP_RW_PNT 0x6E // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG 0x6F    // Register in DMP from which to read or to which to write
#define DMP_REG_1 0x70
#define DMP_REG_2 0x71
#define FIFO_COUNTH 0x72
#define FIFO_COUNTL 0x73
#define FIFO_R_W 0x74
#define WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define XA_OFFSET_H 0x77
#define XA_OFFSET_L 0x78
#define YA_OFFSET_H 0x7A
#define YA_OFFSET_L 0x7B
#define ZA_OFFSET_H 0x7D
#define ZA_OFFSET_L 0x7E

#define PI 3.14159265358979323846f

#define X 0
#define Y 1
#define Z 2

// Using the MSENSR-9250 breakout board, ADO is set to 0
// Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1
// mbed uses the eight-bit device address, so shift seven-bit addresses left by one!
#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69 << 1 // Device address when ADO = 1 -> 3.3v
#else
#define MPU9250_ADDRESS 0x68 << 1 // Device address when ADO = 0 -> 5v
#endif

// Set initial input parameters
enum AccelerationScale
{
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum GyroScale
{
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

enum MagneticScale
{
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS,     // 0.15 mG per LSB
  MFS_ARDUINO
};

class MPU9250
{
public:
  MPU9250(PinName sda, PinName scl, uint8_t aScale = AFS_2G, uint8_t gScale = GFS_250DPS, uint8_t mScale = MFS_16BITS);
  MPU9250(I2C *i2c);
  ~MPU9250();

  void writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
  char readByte(uint8_t address, uint8_t subAddress);
  void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t *dest);

  void getAccelRes();
  void getGyroRes();
  void getMagRes();

  void readAccelData();
  void readGyroData();
  void readMagData();
  int16_t readTempData();

  void resetMPU9250();
  void initAK8963();
  void initMPU9250();
  void calibrateMPU9250();
  void MPU9250SelfTest();

  // Testing...
  float GyroMeasError = PI * (60.0f / 180.0f);
  float beta = sqrt(3.0f / 4.0f) * GyroMeasError; // compute beta
  float GyroMeasDrift = PI * (1.0f / 180.0f);     // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
  float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift; // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
  // 쿼터니언 w, x, y, z를 이용하여 yaw, pitch, roll을 계산하는 함수
  void quaternionToEuler()
  {
    yaw = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw *= 180.0f / PI;
    yaw -= 54.0f; // Declination: 00 54 48.83, 2023-3-23
    roll *= 180.0f / PI;

    // Get gravity
    float gravityVector[3];
    gravityVector[0] = 2 * (q[1] * q[3] - q[0] * q[2]);
    gravityVector[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
    gravityVector[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
  }

  void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
  void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);

public:
  float selfTest[6];

  float calibrateGyro[3];
  float calibrateAccel[3];
  float calibrationMag[3];
  float magbias[3];

  uint8_t aScale;         // AFS_2G, AFS_4G, AFS_8G, AFS_16G
  uint8_t gScale;         // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
  uint8_t mScale;         // MFS_14BITS or MFS_16BITS, 14-bit or 16-bit magnetometer resolution
  uint8_t mMode;          // Either 8 Hz 0x02) or 100 Hz (0x06) magnetometer data ODR
  float aRes, gRes, mRes; // scale resolutions per LSB for the sensors

  int16_t accelCount[3];                 // Stores the 16-bit signed accelerometer sensor output
  int16_t gyroCount[3];                  // Stores the 16-bit signed gyro sensor output
  int16_t magCount[3];                   // Stores the 16-bit signed magnetometer sensor output
  float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}; // vector to hold quaternion

  int16_t rawAccelData[3];
  int16_t rawGyroData[3];
  int16_t rawMagData[3];

  float accel[3];
  float gyro[3];
  float mag[3];

  float pitch, yaw, roll;
  float deltat = 0.0f;              // integration interval for both filter schemes
  int lastUpdate, firstUpdate, Now; // used to calculate integration interval
  int delt_t;                       // used to control display output rate
  int count;                        // used to control display output rate
  int16_t tempCount;                // Stores the real internal chip temperature in degrees Celsius
  float temperature;

private:
  I2C *i2c;
};

#endif