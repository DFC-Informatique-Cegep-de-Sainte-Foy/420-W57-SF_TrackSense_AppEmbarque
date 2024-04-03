#include "Modules/GY87_Adafruit.h"

GY87_Adafruit::GY87_Adafruit(TSProperties *p_tsProperties)
    : _TSProperties(p_tsProperties)
{
    Serial.println("-----> 1 <-----");
    // initialiser les MPU6050 ET HMC5883L
    this->_mpu = new Adafruit_MPU6050();
    this->_compass = new Adafruit_HMC5883_Unified(12345);
    this->sensor_compass = new sensor_t();
    /*---------------------------------Initialiser MPU---------------------------------------*/
    Serial.println("MPU6050 6 accelerator et gyroscoper Test");
    Serial.println("");
    // Stand By Mpu6050
    if (!_mpu->begin())
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
    // switch ON pour que ESP32 connecte directement au HMC5883L
    _mpu->setI2CBypass(true); // 开启从模式，让MPU6050失去对HMC5883L的控制，而是由当前程序的控制者ESP32控制读取HMC5883L的芯片数据
    /*---------------------------------Initialiser Compass---------------------------------------*/
    Serial.println("-----> 2 <-----");
    Serial.println("HMC5883 Magnetometer Test");
    if (!_compass->begin())
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
    /*---------------------------------Creer Sensors---------------------------------------*/
    Serial.println("-----> 3 <-----");
    Serial.println("Creer les 4 Sensors");
    // initialiser les sensors apres mpu6050 stand By
    this->sensor_accel = _mpu->getAccelerometerSensor();
    this->sensor_gyro = _mpu->getGyroSensor();
    this->sensor_temp = _mpu->getTemperatureSensor();
    _compass->getSensor(this->sensor_compass); // 参数注意不要空指针
    /*---------------------------------Creer Events---------------------------------------*/
    Serial.println("-----> 4 <-----");
    Serial.println("Creer les 4 Events");
    // creer sensors events
    this->evt_acce = new sensors_event_t();
    this->evt_gyro = new sensors_event_t();
    this->evt_tem = new sensors_event_t();
    this->evt_compass = new sensors_event_t();
    // lire les datas par sensor events 初始化时暂时不用读数数据
    // _compass->getEvent(this->evt_compass);
    // _mpu->getEvent(this->evt_acce, this->evt_gyro, this->evt_tem);
    /*---------------------------------Sensors details---------------------------------------*/
    Serial.println("-----> 5 <-----");
    Serial.println("Print Sensors details");
    // Printer info des sensor
    sensor_accel->printSensorDetails();
    sensor_gyro->printSensorDetails();
    sensor_temp->printSensorDetails();
    this->displayCompassSensorDetails();
    /*---------------------------------Configuration---------------------------------------*/
    Serial.println("-----> 6 <-----");
    Serial.println("Configuration Parametres pour Calibration");
    // Configuration_MPU
    _mpu->setAccelerometerRange(MPU6050_RANGE_8_G);
    _mpu->setGyroRange(MPU6050_RANGE_500_DEG);
    _mpu->setFilterBandwidth(MPU6050_BAND_21_HZ);
    // Calibration_Compass
    this->calibrationHMC5883L();
    // Declination_Compass
    this->setMagneticDeclination(-15, 5);
    /*---------------------------------Configuration---------------------------------------*/
    Serial.println("-----> 7 <-----");
    Serial.println("Print Configurations");
    // print configuration
    this->printConfiguration();
}

GY87_Adafruit::~GY87_Adafruit()
{
    delete this->_compass;
    this->_compass = nullptr;
    delete this->_mpu;
    this->_mpu = nullptr;
}

void GY87_Adafruit::read()
{
    /*---------------------------------lire donnees bruits via events----------------------------------------*/
    // Lire data bruites
    _compass->getEvent(this->evt_compass);
    _mpu->getEvent(this->evt_acce, this->evt_gyro, this->evt_tem);
    /*----------------------------Appliquer les paramettres de calibration--------------------------------*/
    float GaX = ((evt_compass->magnetic.x) - Xoffset) * Kx;
    float GaY = ((evt_compass->magnetic.y) - Yoffset) * Ky;
    /*------------------------Calculer le heading--------------------------*/
    float Magangle = 0;
    /*----------------------------Appliquer la declination--------------------------------*/
    Magangle += _magneticDeclinationDegrees; // 先应用本地的磁偏角量，再进行三角函数计算
    if ((GaX > 0) && (GaY > 0))
        Magangle = atan(GaY / GaX) * 57.29;
    else if ((GaX > 0) && (GaY < 0))
        Magangle = 360 + atan(GaY / GaX) * 57.29;
    else if ((GaX == 0) && (GaY > 0))
        Magangle = 90;
    else if ((GaX == 0) && (GaY < 0))
        Magangle = 270;
    else if (GaX < 0)
        Magangle = 180 + atan(GaY / GaX) * 57.29;
    /*----------------------------Afficher les data bruites --------------------------------*/
    Serial.print("MagAngle-> ");
    Serial.print(Magangle);
    Serial.print(" ");
    Serial.print("Accel xyz: ");
    Serial.print(evt_acce->acceleration.x);
    Serial.print(" ");
    Serial.print(evt_acce->acceleration.y);
    Serial.print(" ");
    Serial.print(evt_acce->acceleration.z);
    Serial.print(" ");
    // Rendre cohérent le système de coordonnées de l'axe d'accélération et de l'axe de vitesse angulaire
    this->normaliseMPU(&(evt_gyro->gyro.x), &(evt_gyro->gyro.y), &(evt_gyro->gyro.z));
    Serial.print("Rota xyz: ");
    Serial.print(evt_gyro->gyro.x);
    Serial.print(" ");
    Serial.print(evt_gyro->gyro.y);
    Serial.print(" ");
    Serial.print(evt_gyro->gyro.z);
    // Serial.print(" rad/s");
    Serial.println("  ");
    /*-------------------------------------------------------*/
    // Serial.print("Compass XYZ: ");
    // Serial.print(evt_compass->magnetic.x);
    // Serial.print("  ");
    // // Serial.print("Y: ");
    // Serial.print(evt_compass->magnetic.y);
    // Serial.print("  ");
    // // Serial.print("Z: ");
    // Serial.print(evt_compass->magnetic.z);
    // Serial.print("  ");
    // Serial.print("uT");
    // // Serial.print("  ");
    // Serial.println("  ");
    /*-------------------------------------------------------*/
    // Serial.print("Pitch-Gyro: ");
    // Serial.print(temp_evt_gyro.gyro.pitch);
    // Serial.print(", Y: ");
    // Serial.print(temp_evt_gyro.gyro.y);
    // Serial.print(", Z: ");
    // Serial.print(temp_evt_gyro.gyro.z);
    // Serial.print(" rad/s");
    // Serial.print("  ");
    /*-------------------------------------------------------*/

    // -------------------------------calcule heading----------------------------------------------
    // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    // float heading = atan2(temp_evt_compass.magnetic.y, temp_evt_compass.magnetic.x);
    // // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
    // // Find yours here: http://www.magnetic-declination.com/
    // // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
    // // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
    // // float declinationAngle = 0.26;
    // // heading += declinationAngle;

    // // Correct for when signs are reversed.
    // if (heading < 0)
    //     heading += 2 * PI;

    // // Check for wrap due to addition of declination.
    // if (heading > 2 * PI)
    //     heading -= 2 * PI;

    // // Convert radians to degrees for readability.
    // float headingDegrees = heading * 180 / M_PI;

    // Serial.print("Heading (degrees): ");
    // Serial.println(headingDegrees);
    // -------------------------------calcule heading----------------------------------------------

    // Serial.print("Pitch-Compass: ");
    // Serial.println(temp_evt_compass.orientation.pitch);
    // Serial.print("Roll - >");
    // Serial.println(temp_evt_compass.orientation.roll);
    // Serial.print("Status - >");
    // Serial.println(temp_evt_compass.orientation.status);

    // /*---------------------------------Sauvgarder donnees bruits en Json----------------------------------------*/
}

void GY87_Adafruit::displayCompassSensorDetails(void)
{
    // sensor_t sensor;
    _compass->getSensor(this->sensor_compass);
    Serial.println("------------------------------------");
    Serial.print("Sensor:       ");
    Serial.println(sensor_compass->name);
    Serial.print("Driver Ver:   ");
    Serial.println(sensor_compass->version);
    Serial.print("Unique ID:    ");
    Serial.println(sensor_compass->sensor_id);
    Serial.print("Max Value:    ");
    Serial.print(sensor_compass->max_value);
    Serial.println(" uT");
    Serial.print("Min Value:    ");
    Serial.print(sensor_compass->min_value);
    Serial.println(" uT");
    Serial.print("Resolution:   ");
    Serial.print(sensor_compass->resolution);
    Serial.println(" uT");
    Serial.println("------------------------------------");
    Serial.println("");

    Serial.println("------------------------------------");

    delay(500);
}
void GY87_Adafruit::printConfiguration(void)
{
    Serial.print("Accelerometer range set to: ");
    switch (this->_mpu->getAccelerometerRange())
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
    Serial.println("-----------------------------------------------");

    Serial.print("Gyro range set to: ");
    switch (this->_mpu->getGyroRange())
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
    Serial.println("-----------------------------------------------");

    Serial.print("Filter bandwidth set to: ");
    switch (this->_mpu->getFilterBandwidth())
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
    Serial.println("-----------------------------------------------");

    // Calibration offset, scale et Declination magnetic Configuration
    Serial.print("Offset X for HMC5883L set to: ");
    Serial.println(Xoffset);
    Serial.print("Offset Y for HMC5883L set to: ");
    Serial.println(Yoffset);
    Serial.print(" Kx : ");
    Serial.println(Kx);
    Serial.print(" Ky : ");
    Serial.println(Ky);
    Serial.print("Declination  for HMC5883L set to: ");
    Serial.println(this->_magneticDeclinationDegrees);
    Serial.println("-----------------------------------------------");
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
void GY87_Adafruit::calibrationHMC5883L(void)
{
    clearCalibration();
    long calibrationData[3][2] = {{65000, -65000}, {65000, -65000}, {65000, -65000}};
    // long x = calibrationData[0][0] = calibrationData[0][1] = getX();
    // long y = calibrationData[1][0] = calibrationData[1][1] = getY();
    // long z = calibrationData[2][0] = calibrationData[2][1] = getZ();
    /*-------------------------------------------------------------------------*/
    Serial.println("Calibration HMC5883L  ");
    sensors_event_t temp_evt_compass;
    uint8_t i = 0;
    float GaX, GaY, GaXmax = 0, GaXmin = 0, GaYmax = 0, GaYmin = 0;
    while (i != 100)
    {
        _compass->getEvent(&temp_evt_compass);
        GaX = temp_evt_compass.magnetic.x;
        GaY = temp_evt_compass.magnetic.y;

        GaXmax = GaXmax < GaX ? GaX : GaXmax;
        GaXmin = GaXmin > GaX ? GaX : GaXmin;
        GaYmax = GaYmax < GaY ? GaY : GaYmax;
        GaYmin = GaYmax < GaY ? GaY : GaYmin;
        delay(200);
        i++;
        /* code */
    }
    this->Xoffset = (GaXmax + GaXmin) / 2;
    this->Yoffset = (GaYmax + GaYmin) / 2;
    this->Kx = 2 / (GaXmax - GaXmin);
    this->Ky = 2 / (GaYmax - GaYmin);
    // Serial.println("La Calibration est fini.");
    // Serial.print("Xoffset ");
    // Serial.print(Xoffset);
    // Serial.print(" Yoffset ");
    // Serial.print(Yoffset);
    // Serial.print(" Kx ");
    // Serial.print(Kx);
    // Serial.print(" Ky ");
    // Serial.println(Ky);
}

void GY87_Adafruit::clearCalibration()
{
    setCalibrationOffsets(0., 0., 0.);
    setCalibrationScales(1., 1., 1.);
}

void GY87_Adafruit::setCalibrationOffsets(float x_offset, float y_offset, float z_offset)
{
    _offset[0] = x_offset;
    _offset[1] = y_offset;
    _offset[2] = z_offset;
}

void GY87_Adafruit::setCalibrationScales(float x_scale, float y_scale, float z_scale)
{
    _scale[0] = x_scale;
    _scale[1] = y_scale;
    _scale[2] = z_scale;
}
/**
 * Define the magnetic declination for accurate degrees.
 * https://www.magnetic-declination.com/
 *
 * @example
 * For: Londrina, PR, Brazil at date 2022-12-05
 * The magnetic declination is: -19º 43'
 *
 * then: setMagneticDeclination(-19, 43);
 */
void GY87_Adafruit::setMagneticDeclination(int degrees, uint8_t minutes)
{
    _magneticDeclinationDegrees = degrees + minutes / 60;
}

void GY87_Adafruit::tick()
{
    read();
}
