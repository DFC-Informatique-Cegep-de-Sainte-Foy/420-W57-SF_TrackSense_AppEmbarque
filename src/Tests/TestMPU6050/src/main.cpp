#include <Arduino.h>
#include "Program.h"

Program *program = nullptr;

void setup()
{
  Serial.end();
  // Serial.begin(115200);
  Serial.begin(9600);
  program = new Program();
}

void loop()
{
  program->execute();
}