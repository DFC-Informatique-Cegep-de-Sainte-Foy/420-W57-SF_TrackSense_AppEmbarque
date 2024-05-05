#pragma once
#include <Arduino.h>
#include "Interfaces/IScreen.h"
#include "Configurations.h"
#include "TSProperties.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

class ScreenGC9A01 : public IScreen
{
private:
    TSProperties *_TSProperties;

    uint16_t _lastBuffer;

    bool status = false;
    uint16_t
    calculateScreenBuffer();

    float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // saved H, M, S x & y multipliers
    float ox = 0, oy = 0, px = 0, py = 0, qx = 0, qy = 0, rx = 0, ry = 0;

    float sdeg = 0, mdeg = 0, hdeg = 0;
    uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // saved H, M, S x & y coords
    uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
    float lastDegree = -1;
#define DEG2RAD 0.0174532925
#define RAD2DEG 180.0 / 3.141592653589793238463
// some extra colors
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFBE0
#define GREY 0x84B5
#define BORDEAUX 0xA000
public:
    Adafruit_GC9A01A *tft;
    GFXcanvas16 *canvas;
    float D1x = 0, D1y = 0, D2x = 0, D2y = 0, D3x = 0, D3y = 0;
    float directionNordx1 = 0, directionNordy1 = 0, directionNordx2 = 0, directionNordy2 = 0, directionNordx3 = 0, directionNordy3 = 0, directionNordEndX = 0, directionNordEndY = 0;
    ScreenGC9A01(TSProperties *TSProperties);
    ~ScreenGC9A01();

    /* Tests */
    void testButtonsScreen() override;
    void testGPS() override;

    /* Elements */
    void drawLogoTS() override;
    void drawBattery(int16_t coordX, int16_t coordY, int16_t largeurX, uint pourcentage) override;
    void drawIsRideStarted(int16_t coordX, int16_t coordY, int16_t largeurX) override;
    void drawError() override;
    void drawIsGPSValid(int16_t coordX, int16_t coordY, int16_t largeurX) override;
    void drawStatistics(String title, String value, String unit, int16_t titleCoordX, int16_t valueCoordX, int16_t unitCoordX, int16_t coordY) override;
    void drawCompass(float degree);
    void drawBoutonTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3) override;
    /* Calculations */
    int calculateXCoordTextToCenter(String text) override;
    int calculateXCoordItemToCenter(uint16_t lengthInPixels) override;

    /* Drawing tools */
    void drawOnScreen() override;
    void drawBackgroundColor(uint16_t darkModeColor = TFT_DARK_MODE_BACKGROUND_COLOR, uint16_t lightModeColor = TFT_LIGHT_MODE_BACKGROUND_COLOR) override;
    void setRotation(u_int8_t rotation) override;
    void setFont(uint id) override;
    void printText(String text, int16_t coordX, int16_t coordY) override;
    void setTextSize(uint8_t size) override;
    void setTextWrap(boolean wrap) override;
    void setTextColor(uint16_t textDarkModeColor = TFT_DARK_MODE_TEXT_COLOR,
                      uint16_t backgroundDarkModeColor = TFT_DARK_MODE_BACKGROUND_COLOR,
                      uint16_t textLightModeColor = TFT_LIGHT_MODE_TEXT_COLOR,
                      uint16_t backgroundLightModeColor = TFT_LIGHT_MODE_BACKGROUND_COLOR) override;

    void drawRect(int16_t x, int16_t y, int16_t width, int16_t height,
                  uint16_t darkModeColor = TFT_DARK_MODE_TEXT_COLOR,
                  uint16_t lightModeColor = TFT_LIGHT_MODE_TEXT_COLOR) override;

    void drawFillRect(int16_t x, int16_t y, int16_t width, int16_t height,
                      uint16_t darkModeColor = TFT_DARK_MODE_TEXT_COLOR,
                      uint16_t lightModeColor = TFT_LIGHT_MODE_TEXT_COLOR) override;
    void Draw_green_ticks_bevels();
    void Draw_points_azimuths();
    void Draw_Destination(float);
    void Draw_Compass(float);
    void Draw_Cadran_Compass();
    void cleanNeedleCompass();

    void drawFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                          int16_t x2, int16_t y2, uint16_t color);

    void drawGoHomePage();
    float calculerDirectionDegree(float p_longitude_destination, float p_latitude_destination);
    void Draw_FlecheNord(float north);
    void Draw_Distance();
};
