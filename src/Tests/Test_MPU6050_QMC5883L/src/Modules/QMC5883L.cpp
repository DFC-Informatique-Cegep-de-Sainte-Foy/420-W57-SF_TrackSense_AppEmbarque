#include "Modules/QMC5883L.h"
#include "TSProperties.h"

QMC5883L::QMC5883L(TSProperties *p_tsProperties, MyMPU6050 *p_mpu6050, int p_QMC_addr)
    : _TSProperties(p_tsProperties),
      _mpu6050(p_mpu6050),
      _QMCaddr(p_QMC_addr)
{
    Serial.println("CTOR QMC");
    _compass = new QMC5883LCompass(); // mprograms/QMC5883LCompass@^1.2.3
    _compass->setADDR(_QMCaddr);      // par default 0x0D
    _compass->init();                 // etre appelé dans setup()
    // 设定磁偏角
    _compass->setMagneticDeclination(-15, 5);
    // 粘贴校准后的调整方法;
    // _compass->setCalibrationOffsets(651.00, -373.00, -760.00);
    // _compass->setCalibrationScales(1.07, 1.02, 0.92);
    // _compass->setCalibrationOffsets(623.00, -349.00, -899.00);
    // _compass->setCalibrationScales(1.14, 1.16, 0.79);
    _compass->setCalibrationOffsets(-355.00, -352.00, -1510.00);
    _compass->setCalibrationScales(1.69, 1.41, 0.59);
}

QMC5883L::~QMC5883L()
{
    // delete this->_compass;
}

void QMC5883L::tick()
{
    // reading
    this->obtenirXYZ();
    // // 获取当前方位角
    this->obtenirAzimuth();
    this->obtenirBearing();
    this->obtenirDirection();
    // // 更新当前TS数据
    this->_TSProperties->PropertiesCompass.Heading_angle = this->_azimuth;
    // this->_TSProperties->PropertiesCompass.XH = this->_compass->getX();
    // this->_TSProperties->PropertiesCompass.YH = this->_compass->getY();
    // this->_TSProperties->PropertiesCompass.ZH = this->_compass->getZ();
    // // 打印debug数据
    // // this->printData();
}

int QMC5883L::obtenirAzimuth()
{
    this->_azimuth = this->_compass->getAzimuth();
    return this->_azimuth;
}

byte QMC5883L::obtenirBearing()
{
    this->_bearing = _compass->getBearing(this->_azimuth);
    return this->_bearing;
}

void QMC5883L::obtenirDirection()
{
    int d = this->_compass->getBearing(this->_azimuth);
    monOrientation[0] = _bearings[d][0];
    monOrientation[1] = _bearings[d][1];
    monOrientation[2] = _bearings[d][2];
}

void QMC5883L::printData()
{
    Serial.print("Azimuth-> ");
    Serial.print(this->_TSProperties->PropertiesCompass.Heading_angle);
    Serial.print("  ");
    Serial.print(monOrientation[0]);
    Serial.print(monOrientation[1]);
    Serial.println(monOrientation[2]);
}

void QMC5883L::obtenirXYZ()
{
    this->_compass->read();
    this->_xyzRaw[0] = this->_compass->getX();
    this->_xyzRaw[1] = this->_compass->getY();
    this->_xyzRaw[2] = this->_compass->getZ();
}

float QMC5883L::getMagnetometerTiltCompensatedBearing()
{
    // 需要Acce 的 pitch 和roll ， 结合 磁力计的 xh ，yh和zh，才能计算准确
    // float r, p, mx, my, mz;
    // mx = this->_compass->getX();
    // my = this->_compass->getY();
    // mz = this->_compass->getZ();
    // Serial.println("XYZ->");
    // Serial.print(mx);
    // Serial.print(" ");
    // Serial.print(my);
    // Serial.print(" ");
    // Serial.println(mz);

    // r = this->_TSProperties->PropertiesPosture.RollAngle;  // degree
    // p = this->_TSProperties->PropertiesPosture.PitchAngle; // degree

    // Serial.println("Pitch Roll");
    // Serial.print(p);
    // Serial.print(" ");
    // Serial.print(r);
    // Serial.print(" ");

    // Convert back to radians
    // int rollRadians = (M_PI / 180) * r;
    // int pitchRadians = (M_PI / 180) * p;

    // int cx = (int)(mx * cos(p) + my * sin(r) * sin(p) - mz * cos(r) * sin(p));
    // int cy = (int)(my * cos(r) + mz * sin(r));

    // Also diffrerent here:
    // https://web.archive.org/web/20130624074336/http://www.loveelectronics.co.uk:80/Tutorials/13/tilt-compensated-compass-arduino-tutorial
    // Same as 1st!!!

    // int tx = cx, ty = cy, tz = mz; // Done offx,y above
    // float atan2val = 180 / M_PI * atan2((float)(ty), (float)(tx));

    // int tb = (int)(-atan2val + 360) % 360;

    // return tb;
}
