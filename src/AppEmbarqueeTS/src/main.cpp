// #define Address_detect
#define TrackSense

#ifdef Address_detect
#include <Adafruit_I2CDevice.h>

Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(0x68);

void setup()
{
    // while (!Serial)
    // {
    //     delay(10);
    // }
    Serial.begin(115200);
    Serial.println("I2C address detection test");

    if (!i2c_dev.begin())
    {
        Serial.print("Did not find device at 0x");
        Serial.println(i2c_dev.address(), HEX);
        while (1)
            ;
    }
    Serial.print("Device found on address 0x");
    Serial.println(i2c_dev.address(), HEX);
}

void loop()
{
}
#endif

#ifdef TrackSense

#include <Arduino.h>
#include "Program.h"
#include <Modules/CST816S.h>

void loopCore0(void *pvParameters); // forward declaration of the loopCore0 function
Program *program = nullptr;

// CST816S touch(21, 22, 33, 34); // sda(21), scl(22), rst(33), irq(34)  works

void setup()
{
    Serial.end();
    Serial.begin(115200);
    // Test touch
    // touch.begin();
    program = new Program();

    //???
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

#if DEBUG_TS_CORE
    Serial.println("Debug mode");

    Serial.printf("Flash Speed = %d Flash mode = %d", ESP.getFlashChipSpeed(), (int)ESP.getFlashChipMode());
    Serial.println();

    Serial.printf("Chip Cores = %d", ESP.getChipCores());
    Serial.println();

    Serial.printf("Chip Model = %s", ESP.getChipModel());
    Serial.println();

    Serial.printf("Chip Revision = %d", ESP.getChipRevision());
    Serial.println();

    Serial.printf("CPU Frequency = %d", ESP.getCpuFreqMHz());
    Serial.println();

    Serial.printf("Heap Size = %d", ESP.getHeapSize());
    Serial.println();

    Serial.printf("Free Heap = %d", ESP.getFreeHeap());
    Serial.println();

    Serial.printf("Sketch Size = %d", ESP.getSketchSize());
    Serial.println();

    Serial.printf("Free Sketch Space = %d", ESP.getFreeSketchSpace());
    Serial.println();

    Serial.printf("Flash Chip Size = %d", ESP.getFlashChipSize());
    Serial.println();

    Serial.printf("Cycle Count = %d", ESP.getCycleCount());
    Serial.println();

    Serial.printf("PSRAM Size = %d", ESP.getPsramSize());
    Serial.println();

    Serial.printf("Max Alloc Heap = %d", ESP.getMaxAllocHeap());
    Serial.println();

    Serial.printf("Max Alloc PSRAM = %d", ESP.getMaxAllocPsram());
    Serial.println();

    Serial.printf("SDK Version = %s", ESP.getSdkVersion());
    Serial.println();

    Serial.printf("Sketch Size = %d", ESP.getSketchSize());
    Serial.println();

    ESP.magicFlashChipMode(0);
    delay(1000);
    Serial.printf("Flash Speed = %d Flash mode = %d", ESP.getFlashChipSpeed(), ESP.getFlashChipMode());
    Serial.println();

    // int optimumStackSize = (5000 - uxTaskGetStackHighWaterMark(NULL)) + 2000;
    // Serial.printf("Optimum Stack Size = %d", optimumStackSize);
    // Serial.println();
#endif
}

void loop()
{
    program->executeCore1();
    // test touch
    // if (touch.available())
    // {
    //     Serial.print(touch.gesture());
    //     Serial.print("\t");
    //     Serial.print(touch.data.points);
    //     Serial.print("\t");
    //     Serial.print(touch.data.event);
    //     Serial.print("\t");
    //     Serial.print(touch.data.x);
    //     Serial.print("\t");
    //     Serial.println(touch.data.y);
    // }
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

#endif