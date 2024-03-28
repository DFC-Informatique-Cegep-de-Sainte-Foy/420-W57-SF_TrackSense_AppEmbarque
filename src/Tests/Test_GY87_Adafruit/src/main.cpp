#include <Wire.h>
#include <GY87_Adafruit.h>

GY87_Adafruit *mygy;
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  mygy = new GY87_Adafruit();
}

void loop()
{
  mygy->_read();
}
