#include "Modules/GY87_Adafruit.h"

GY87_Adafruit::GY87_Adafruit(TSProperties *p_tsProperties)
    : _TSProperties(p_tsProperties),
      _timeSpamBuzzerChute(0)
{
    // initialiser les MPU6050 ET HMC5883L
    this->_mpu = new Adafruit_MPU6050();
    this->_compass = new Adafruit_HMC5883_Unified(12345);
    // creer sensors events
    this->evt_acce = new sensors_event_t();
    this->evt_gyro = new sensors_event_t();
    this->evt_tem = new sensors_event_t();
    this->evt_compass = new sensors_event_t();

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
    // // initialiser les sensors apres mpu6050 stand By
    this->mpu_accel = _mpu->getAccelerometerSensor();
    this->mpu_gyro = _mpu->getGyroSensor();
    this->mpu_temp = _mpu->getTemperatureSensor();

    // switch ON pour que ESP32 connecte directement au HMC5883L
    _mpu->setI2CBypass(true); // 开启从模式，让MPU6050失去对HMC5883L的控制，而是由当前程序的控制者ESP32控制读取HMC5883L的芯片数据
    // /*---------------------------------Initialiser sensor events---------------------------------------*/

    // initailiser les sensor events
    _compass->getEvent(this->evt_compass);
    _mpu->getEvent(this->evt_acce, this->evt_gyro, this->evt_tem);

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

    // Printer info des sensor
    mpu_accel->printSensorDetails();
    mpu_gyro->printSensorDetails();
    mpu_temp->printSensorDetails();
    this->displayCompassSensorDetails();
    // Configuration
    _mpu->setAccelerometerRange(MPU6050_RANGE_8_G);
    _mpu->setGyroRange(MPU6050_RANGE_500_DEG);
    _mpu->setFilterBandwidth(MPU6050_BAND_21_HZ);
    // print configuration
    this->calibrationHMC5883L();
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
    // chaque lu va creer un nouveau event
    sensors_event_t temp_evt_acce;
    sensors_event_t temp_evt_gyro;
    sensors_event_t temp_evt_tem;
    sensors_event_t temp_evt_compass;
    // affecter les events
    _compass->getEvent(&temp_evt_compass);
    _mpu->getEvent(&temp_evt_acce, &temp_evt_gyro, &temp_evt_tem);

    /*---------------------------------lire donnees bruits via events----------------------------------------*/
    float GaX = ((temp_evt_compass.magnetic.x) - Xoffset) * Kx;
    float GaY = ((temp_evt_compass.magnetic.y) - Yoffset) * Ky;

    float Magangle = 0;

    if ((GaX > 0) && (GaY > 0))
        Magangle = atan(GaY / GaX) * 57;
    else if ((GaX > 0) && (GaY < 0))
        Magangle = 360 + atan(GaY / GaX) * 57;
    else if ((GaX == 0) && (GaY > 0))
        Magangle = 90;
    else if ((GaX == 0) && (GaY < 0))
        Magangle = 270;
    else if (GaX < 0)
        Magangle = 180 + atan(GaY / GaX) * 57;

    // // 应用磁偏角
    // Magangle += _magneticDeclinationDegrees;
    Magangle += (-15.6);
    // Serial.print("MagAngle-> ");
    // Serial.println(Magangle);
    _TSProperties->PropertiesCompass.heading = Magangle;
    // Serial.print("Accn X: ");
    // Serial.print(temp_evt_acce.acceleration.x);
    // Serial.print(", Y: ");
    // Serial.print(temp_evt_acce.acceleration.y);
    // Serial.print(", Z: ");
    // Serial.print(temp_evt_acce.acceleration.z);
    // Serial.print(" m/s^2");
    // Serial.print("  ");
    // Rendre cohérent le système de coordonnées de l'axe d'accélération et de l'axe de vitesse angulaire
    /*----------------------------------------Normaliser--------------------------------------------------------------------*/
    this->normaliseMPU(&(temp_evt_gyro.gyro.x), &(temp_evt_gyro.gyro.y), &(temp_evt_gyro.gyro.z));
    /*------------------------------------------------------------------------------------------------------------*/

    // Serial.print("Rot X: ");
    // Serial.print(temp_evt_gyro.gyro.x);
    // Serial.print(", Y: ");
    // Serial.print(temp_evt_gyro.gyro.y);
    // Serial.print(", Z: ");
    // Serial.print(temp_evt_gyro.gyro.z);
    // Serial.print(" rad/s");
    // Serial.print("  ");
    // /*------------------------------------------------------------------------------------------------------------*/
    // Serial.print("Mag X: ");
    // Serial.print(temp_evt_compass.magnetic.x);
    // Serial.print(", Y: ");
    // Serial.print(temp_evt_compass.magnetic.y);
    // Serial.print(", Z: ");
    // Serial.print(temp_evt_compass.magnetic.z);
    // Serial.print(" rad/s");
    // Serial.println("  ");
    /*---------------------------------------------------maj. data raw au TR---------------------------------------------------------*/
    this->_TSProperties->PropertiesCompass.Acc_X = temp_evt_acce.acceleration.x;
    this->_TSProperties->PropertiesCompass.Acc_Y = temp_evt_acce.acceleration.y;
    this->_TSProperties->PropertiesCompass.Acc_Z = temp_evt_acce.acceleration.z;

    this->_TSProperties->PropertiesCompass.Gyro_X = temp_evt_gyro.gyro.x;
    this->_TSProperties->PropertiesCompass.Gyro_Y = temp_evt_gyro.gyro.y;
    this->_TSProperties->PropertiesCompass.Gyro_Z = temp_evt_gyro.gyro.z;

    this->_TSProperties->PropertiesCompass.Mag_X = temp_evt_compass.magnetic.x;
    this->_TSProperties->PropertiesCompass.Mag_Y = temp_evt_compass.magnetic.y;
    this->_TSProperties->PropertiesCompass.Mag_Z = temp_evt_compass.magnetic.z;
    /*-------------------------------------------------Immiter une chute pour tester-----------------------------------------------------------*/
    if (this->_TSProperties->PropertiesCompass.Acc_X > 4 && this->_TSProperties->PropertiesCompass.Gyro_X > 4)
    {
        Serial.println("Chute Detecter!");
        this->_TSProperties->PropertiesGPS.estChute = true;
        _timeSpamBuzzerChute = millis();
    }
    if (millis() - _timeSpamBuzzerChute < 4000)
    {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(500);
        digitalWrite(PIN_BUZZER, LOW);
    }
    /*------------------------------------------------------------------------------------------------------------*/

    // Serial.print("Pitch-Gyro: ");
    // Serial.print(temp_evt_gyro.gyro.pitch);
    // Serial.print(", Y: ");
    // Serial.print(temp_evt_gyro.gyro.y);
    // Serial.print(", Z: ");
    // Serial.print(temp_evt_gyro.gyro.z);
    // Serial.print(" rad/s");
    // Serial.print("  ");

    // Serial.print("XYZ: ");
    // Serial.print(temp_evt_compass.magnetic.x);
    // Serial.print("  ");
    // // Serial.print("Y: ");
    // Serial.print(temp_evt_compass.magnetic.y);
    // Serial.print("  ");
    // // Serial.print("Z: ");
    // Serial.print(temp_evt_compass.magnetic.z);
    // Serial.print("  ");
    // Serial.print("uT");
    // // Serial.print("  ");
    // Serial.println("  ");
    // 以上程序获得磁力计的原始数据，单位特斯拉，是单位高斯的千分之一
    // 一下程序进行HMC原始数据校准
    // Serial.println("Calibration HMC5883L  ");
    // uint8_t i = 0;
    // float GaX, GaY, GaXmax = 0, GaXmin = 0, GaYmax = 0, GaYmin = 0;
    // while (i != 100)
    // {
    //     GaX = temp_evt_compass.magnetic.x;
    //     GaY = temp_evt_compass.magnetic.y;

    //     GaXmax = GaXmax < GaX ? GaX : GaXmax;
    //     GaXmin = GaXmin > GaX ? GaX : GaXmin;
    //     GaYmax = GaYmax < GaY ? GaY : GaYmax;
    //     GaYmin = GaYmax < GaY ? GaY : GaYmin;
    //     delay(200);
    //     i++;
    //     /* code */
    // }

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

    // -------------------------------计算欧拉角----------------------------------------------
    // Serial.print("Time -> ");
    // Serial.println(esp_timer_get_time());
    // this->Now = esp_timer_get_time();
    // this->deltat = (float)((this->Now - this->lastUpdate) / 1000000.0f);
    // this->lastUpdate = this->Now;

    // 计算四元数来获取欧拉角
    // this->MadgwickQuaternionUpdate(this->evt_acce->acceleration.x, this->evt_acce->acceleration.y, this->evt_acce->acceleration.z, this->evt_gyro->gyro.x * PI / 180.0f, this->evt_gyro->gyro.y * PI / 180.0f, this->evt_gyro->gyro.z * PI / 180.0f, this->evt_compass->magnetic.x, this->evt_compass->magnetic.y, this->evt_compass->magnetic.z);
    // Serial.print("Q1 : ");
    // Serial.print(this->q[0]);
    // Serial.print("  Q2 : ");
    // Serial.print(this->q[1]);
    // Serial.print("  Q3 : ");
    // Serial.print(this->q[2]);
    // Serial.print("  Q4 : ");
    // Serial.println(this->q[3]);

    // this->quaternionToEuler();
    // Serial.print("Pitch : ");
    // Serial.print(this->pitch);
    // Serial.print("  Roll : ");
    // Serial.print(this->roll);
    // Serial.print("  Yaw : ");
    // Serial.println(this->yaw);

    // if (this->pitch > 45)
    // {
    //     Serial.println("Chute detecter!");
    //     this->_TSProperties->PropertiesGPS.estChute = true;
    // }

    // /*---------------------------------Sauvgarder donnees bruits en Json----------------------------------------*/
}

void GY87_Adafruit::displayCompassSensorDetails(void)
{
    sensor_t sensor;
    // _compass->getSensor(this->sensor_compass);
    _compass->getSensor(&sensor);
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
    // Serial.println("8---GY87 --> tick");
    read();
}

void GY87_Adafruit::MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{

    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3]; // short name local variable for readability
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f)
    {
        return;
    } // handle NaN
    norm = 1.0f / norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f)
    {
        return;
    } // handle NaN
    norm = 1.0f / norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4); // normalise step magnitude
    norm = 1.0f / norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4); // normalise quaternion
    norm = 1.0f / norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;

    // Serial.print("Q1 ");
    // Serial.print(q[0]);
    // Serial.print(" Q2 ");
    // Serial.print(q[1]);
    // Serial.print(" Q3 ");
    // Serial.print(q[2]);
    // Serial.print(" Q4 ");
    // Serial.println(q[03]);
}

void GY87_Adafruit::quaternionToEuler()
{
    yaw = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw *= 180.0f / PI;
    yaw -= 54.0f; // Declination: 00 54 48.83, 2023-3-23，应用魁北克当地的磁偏角
    roll *= 180.0f / PI;

    // Get gravity
    float gravityVector[3];
    gravityVector[0] = 2 * (q[1] * q[3] - q[0] * q[2]);
    gravityVector[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
    gravityVector[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
}
