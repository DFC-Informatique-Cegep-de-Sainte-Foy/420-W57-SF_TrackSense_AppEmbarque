
#include <Arduino.h>
#include <Wire.h>
#include "Program.h"

// GY87_Adafruit *mygy;
Program *program = nullptr;

void setup()
{
  // Wire.begin();
  Serial.begin(9600);
  // mygy = new GY87_Adafruit();
  program = new Program();
  // mygy->calibrationHMC5883L(); // 获得校准参数
  // 将校准参数保存在File中，不用每次都校准
  // 应用Quebec磁偏角
  // mygy->setMagneticDeclination(-15, 5);
}

void loop()
{
  program->execute();

  // mygy->read();
}
