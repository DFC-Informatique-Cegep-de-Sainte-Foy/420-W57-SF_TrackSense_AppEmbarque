// #include <Wire.h>
// #include <Arduino.h>
// #include <QMC5883LCompass.h>

// /*
//     A bearing is an angle less than 90° within a quadrant defined by the cardinal directions.
//     An azimuth is an angle between 0° and 360° measured clockwise from North.
//     "South 45° East" and "135°" are the same direction expressed as a bearing and as an azimuth.

// */
// // lib main_mprograms

// // #define CalibrationQMC
// #define QMC5883L
// // #define HMC5883

// #ifdef CalibrationQMC

// void setup()
// {
//     Serial.begin(9600);
//     compass.init();

//     Serial.println("This will provide calibration settings for your QMC5883L chip. When prompted, move the magnetometer in all directions until the calibration is complete.");
//     Serial.println("Calibration will begin in 5 seconds.");
//     delay(5000);

//     Serial.println("CALIBRATING. Keep moving your sensor...");
//     compass.calibrate();

//     Serial.println("DONE. Copy the lines below and paste it into your projects sketch.);");
//     Serial.println();
//     Serial.print("compass.setCalibrationOffsets(");
//     Serial.print(compass.getCalibrationOffset(0));
//     Serial.print(", ");
//     Serial.print(compass.getCalibrationOffset(1));
//     Serial.print(", ");
//     Serial.print(compass.getCalibrationOffset(2));
//     Serial.println(");");
//     Serial.print("compass.setCalibrationScales(");
//     Serial.print(compass.getCalibrationScale(0));
//     Serial.print(", ");
//     Serial.print(compass.getCalibrationScale(1));
//     Serial.print(", ");
//     Serial.print(compass.getCalibrationScale(2));
//     Serial.println(");");
// }

// void loop()
// {
//     delay(1000);
// }
// #endif

// #ifdef QMC5883L

// QMC5883LCompass compass;
// void setup()
// {
//     Serial.begin(9600);
//     Wire.begin();
//     // GY87 板载的HMC5883L的 地址不是 0X77 也不是0X1E
//     compass.setADDR(0x1E);
//     // compass.setADDR(0x0D);
//     compass.init();
//     compass.setMagneticDeclination(-15, 5);

//     // copy les methodes apres calibration;
//     // first calibration 03-14
//     // compass.setCalibrationOffsets(651.00, -373.00, -760.00);
//     // compass.setCalibrationScales(1.07, 1.02, 0.92);
//     // // second calibration 03-15
//     // compass.setCalibrationOffsets(623.00, -349.00, -899.00);
//     // compass.setCalibrationScales(1.14, 1.16, 0.79);
//     // thrid calibration 03-19
//     compass.setCalibrationOffsets(-107.00, -397.00, -1724.00);
//     compass.setCalibrationScales(1.80, 1.64, 0.55);
// }

// void loop()
// {
//     // 测量方位角度（数值在0-359度之间）,朝向
//     // 先读取数据
//     compass.read();
//     int azimuth;
//     azimuth = compass.getAzimuth();

//     Serial.print("Azimuth-> ");

//     Serial.print(azimuth);

//     Serial.println();

//     // delay(500);

//     // 简易测量方位角度（数值在0-11之间，每个数值间隔30度）
//     byte b = compass.getBearing(azimuth); // return the a value of 0-15 360 / 16 unite =22.5°
//     Serial.print("B: ");

//     Serial.print(b);

//     Serial.println();

//     // delay(500);

//     // 获取三轴读数

//     int x = compass.getX();
//     int y = compass.getY();
//     int z = compass.getZ();
//     Serial.print("x-y-z  ");
//     Serial.print(x);
//     Serial.print("-");
//     Serial.print(y);
//     Serial.print("-");
//     Serial.println(z);

//     delay(500);

//     // 16 directions represantant NESO
//     // char myArray[3];
//     // compass.getDirection(myArray, azimuth);
//     // Serial.print(myArray[0]);
//     // Serial.print(myArray[1]);
//     // Serial.print(myArray[2]);
//     // Serial.println();
// }
// #endif

// #ifdef HMC5883
// /*
// An Arduino code example for interfacing with the HMC5883 3-Axis Digital Compass

// by: Jordan McConnell
//  SparkFun Electronics
//  created on: 6/30/11
//  license: OSHW 1.0, http://freedomdefined.org/OSHW

// Analog input 4 I2C SDA
// Analog input 5 I2C SCL
// */

// /*
//     结果 ：
//         From(): i2cRead returned Error 263
//         x: 1073486420  y: 1073486672  z: 0
//         [ 28573][E][Wire.cpp:513] requestFrom(): i2cRead returned Error 263
// */

// #include <Wire.h> //I2C Arduino Library

// #define address 0x1E // 0011110b, I2C 7bit address of HMC5883

// int aX, aY, aZ;

// void setup()
// {
//     // Initialize Serial and I2C communications
//     Serial.begin(9600);
//     Wire.begin();

//     // Put the HMC5883 IC into the correct operating mode
//     Wire.beginTransmission(address); // open communication with HMC5883
//     Wire.write(0x02);                // select mode register
//     Wire.write(0x00);                // continuous measurement mode
//     Wire.endTransmission();
// }

// void loop()
// {

//     int x, y, z; // triple axis data

//     // Tell the HMC5883 where to begin reading data
//     Wire.beginTransmission(address);
//     Wire.write(0x03); // select register 3, X MSB register
//     Wire.endTransmission();

//     // Read data from each axis, 2 registers per axis
//     Wire.requestFrom(address, 6);
//     if (6 <= Wire.available())
//     {
//         x = Wire.read() << 8; // X msb
//         x |= Wire.read();     // X lsb
//         z = Wire.read() << 8; // Z msb
//         z |= Wire.read();     // Z lsb
//         y = Wire.read() << 8; // Y msb
//         y |= Wire.read();     // Y lsb
//     }

//     aX = (aX + x) / 2;
//     aY = (aY + y) / 2;
//     aZ = (aZ + z) / 2;

//     // Print out values of each axis
//     Serial.print("x: ");
//     Serial.print(aX);
//     Serial.print("  y: ");
//     Serial.print(aY);
//     Serial.print("  z: ");
//     Serial.println(aZ);

//     delay(250);
// }

// #endif