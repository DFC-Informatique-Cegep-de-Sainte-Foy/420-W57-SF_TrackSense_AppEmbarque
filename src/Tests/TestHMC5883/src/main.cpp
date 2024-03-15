#include <Wire.h>
#include <Arduino.h>
#include <QMC5883LCompass.h>

/*
    A bearing is an angle less than 90° within a quadrant defined by the cardinal directions.
    An azimuth is an angle between 0° and 360° measured clockwise from North.
    "South 45° East" and "135°" are the same direction expressed as a bearing and as an azimuth.

*/
QMC5883LCompass compass;

// #define CalibrationQMC
#define QMC5883L

#ifdef CalibrationQMC

void setup()
{
    Serial.begin(9600);
    compass.init();

    Serial.println("This will provide calibration settings for your QMC5883L chip. When prompted, move the magnetometer in all directions until the calibration is complete.");
    Serial.println("Calibration will begin in 5 seconds.");
    delay(5000);

    Serial.println("CALIBRATING. Keep moving your sensor...");
    compass.calibrate();

    Serial.println("DONE. Copy the lines below and paste it into your projects sketch.);");
    Serial.println();
    Serial.print("compass.setCalibrationOffsets(");
    Serial.print(compass.getCalibrationOffset(0));
    Serial.print(", ");
    Serial.print(compass.getCalibrationOffset(1));
    Serial.print(", ");
    Serial.print(compass.getCalibrationOffset(2));
    Serial.println(");");
    Serial.print("compass.setCalibrationScales(");
    Serial.print(compass.getCalibrationScale(0));
    Serial.print(", ");
    Serial.print(compass.getCalibrationScale(1));
    Serial.print(", ");
    Serial.print(compass.getCalibrationScale(2));
    Serial.println(");");
}

void loop()
{
    delay(1000);
}
#endif

#ifdef QMC5883L
void setup()
{
    Serial.begin(9600);
    // Wire.begin();
    compass.init();
    // 粘贴校准后的调整方法;
    compass.setCalibrationOffsets(651.00, -373.00, -760.00);
    compass.setCalibrationScales(1.07, 1.02, 0.92);
}

void loop()
{
    // 测量方位角度（数值在0-359度之间）,朝向
    compass.read();
    int azimuth;
    azimuth = compass.getAzimuth();

    Serial.print("Azimuth-> ");

    Serial.print(azimuth);

    Serial.println();

    delay(500);

    // 简易测量方位角度（数值在0-11之间，每个数值间隔30度）
    // byte b = compass.getBearing(azimuth); // return the a value of 0-15 360 / 16 unite =22.5°
    // Serial.print("B: ");

    // Serial.print(b);

    // Serial.println();

    // delay(250);

    // 获取三轴读数

    // int x = compass.getX();
    // int y = compass.getY();
    // int z = compass.getZ();
    // Serial.print("x-y-z  ");
    // Serial.print(x);
    // Serial.print("-");
    // Serial.print(y);
    // Serial.print("-");
    // Serial.println(z);

    // 16 directions represantant NESO
    // char myArray[3];
    // compass.getDirection(myArray, azimuth);
    // Serial.print(myArray[0]);
    // Serial.print(myArray[1]);
    // Serial.print(myArray[2]);
    // Serial.println();
}
#endif
