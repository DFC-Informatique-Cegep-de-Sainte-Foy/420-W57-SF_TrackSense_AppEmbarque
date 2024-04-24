#include <Arduino.h>

#define DrawBitMap
// #define TouchTest

#ifdef DrawBitMap
/*******************************************************************************
 * Dependent libraries:
 * - JPEGDEC
 * - CST816S
 * - Arduino_GFX_Library
 *
 * Download using library manager
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload JPEG file
 *   SPIFFS (ESP32):
 *     upload SPIFFS data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *
 *
 * LCD <=>  ESP32   SPI
 * IOVCC    - 3.3V
 * VCC28    - 3.3V
 * GND      - GND
 * MOSI     - 23
 * SCLK     - 18
 * RST      - 4
 * CS       - 15
 * A0       - 2   //DC pin
 * TE       - Not Connected
 * LEDA     - 3.3V  //backlight +
 * LEDK     - GND   //backlight - (use an N-channel mosfet to control the backlight with any pin)
 *
 * TOUCH <=> ESP32   IIC
 * VCC    - 3.3V
 * GND    - GND
 * TSCL   - 22
 * TSDA   - 21
 * TRST   - 33
 * TINT   - 34
 *
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#include <CST816S.h>
#include <SPIFFS.h>
#include "JpegClass.h"

CST816S touch(21, 22, 33, 34); // sda, scl, rst, irq
Arduino_DataBus *bus = new Arduino_HWSPI(0 /* DC */, 05 /* CS */);

Arduino_GFX *gfx = new Arduino_GC9A01(bus, 4, 0 /* rotation */, true /* IPS */);

static JpegClass jpegClass;

/// @brief pixel drawing callback
/// @param pDraw bitmap 2 draw
/// @return
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
    gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
    return 1;
}
int x = 0;
int y = 15;
char *images[] = {"/0.jpg", "/1.jpg", "/2.jpg", "/3.jpg", "/4.jpg", "/5.jpg", "/6.jpg", "/7.jpg",
                  "/8.jpg", "/9.jpg", "/10.jpg", "/11.jpg", "/12.jpg", "/13.jpg", "/14.jpg", "/15.jpg"};

void setup()
{
    Serial.begin(115200);
    // while (!Serial);
    Serial.println("JPEG Image Viewer");

    // Init Display
    gfx->begin();
    gfx->fillScreen(BLACK);

    touch.begin();
    if (!SPIFFS.begin())
    {
        Serial.println(F("ERROR: File System Mount Failed!"));
        gfx->println(F("ERROR: File System Mount Failed!"));
        return;
    }
    else
    {
        jpegClass.draw(&SPIFFS, (char *)images[x], jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
    }
}

void loop()
{
    if (touch.available())
    {
        if (touch.data.gestureID == SWIPE_LEFT)
        {
            x++;
            if (x > y)
            {
                x = 0;
            }
            jpegClass.draw(&SPIFFS, (char *)images[x], jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
        }
        if (touch.data.gestureID == SWIPE_RIGHT)
        {
            x--;
            if (x < 0)
            {
                x = y;
            }
            jpegClass.draw(&SPIFFS, (char *)images[x], jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
        }
        delay(100);
    }
}

#endif

#ifdef TouchTest

#include <CST816S.h>

CST816S touch(21, 22, 33, 34); // sda, scl, rst, irq

void setup()
{
    Serial.begin(9600);
    touch.begin();
    Serial.print(touch.data.version);
    Serial.print("\t");
    Serial.print(touch.data.versionInfo[0]);
    Serial.print("-");
    Serial.print(touch.data.versionInfo[1]);
    Serial.print("-");
    Serial.println(touch.data.versionInfo[2]);
}

void loop()
{

    if (touch.available())
    {
        Serial.print(touch.gesture());
        Serial.print("\t");
        Serial.print(touch.data.points);
        Serial.print("\t");
        Serial.print(touch.data.event);
        Serial.print("\t");
        Serial.print(touch.data.x);
        Serial.print("\t");
        Serial.println(touch.data.y);
    }
}

#endif
