// #include <Wire.h>
// #include <Arduino.h>
// #include <QMC5883LCompass.h>
// #include <Modules/QMC5883L.h>

// /*
//     A bearing is an angle less than 90° within a quadrant defined by the cardinal directions.
//     An azimuth is an angle between 0° and 360° measured clockwise from North.
//     "South 45° East" and "135°" are the same direction expressed as a bearing and as an azimuth.

//     main_ADD
// */
// QMC5883L *compass;
// MPU6050 *mpu6050;
// TSProperties *ts;
// // #define CalibrationQMC
// // #define MyQMC5883L
// #define I2CAddressScan

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

// #ifdef MyQMC5883L

// void setup()
// {
//     ts = new TSProperties();
//     mpu6050 = new MPU6050(ts, 0x68);
//     compass = new QMC5883L(ts, mpu6050, 0x77);
//     Serial.begin(9600);
//     Wire.begin();
//     copy les methodes apres calibration;
//     first calibration 03-14
//     compass.setCalibrationOffsets(651.00, -373.00, -760.00);
//     compass.setCalibrationScales(1.07, 1.02, 0.92);
//     // second calibration 03-15
//     compass.setCalibrationOffsets(623.00, -349.00, -899.00);
//     compass.setCalibrationScales(1.14, 1.16, 0.79);
//     thrid calibration 03-19
//     compass.setCalibrationOffsets(-107.00, -397.00, -1724.00);
//     compass.setCalibrationScales(1.80, 1.64, 0.55);
// }

// void loop()
// {
//     // 测量方位角度（数值在0-359度之间）,朝向
//     // 先读取数据
//     compass->obtenirXYZ();
//     int azimuth;
//     azimuth = compass->obtenirAzimuth();

//     Serial.print("Azimuth-> ");

//     Serial.print(azimuth);

//     Serial.println();

//     delay(500);

//     // 简易测量方位角度（数值在0-11之间，每个数值间隔30度）
//     byte b = compass->obtenirBearing(); // return the a value of 0-15 360 / 16 unite =22.5°
//     // Serial.print("B: ");

//     // Serial.print(b);

//     // Serial.println();

//     // delay(250);

//     // 获取三轴读数

//     // int x = compass.getX();
//     // int y = compass.getY();
//     // int z = compass.getZ();
//     // Serial.print("x-y-z  ");
//     // Serial.print(x);
//     // Serial.print("-");
//     // Serial.print(y);
//     // Serial.print("-");
//     // Serial.println(z);

//     // 16 directions represantant NESO
//     // char myArray[3];
//     // compass.getDirection(myArray, azimuth);
//     // Serial.print(myArray[0]);
//     // Serial.print(myArray[1]);
//     // Serial.print(myArray[2]);
//     // Serial.println();
// }
// #endif

// #ifdef I2CAddressScan
// void setup()
// {
//     Serial.begin(9600);
//     Wire.begin();
//     Serial.println("\nI2C Scanner");
//     /*
//     I2C device found at address 0x68  !
//     I2C device found at address 0x77  !
//     0x0D
//     */
// }

// void loop()
// {
//     byte error, address;
//     int devices = 0;

//     Serial.println("Scanning...");

//     for (address = 1; address < 127; address++)
//     {
//         // The i2c_scanner uses the return value of
//         // the Write.endTransmisstion to see if
//         // a device did acknowledge to the address.
//         Wire.beginTransmission(address);
//         error = Wire.endTransmission();

//         if (error == 0)
//         {
//             Serial.print("I2C device found at address 0x");
//             if (address < 16)
//                 Serial.print("0");
//             Serial.print(address, HEX);
//             Serial.println("  !");
//             devices++;
//         }
//         else if (error == 4)
//         {
//             Serial.print("Unknown error at address 0x");
//             if (address < 16)
//                 Serial.print("0");
//             Serial.println(address, HEX);
//         }
//     }
//     if (devices == 0)
//         Serial.println("No I2C devices found\n");
//     else
//         Serial.println("done\n");

//     delay(5000); // wait 5 seconds for next scan
// }
// #endif
