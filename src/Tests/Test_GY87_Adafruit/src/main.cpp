
#include <Arduino.h>
#include <Wire.h>
#include "Program.h"

// GY87_Adafruit *mygy;
Program *program = nullptr;
void loopCore0(void *pvParameters);

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

  // Test
  ledcSetup(LEDC_CHANNEL_0, LEDC_FREQ, LEDC_RESOLUTION); // configure LED PWM functionalitites
  ledcAttachPin(PIN_LED, LEDC_CHANNEL_0);                // attach the channel to the GPIO to be controlled

  xTaskCreatePinnedToCore(
      loopCore0,
      "loopCore0", // Name of the task
      5000,        // Stack size in words // À vérifier !!!
      NULL,        // Task input parameter
      1,           // Priority of the task
      NULL,        // Task handle.
      0            // Core where the task should run
  );
}

void loop()
{
  program->execute();

  // mygy->read();
}

void loopCore0(void *pvParameters)
{
  while (true)
  {
#if DEBUG_TS_CORE == 1
    Serial.printf("Stack High Water Mark = %d.......", uxTaskGetStackHighWaterMark(NULL));
    Serial.println();
    int optimumStackSize = (5000 - uxTaskGetStackHighWaterMark(NULL)) + 2000;
    Serial.printf("Optimum Stack Size = %d.......", optimumStackSize);
    Serial.println();
#endif
    program->executeCore0();

    /*
        REALLY IMPORTANT :
        Some Arduino low code is running on core 0 by default. If a Task is running on core 0 without pause,
        it take 100% of calculation time and let no time for Arduino low code to run. The Application will crash at random moment...
        So, we need to pause the task to let some time for Arduino low code to run.
        vTaskDelay(10 / portTICK_PERIOD_MS) is the minimum value to let Arduino low code to run.
    */
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
