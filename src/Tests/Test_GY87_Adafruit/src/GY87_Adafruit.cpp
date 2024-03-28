#include "GY87_Adafruit.h"

GY87_Adafruit::GY87_Adafruit()
{
    // initialiser les MPU6050 ET HMC5883L
    this->mpu = new Adafruit_MPU6050();
    this->compass = new Adafruit_HMC5883_Unified(12345);
    // creer sensors events
    this->evt_acce = new sensors_event_t();
    this->evt_gyro = new sensors_event_t();
    this->evt_tem = new sensors_event_t();
    this->evt_compass = new sensors_event_t();

    Serial.println("MPU6050 6 accelerator et gyroscoper Test");
    Serial.println("");
    // Stand By Mpu6050
    if (!mpu->begin())
    {
        /* There was a problem detecting the MPU6050 ... check your connections */
        Serial.println("Ooops, no MPU6050 detected ... Check your wiring!");
        while (1)
            ;
    }
    else
    {
        Serial.println("MPU6050 Stand By...");
    }
    // initialiser les sensors apres mpu6050 stand By
    this->mpu_accel = mpu->getAccelerometerSensor();
    this->mpu_gyro = mpu->getGyroSensor();
    this->mpu_temp = mpu->getTemperatureSensor();

    // switch ON pour que ESP32 connecte directement au HMC5883L
    mpu->setI2CBypass(true); // 开启从模式，让MPU6050失去对HMC5883L的控制，而是由当前程序的控制者ESP32控制读取HMC5883L的芯片数据

    // initailiser les sensor events
    compass->getEvent(this->evt_compass);
    mpu->getEvent(this->evt_acce, this->evt_gyro, this->evt_tem);

    //
    Serial.println("HMC5883 Magnetometer Test");
    // Stand By HMC5883L
    if (!compass->begin())
    {
        /* There was a problem detecting the HMC5883 ... check your connections */
        Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
        while (1)
            ;
    }
    else
    {
        Serial.println("HMC5883 Stand By...");
    }

    // Printer info des sensor
    mpu_accel->printSensorDetails();
    mpu_gyro->printSensorDetails();
    mpu_temp->printSensorDetails();
    this->displayCompassSensorDetails();
    // Configuration
    mpu->setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu->setGyroRange(MPU6050_RANGE_500_DEG);
    mpu->setFilterBandwidth(MPU6050_BAND_21_HZ);
    // print configuration
    this->printConfiguration();
}

GY87_Adafruit::~GY87_Adafruit()
{
    delete this->compass;
    this->compass = nullptr;
    delete this->mpu;
    this->mpu = nullptr;
}

void GY87_Adafruit::_read()
{
    // chaque lu va creer un nouveau event
    sensors_event_t temp_evt_acce;
    sensors_event_t temp_evt_gyro;
    sensors_event_t temp_evt_tem;
    sensors_event_t temp_evt_compass;
    // affecter les events
    compass->getEvent(&temp_evt_compass);
    mpu->getEvent(&temp_evt_acce, &temp_evt_gyro, &temp_evt_tem);
    // update membre_event
    // this->evt_acce = temp_evt_acce;
    // this->evt_gyro = temp_evt_gyro;
    // this->evt_tem = temp_evt_tem;
    // this->evt_compass = temp_evt_compass;

    // lire les données bruites
    /* Print out the values */
    // Serial.print("Acceleration X: ");
    // Serial.print(this->evt_acce->acceleration.x);
    // Serial.print(", Y: ");
    // Serial.print(this->evt_acce->acceleration.y);
    // Serial.print(", Z: ");
    // Serial.print(this->evt_acce->acceleration.z);
    // Serial.print(" m/s^2");
    // Serial.println("  ");

    // Serial.print("Rotation X: ");
    // Serial.print(this->evt_gyro->gyro.x);
    // Serial.print(", Y: ");
    // Serial.print(this->evt_gyro->gyro.y);
    // Serial.print(", Z: ");
    // Serial.print(this->evt_gyro->gyro.z);
    // Serial.print(" rad/s");
    // Serial.println("  ");

    // Serial.print("X: ");
    // Serial.print(this->evt_compass->magnetic.x);
    // Serial.print("  ");
    // Serial.print("Y: ");
    // Serial.print(this->evt_compass->magnetic.y);
    // Serial.print("  ");
    // Serial.print("Z: ");
    // Serial.print(this->evt_compass->magnetic.z);
    // Serial.print("  ");
    // Serial.print("uT");
    // Serial.print("  ");
    /*---------------------------------lire donnees bruits via events----------------------------------------*/
    // Serial.print("Acceleration X: ");
    // Serial.print(temp_evt_acce.acceleration.x);
    // Serial.print(", Y: ");
    // Serial.print(temp_evt_acce.acceleration.y);
    // Serial.print(", Z: ");
    // Serial.print(temp_evt_acce.acceleration.z);
    // Serial.print(" m/s^2");
    // Serial.print("  ");
    // Rendre cohérent le système de coordonnées de l'axe d'accélération et de l'axe de vitesse angulaire
    this->normaliseMPU(&(temp_evt_gyro.gyro.x), &(temp_evt_gyro.gyro.y), &(temp_evt_gyro.gyro.z));

    Serial.print("Rotation X: ");
    Serial.print(temp_evt_gyro.gyro.x);
    Serial.print(", Y: ");
    Serial.print(temp_evt_gyro.gyro.y);
    Serial.print(", Z: ");
    Serial.print(temp_evt_gyro.gyro.z);
    Serial.print(" rad/s");
    Serial.print("  ");

    Serial.print("X: ");
    Serial.print(temp_evt_compass.magnetic.x);
    Serial.print("  ");
    Serial.print("Y: ");
    Serial.print(temp_evt_compass.magnetic.y);
    Serial.print("  ");
    Serial.print("Z: ");
    Serial.print(temp_evt_compass.magnetic.z);
    Serial.print("  ");
    Serial.print("uT");
    Serial.print("  ");
    Serial.println("  ");
    /*---------------------------------Sauvgarder donnees bruits en Json----------------------------------------*/
}

void GY87_Adafruit::displayCompassSensorDetails(void)
{
    sensor_t sensor;
    compass->getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print("Sensor:       ");
    Serial.println(sensor.name);
    Serial.print("Driver Ver:   ");
    Serial.println(sensor.version);
    Serial.print("Unique ID:    ");
    Serial.println(sensor.sensor_id);
    Serial.print("Max Value:    ");
    Serial.print(sensor.max_value);
    Serial.println(" uT");
    Serial.print("Min Value:    ");
    Serial.print(sensor.min_value);
    Serial.println(" uT");
    Serial.print("Resolution:   ");
    Serial.print(sensor.resolution);
    Serial.println(" uT");
    Serial.println("------------------------------------");
    Serial.println("");

    Serial.println("------------------------------------");

    delay(500);
}

void GY87_Adafruit::printConfiguration(void)
{
    Serial.print("Accelerometer range set to: ");
    switch (this->mpu->getAccelerometerRange())
    {
    case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    Serial.print("Gyro range set to: ");
    switch (this->mpu->getGyroRange())
    {
    case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
    case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
    case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
    case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }

    Serial.print("Filter bandwidth set to: ");
    switch (this->mpu->getFilterBandwidth())
    {
    case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
    case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
    case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
    case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
    case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
    case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
    case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }
    // Calibration offset, scale et Declination magnetic Configuration
    Serial.println("Offset for HMC5883L set to: ");
    Serial.println("Scale  for HMC5883L set to: ");
    Serial.println("Declination  for HMC5883L set to: ");

    Serial.println("");
}
/// @brief normaliser le system de cartésiennes pour acceleration et gyroscope
/// @param gyro_X
/// @param gyro_Y
/// @param gyro_Z
void GY87_Adafruit::normaliseMPU(float *gyro_X, float *gyro_Y, float *gyro_Z)
{
    // switch x avec y
    float temp_gyro_x = *gyro_X;
    *gyro_X = -(*gyro_Y);
    *gyro_Y = temp_gyro_x;
    // oppoer de l'axe z
    *gyro_Z = -(*gyro_Z);
}
