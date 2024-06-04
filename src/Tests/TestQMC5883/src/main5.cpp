// #include <Wire.h>
// #include <HMC5883L.h>

// HMC5883L *compass = nullptr;
// // container
// int16_t mx, my, mz;
// float Mxyz[3];
// void setup()
// {
//     Serial.begin(9600);
//     Wire.begin();
//     compass = new HMC5883L(0x0D);
//     if (!compass->testConnection())
//     {
//         Serial.println("Compass n'est pas trouvé!");
//     }
//     compass->setDataRate(HMC5883L_RATE_15);
//     // compass->setGain(HMC5883L_GAIN_1_3);
//     // compass->setMeasurementBias(0);
//     compass->setMode(HMC5883L_MODE_CONTINUOUS);
//     // compass->setSampleAveraging(3);

//     compass->initialize();

//     Serial.print("SampleAveraging->");
//     Serial.println(compass->getSampleAveraging());

//     Serial.print("DataRate->");
//     Serial.println(compass->getDataRate());

//     Serial.print("MeasurementBias->");
//     Serial.println(compass->getMeasurementBias());

//     Serial.print("Gain->");
//     Serial.println(compass->getGain());

//     Serial.print("Mode->");
//     Serial.println(compass->getMode());

//     Serial.print("Ready->");
//     Serial.println(compass->getReadyStatus());

//     Serial.print("Ready->");
//     Serial.println(compass->getReadyStatus());
// }

// void loop()
// {

//     // 读取磁力计数据
//     compass->getHeading(&mx, &my, &mz);
//     // Mxyz[0] = compass->getHeadingX();
//     // Mxyz[1] = compass->getHeadingY();
//     // Mxyz[2] = compass->getHeadingZ();

//     Serial.print("x-y-z ");
//     Serial.print(mx);
//     Serial.print("-");
//     Serial.print(my);
//     Serial.print("-");
//     Serial.print(mz);

//     // 计算方位角度
//     float heading = atan2(Mxyz[1], Mxyz[0]);

//     // 将弧度转换为度
//     heading = heading * 180 / M_PI;

//     // 调整方位角度范围为0-360度
//     if (heading < 0)
//     {
//         heading += 360;
//     }

//     // 输出方位角度
//     Serial.print(" Heading: ");
//     Serial.println(heading);

//     delay(500);
// }
