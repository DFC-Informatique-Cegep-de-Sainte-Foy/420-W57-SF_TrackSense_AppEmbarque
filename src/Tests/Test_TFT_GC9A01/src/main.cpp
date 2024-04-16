
// #define spiral   //works greate
// #define humidity_meter // works greate
// #define chart_mono // works greate
// #define chart_rainbow // works greate
// #define tachometer // works greate    类似于跑步用秒表，可以作为参考
// #define voltage // works greate
// #define myclock // works greate          一个表盘，有指针每秒移动，可以作为模版
#define myCompass

#ifdef spiral

#include <Arduino.h>
// GCA901_Nano_Bodmer_spiral
//
// microcontroller: Arduino Nano
// 1.28 inch circular SPI TFT GC9A01 controller - 240*240 pixels
//
// this sketch implements Bodmer's 'rainbow spiral'
// drawn from TFT_eSPI library examples
// TFT_eSPI by Bodmer
//
// Floris Wouterlood
// September 1, 2023
// public domain

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC 0
#define TFT_CS 5

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

#define DEG2RAD 0.0174532925
#define GREY 0x2108

int segment = 0;
unsigned int col = 0;
int delta = -1;

byte red = 31;  // red is the top 5 bits of a 16 bit colour value
byte green = 0; // green is the middle 6 bits
byte blue = 0;  // blue is the bottom 5 bits
byte state = 0;
unsigned int rainbow(byte value);
unsigned int brightness(unsigned int colour, int brightness);
void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
void setup(void)
{

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(GREY);
}

void loop()
{

    fillArc(120, 120, segment * 6, 1, 120 - segment / 4, 120 - segment / 4, 3, rainbow(col));

    segment += delta;
    col += 1;
    if (col > 191)
        col = 0;
    if (segment < 0)
        delta = 1;
    if (segment > 298)
        delta = -1; // ~5 turns in the spiral (300*6 degrees)
                    // delay (5);                                                                     // slow drawing down
}

// #########################################################################
// Draw an arc with a defined thickness (modified to aid drawing spirals)  #
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same an arc of a circle is drawn

void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
{

    // make the segment size 7 degrees to prevent gaps when drawing spirals
    byte seg = 7; // Angle a single segment subtends (made more than 6 deg. for spiral drawing)
    byte inc = 6; // Draw segments every 6 degrees

    // draw colour blocks every inc degrees
    for (int i = start_angle; i < start_angle + seg * seg_count; i += inc)
    {
        float sx = cos((i - 90) * DEG2RAD); // calculate pair of coordinates for segment start
        float sy = sin((i - 90) * DEG2RAD);
        uint16_t x0 = sx * (rx - w) + x;
        uint16_t y0 = sy * (ry - w) + y;
        uint16_t x1 = sx * rx + x;
        uint16_t y1 = sy * ry + y;

        float sx2 = cos((i + seg - 90) * DEG2RAD); // calculate pair of coordinates for segment end
        float sy2 = sin((i + seg - 90) * DEG2RAD);
        int x2 = sx2 * (rx - w) + x;
        int y2 = sy2 * (ry - w) + y;
        int x3 = sx2 * rx + x;
        int y3 = sy2 * ry + y;

        tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
        tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
    }
}

// #########################################################################
// Return a 16 bit colour with brightness 0 - 100%                         #
// #########################################################################

unsigned int brightness(unsigned int colour, int brightness)
{

    byte red = colour >> 11;
    byte green = (colour & 0x7E0) >> 5;
    byte blue = colour & 0x1F;

    blue = (blue * brightness) / 100;
    green = (green * brightness) / 100;
    red = (red * brightness) / 100;

    return (red << 11) + (green << 5) + blue;
}

// #########################################################################
// Return a 16 bit rainbow colour                                          #
// #########################################################################

unsigned int rainbow(byte value)
{
    // value is expected to be in range 0-127
    // the value is converted to a spectrum colour from 0 = blue through to 127 = red

    switch (state)
    {
    case 0:
        green++;
        if (green == 64)
        {
            green = 63;
            state = 1;
        }
        break;
    case 1:
        red--;
        if (red == 255)
        {
            red = 0;
            state = 2;
        }
        break;
    case 2:
        blue++;
        if (blue == 32)
        {
            blue = 31;
            state = 3;
        }
        break;
    case 3:
        green--;
        if (green == 255)
        {
            green = 0;
            state = 4;
        }
        break;
    case 4:
        red++;
        if (red == 32)
        {
            red = 31;
            state = 5;
        }
        break;
    case 5:
        blue--;
        if (blue == 255)
        {
            blue = 0;
            state = 0;
        }
        break;
    }
    return red << 11 | green << 5 | blue;
}

#endif

#ifdef humidity_meter
// GC9A01_Nano_humidity_meter
//
// governs an 'analog' meter VU meter style on color scale
// microcontroller: Arduino Nano
// display 240*240 circular SPI 3.3V TFT with GC9A01 controller
//
// note: for demonstration purposes I have inserted a random function providing pseudo-humidity
// GC9A01       Nano
//  RST -------- NC
//  CST -------- 10
//  DC ---------  9
//  SDA -------- 11 - green wire
//  SCL -------- 13 - yellow wire
//
// inspired by analog meter example created by Bodmer in his TFT_eSPI library
// needle rotary point (pivot) coordinates are (120,160)
//
// by Floris Wouterlood
// September 1, 2023
// public domain

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC 0
#define TFT_CS 5 // pin of your choice

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

// RGB 565 color picker at https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html

#define WHITE 0xFFFF // some principal color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GREY 0x2108
#define ORANGE 0xFBE0
#define TEXT_COLOR 0xFFFF
#define AFRICA 0xAB21
#define BORDEAUX 0xA000
#define VOLTMETER 0xF6D3

#define DEG2RAD 0.0174532925

int x, y;
float humidity;
float angle_circle;
// radius pivot to ticks
int k;
float scale_x_out, scale_y_out, scale_x_out_old, scale_y_out_old;
float scale_x_in, scale_y_in, scale_x_in_old, scale_y_in_old;
float n1_x, n1_y, n2_x, n2_y, n3_x, n3_y, n4_x, n4_y;
float n1_x_old, n1_y_old, n2_x_old, n2_y_old, n3_x_old, n3_y_old, n4_x_old, n4_y_old;
float needleAngle;
float outArc_x, outArc_y;
float outArc_x_old, outArc_y_old;
int tickLength = 15;
int radius = 40;

void drawTicks();
void makeMulticolorScale();
void subArc();
void intermediateArc();
void drawNeedle();
void numericModule();
void setup(void)
{

    randomSeed(analogRead(0));
    Serial.begin(9600);
    tft.begin(); // initialize the CG9A01 chip, 240x240 pixels
    tft.setRotation(0);
    tft.fillScreen(BLACK);

    tft.drawRoundRect(80, 195, 80, 40, 4, CYAN); // secondary frame under meter panel frame

    x = 120;
    y = 160; // pivot coords
    drawTicks();
    makeMulticolorScale(); // create the multicolored scale under the ticks
    x = 120;
    y = 160;
    subArc();

    humidity = 20; // draw needle once on fancy humidity - to fill buffers
    x = 120;       // pivot coordinates
    y = 160;       // pivot coordinates
    n1_x_old = 120;
    n1_y_old = 160; // draw needle once on fancy humidity - to fill buffers
    n2_x_old = 120;
    n2_y_old = 160;
    n3_x_old = 120;
    n3_y_old = 160;
    n4_x_old = 120;
    n4_y_old = 160;
    intermediateArc();
    drawNeedle();
    numericModule();

    tft.setCursor(135, 210);
    tft.setTextSize(2);
    tft.setTextColor(WHITE, BLACK);
    tft.print("%");

    delay(100);
}

void loop()
{

    humidity = random(20, 85);
    Serial.print("humidity: ");
    Serial.print(humidity, 0);
    Serial.println(" %");

    drawNeedle();      // execute the needle drawing function
    intermediateArc(); // restore integrity of the intermediate arc
    numericModule();
    delay(1000);
}

void drawTicks()
{

    k = 216; // first tick
    do
    {
        float angle_circle = (k * DEG2RAD);                                         // angle expressed in radians - 1 degree = 0,01745331 radians
        float edge_x1 = (x + ((radius + 84) * cos(angle_circle)));                  // x coordinate tick, inner
        float edge_y1 = (y + ((radius + 84) * sin(angle_circle)));                  // y coordinate tick, inner
        float edge_x1_out = (x + ((radius + 80 + tickLength) * cos(angle_circle))); // x coordinate tick, outer
        float edge_y1_out = (y + ((radius + 80 + tickLength) * sin(angle_circle))); // y coordinate tick, outer
        tft.drawLine(edge_x1, edge_y1, edge_x1_out, edge_y1_out, MAGENTA);          // draw tick
        k = k + 3;
    } while (k < 330);
}

void subArc()
{ // sub arc is two thin  arcs under the green-yellow-orange scale

    k = 216;
    for (k = 216; k < 330; k++)
    {
        angle_circle = (k * DEG2RAD);
        outArc_x = (x + (radius + 69) * cos(angle_circle));
        outArc_y = (y + (radius + 69) * sin(angle_circle));
        outArc_x_old = outArc_x;
        outArc_y_old = outArc_y;
        tft.drawLine(outArc_x_old, outArc_y_old, outArc_x, outArc_y, CYAN);
    }

    for (k = 216; k < 330; k++)
    {
        angle_circle = (k * DEG2RAD);
        outArc_x = (x + (radius + 68) * cos(angle_circle));
        outArc_y = (y + (radius + 68) * sin(angle_circle));
        outArc_x_old = outArc_x;
        outArc_y_old = outArc_y;
        tft.drawLine(outArc_x_old, outArc_y_old, outArc_x, outArc_y, CYAN);
    }
}

void intermediateArc()
{

    k = 216;
    for (k = 216; k < 330; k++)
    {
        angle_circle = (k * DEG2RAD);
        outArc_x = (x + (radius + 40) * cos(angle_circle));
        outArc_y = (y + (radius + 40) * sin(angle_circle));
        outArc_x_old = outArc_x;
        outArc_y_old = outArc_y;
        tft.drawLine(outArc_x_old, outArc_y_old, outArc_x, outArc_y, CYAN);
    }
}

void makeMulticolorScale()
{ // draw the green-yellow-orange scale through triangles arranged on an arc

    k = 216;
    angle_circle = (k * DEG2RAD);                                // angle expressed in radians - 1 degree = 0,01745331 radians
    scale_x_out_old = (x + ((radius + 80) * cos(angle_circle))); // outer points coordinates
    scale_y_out_old = (y + ((radius + 80) * sin(angle_circle)));
    scale_x_in_old = (x + ((radius + 80) * cos(angle_circle))); // inner points coordinates
    scale_y_in_old = (y + ((radius + 80) * sin(angle_circle)));

    do
    {
        angle_circle = (k * DEG2RAD);                            // angle expressed in radians - 1 degree = 0,01745331 radians
        scale_x_out = (x + ((radius + 80) * cos(angle_circle))); // outer points coordinates
        scale_y_out = (y + ((radius + 80) * sin(angle_circle)));
        scale_x_in = (x + ((radius + 72) * cos(angle_circle))); // inner points coordinates
        scale_y_in = (y + ((radius + 72) * sin(angle_circle)));

        if ((k > 128) && (k < 250)) // create green zone
        {
            tft.fillTriangle(scale_x_out_old, scale_y_out_old, scale_x_out, scale_y_out,
                             scale_x_in_old, scale_y_in_old, GREEN);
            tft.fillTriangle(scale_x_out, scale_y_out, scale_x_in_old, scale_y_in_old,
                             scale_x_in, scale_y_in, GREEN);
        }

        if ((k > 250) && (k < 300)) // create yellow zone
        {
            tft.fillTriangle(scale_x_out_old, scale_y_out_old, scale_x_out, scale_y_out,
                             scale_x_in_old, scale_y_in_old, YELLOW);
            tft.fillTriangle(scale_x_out, scale_y_out, scale_x_in_old, scale_y_in_old,
                             scale_x_in, scale_y_in, YELLOW);
        }

        if ((k > 299)) // create orange zone
        {
            tft.fillTriangle(scale_x_out_old, scale_y_out_old, scale_x_out, scale_y_out,
                             scale_x_in_old, scale_y_in_old, ORANGE);
            tft.fillTriangle(scale_x_out, scale_y_out, scale_x_in_old, scale_y_in_old,
                             scale_x_in, scale_y_in, ORANGE);
        }

        scale_x_out_old = scale_x_out;
        scale_y_out_old = scale_y_out;
        scale_x_in_old = scale_x_in;
        scale_y_in_old = scale_y_in;
        k = k + 4;
    } while (k < 330);
}

void drawNeedle()
{

    tft.drawLine(x, y, n1_x_old, n1_y_old, BLACK);                                       // remove old needle
    tft.fillTriangle(n1_x_old, n1_y_old, n2_x_old, n2_y_old, n3_x_old, n3_y_old, BLACK); // remove old needle head
    tft.drawLine(x, y, n4_x_old, n4_y_old, BLACK);                                       // remove previous needle tail

    needleAngle = (((humidity)*DEG2RAD * 1.8) - 3.14);
    n1_x = (x + ((radius + 60) * cos(needleAngle))); // needle tip point
    n1_y = (y + ((radius + 60) * sin(needleAngle)));
    n2_x = (x + ((radius + 50) * cos(needleAngle - 0.05))); // needle tip triange left point
    n2_y = (y + ((radius + 50) * sin(needleAngle - 0.05)));
    n3_x = (x + ((radius + 50) * cos(needleAngle + 0.05))); // needle tip triange left point
    n3_y = (y + ((radius + 50) * sin(needleAngle + 0.05)));
    n4_x = (x + ((radius - 20) * cos(needleAngle + 3.14))); // needle tail point
    n4_y = (y + ((radius - 20) * sin(needleAngle + 3.14)));

    tft.drawLine(x, y, n1_x, n1_y, WHITE);                       // draw new needle
    tft.fillTriangle(n1_x, n1_y, n2_x, n2_y, n3_x, n3_y, WHITE); // draw new needle arrow head
    tft.drawLine(x, y, n4_x, n4_y, WHITE);                       // draw new needle tail
    tft.fillCircle(x, y, 6, BLACK);                              // remove old pivot
    tft.fillCircle(x, y, 2, WHITE);
    tft.drawCircle(x, y, 6, WHITE); // draw new pivot

    n1_x_old = n1_x;
    n1_y_old = n1_y; // remember needle point coordinatess
    n2_x_old = n2_x;
    n2_y_old = n2_y;
    n3_x_old = n3_x;
    n3_y_old = n3_y;
    n4_x_old = n4_x;
    n4_y_old = n4_y;
}

void numericModule()
{

    tft.setCursor(105, 210);
    tft.setTextSize(2);
    tft.setTextColor(WHITE, BLACK);
    tft.print(humidity, 0);
}

#endif

#ifdef chart_mono
// GC9A01_Nano_pie_chart_mono
//
// purpose: pie chart meter with pie expanding and shrinking without flicker
// microcontroller: Arduino Nano
// display 240*240 circular SPI 3.3V TFT with GC9A01 controller
//
// the variable driving the pie chart is named pieParameter
//   GC9A01       Nano
//    RST -------- NC
//    CST -------- 10
//    DC ---------  9
//    SDA -------- 11 - green wire
//    SCL -------- 13 - yellow wire
//
// thanks to everybody who provided inspiration
// Floris Wouterlood
// September 1, 2023
// public domain

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC 0
#define TFT_CS 5

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

// some principal color definitions
// RGB 565 color picker at https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GREY 0x2108
#define SCALE0 0x2108     // color outer segment unused scale
#define SCALE1 0x2108     // color inner segment unused scale
#define TEXT_COLOR 0xFFFF // is currently white

#define DEG2RAD 0.0174532925

int x_center = 120; // center screeen coordinate
int y_center = 120; // center screeen coordinate
float x_new = 0;
float y_new = 0;
float x_old = 0;
float y_old = 0;
float pieParameter;
int iteration = 0;
int radius = 100;
float angle_new;
float angle_old;
float sliceAngle;
int flag = 0; // flag necessary to determine case - either forward or retreat
int j;

void setup()
{

    Serial.begin(9600);
    Serial.println("starting TFT display");
    tft.begin();
    tft.setRotation(0); // display in portrait
    tft.fillScreen(GREY);
    tft.drawCircle(x_center, y_center, radius + 8, CYAN); // outer margin

    x_new = 125, y_new = 20, x_old = 125, y_old = 20;
    tft.fillTriangle(x_center, y_center, x_old, y_old, x_new, y_new, GREEN); // initial triangle to fill buffers

    pieParameter = random(0, 100);
    angle_new = (360 * pieParameter / 100);

    for (j = 0; j < angle_new; j++)
    {
        sliceAngle = ((j * DEG2RAD) - (90 * DEG2RAD));
        x_old = x_new;
        y_old = y_new;
        x_new = (x_center + radius * cos(sliceAngle));
        y_new = (y_center + radius * sin(sliceAngle));
        tft.fillTriangle(x_center, y_center, x_new, y_new, x_old, y_old, GREEN);
    }

    angle_old = angle_new;
}

void loop()
{

    iteration++;
    Serial.print("iteration ");
    Serial.print(iteration);

    pieParameter = random(0, 100);
    angle_new = (360 * pieParameter / 100);
    if (angle_new > angle_old)
        flag = 1;
    if (angle_new <= angle_old)
        flag = 0;

    switch (flag)
    {

    case 0: // case 0 exists when new pieParameter is higher than old

        Serial.println(" - case flag 0 - new value lower than old value");

        for (j = angle_old; j > angle_new; j--)
        {
            sliceAngle = ((j * DEG2RAD) - (90 * DEG2RAD));
            x_old = x_new;
            y_old = y_new;
            x_new = (x_center + radius * cos(sliceAngle));
            y_new = (y_center + radius * sin(sliceAngle));
            tft.fillTriangle(x_center, y_center, x_new, y_new, x_old, y_old, GREY);
        }
        break;

    case 1:

        Serial.println(" - case flag 1 - new value higher than old value");

        for (j = angle_old; j < angle_new; j++)
        {
            sliceAngle = ((j * DEG2RAD) - (90 * DEG2RAD));
            x_old = x_new;
            y_old = y_new;
            x_new = (x_center + radius * cos(sliceAngle));
            y_new = (y_center + radius * sin(sliceAngle));
            tft.fillTriangle(x_center, y_center, x_new, y_new, x_old, y_old, GREEN);
        }
        break;
    }

    angle_old = angle_new;
    delay(2000);
}

#endif

#ifdef chart_rainbow
// GC9A01_Nano_pie_chart_rainbow
//
// purpose: pie chart meter with rainbow-colored pie that expands and shrinks without flicker
// microcontroller: Arduino Nano
// display 240*240 circular SPI 3.3V TFT with GC9A01 controller
//
// the variable driving the pie chart is named pieParameter
// GC9A01       Nano
//  RST -------- NC
//  CST -------- 10
//  DC ---------  9
//  SDA -------- 11 - green wire
//  SCL -------- 13 - yellow wire
//
// thanks to everybody who provided inspiration
// Floris Wouterlood
// September 1, 2023
// public domain

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC 0
#define TFT_CS 5

Adafruit_GC9A01A tft(TFT_CS, TFT_DC); // constructor

// some principal color definitions
// RGB 565 color picker at https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GREY 0x2108
#define TEXT_COLOR 0xFFFF // is currently white

#define DEG2RAD 0.0174532925 // degrees to radians - 360 degrees is 2*pi radians

const uint16_t lookupTable[] = {
    0x480F, // blue to red colors used in 256 colors 16 bit
    0x400F,
    0x400F,
    0x400F,
    0x4010,
    0x3810,
    0x3810,
    0x3810,
    0x3810,
    0x3010,
    0x3010,
    0x3010,
    0x2810,
    0x2810,
    0x2810,
    0x2810,
    0x2010,
    0x2010,
    0x2010,
    0x1810,
    0x1810,
    0x1811,
    0x1811,
    0x1011,
    0x1011,
    0x1011,
    0x0811,
    0x0811,
    0x0811,
    0x0011,
    0x0011,
    0x0011,
    0x0011,
    0x0011,
    0x0031,
    0x0031,
    0x0051,
    0x0072,
    0x0072,
    0x0092,
    0x00B2,
    0x00B2,
    0x00D2,
    0x00F2,
    0x00F2,
    0x0112,
    0x0132,
    0x0152,
    0x0152,
    0x0172,
    0x0192,
    0x0192,
    0x01B2,
    0x01D2,
    0x01F3,
    0x01F3,
    0x0213,
    0x0233,
    0x0253,
    0x0253,
    0x0273,
    0x0293,
    0x02B3,
    0x02D3,
    0x02D3,
    0x02F3,
    0x0313,
    0x0333,
    0x0333,
    0x0353,
    0x0373,
    0x0394,
    0x03B4,
    0x03D4,
    0x03D4,
    0x03F4,
    0x0414,
    0x0434,
    0x0454,
    0x0474,
    0x0474,
    0x0494,
    0x04B4,
    0x04D4,
    0x04F4,
    0x0514,
    0x0534,
    0x0534,
    0x0554,
    0x0554,
    0x0574,
    0x0574,
    0x0573,
    0x0573,
    0x0573,
    0x0572,
    0x0572,
    0x0572,
    0x0571,
    0x0591,
    0x0591,
    0x0590,
    0x0590,
    0x058F,
    0x058F,
    0x058F,
    0x058E,
    0x05AE,
    0x05AE,
    0x05AD,
    0x05AD,
    0x05AD,
    0x05AC,
    0x05AC,
    0x05AB,
    0x05CB,
    0x05CB,
    0x05CA,
    0x05CA,
    0x05CA,
    0x05C9,
    0x05C9,
    0x05C8,
    0x05E8,
    0x05E8,
    0x05E7,
    0x05E7,
    0x05E6,
    0x05E6,
    0x05E6,
    0x05E5,
    0x05E5,
    0x0604,
    0x0604,
    0x0604,
    0x0603,
    0x0603,
    0x0602,
    0x0602,
    0x0601,
    0x0621,
    0x0621,
    0x0620,
    0x0620,
    0x0620,
    0x0620,
    0x0E20,
    0x0E20,
    0x0E40,
    0x1640,
    0x1640,
    0x1E40,
    0x1E40,
    0x2640,
    0x2640,
    0x2E40,
    0x2E60,
    0x3660,
    0x3660,
    0x3E60,
    0x3E60,
    0x3E60,
    0x4660,
    0x4660,
    0x4E60,
    0x4E80,
    0x5680,
    0x5680,
    0x5E80,
    0x5E80,
    0x6680,
    0x6680,
    0x6E80,
    0x6EA0,
    0x76A0,
    0x76A0,
    0x7EA0,
    0x7EA0,
    0x86A0,
    0x86A0,
    0x8EA0,
    0x8EC0,
    0x96C0,
    0x96C0,
    0x9EC0,
    0x9EC0,
    0xA6C0,
    0xAEC0,
    0xAEC0,
    0xB6E0,
    0xB6E0,
    0xBEE0,
    0xBEE0,
    0xC6E0,
    0xC6E0,
    0xCEE0,
    0xCEE0,
    0xD6E0,
    0xD700,
    0xDF00,
    0xDEE0,
    0xDEC0,
    0xDEA0,
    0xDE80,
    0xDE80,
    0xE660,
    0xE640,
    0xE620,
    0xE600,
    0xE5E0,
    0xE5C0,
    0xE5A0,
    0xE580,
    0xE560,
    0xE540,
    0xE520,
    0xE500,
    0xE4E0,
    0xE4C0,
    0xE4A0,
    0xE480,
    0xE460,
    0xEC40,
    0xEC20,
    0xEC00,
    0xEBE0,
    0xEBC0,
    0xEBA0,
    0xEB80,
    0xEB60,
    0xEB40,
    0xEB20,
    0xEB00,
    0xEAE0,
    0xEAC0,
    0xEAA0,
    0xEA80,
    0xEA60,
    0xEA40,
    0xF220,
    0xF200,
    0xF1E0,
    0xF1C0,
    0xF1A0,
    0xF180,
    0xF160,
    0xF140,
    0xF100,
    0xF0E0,
    0xF0C0,
    0xF0A0,
    0xF080,
    0xF060,
    0xF040,
    0xF020,
    0xF800,
};

int x_center = 120; // center screeen coordinate
int y_center = 120; // center screeen coordinate
float x_new = 0;
float y_new = 0;
float x_old = 0;
float y_old = 0;
float pieParameter;
int radius = 100;
float angle_new;
float angle_old;
float sliceAngle;
int flag = 0; // flag necessary to determine case - either forward or retreat
int i, j;
int colorIndex;

void smallDial();
void setup()
{

    Serial.begin(9600);
    Serial.println(" - starting TFT display");
    tft.begin();
    tft.setRotation(0); // display in portrait
    tft.fillScreen(GREY);

    tft.drawCircle(x_center, y_center, radius + 10, MAGENTA); // outer margin
    tft.drawCircle(x_center, y_center, radius + 8, BLUE);     // outer margin
    tft.drawCircle(x_center, y_center, radius + 7, CYAN);     // outer margin

    x_new = 125, y_new = 20, x_old = 125, y_old = 20;
    tft.fillTriangle(x_center, y_center, x_old, y_old, x_new, y_new, GREY); // initial triangle to fill buffers

    pieParameter = random(0, 100);
    angle_new = (360 * pieParameter / 100);

    for (j = 0; j < angle_new; j++)
    {
        sliceAngle = ((j * DEG2RAD) - (90 * DEG2RAD));
        colorIndex = ((sliceAngle + (90 * DEG2RAD)) * 40.1071); // color index derived from actual angle - correction 360/256 for the 256 color lookup table
        x_old = x_new;
        y_old = y_new;
        x_new = (x_center + radius * cos(sliceAngle));
        y_new = (y_center + radius * sin(sliceAngle));
        tft.fillTriangle(x_center, y_center, x_new, y_new, x_old, y_old, (lookupTable[colorIndex]));
    }

    angle_old = angle_new;
}

void loop()
{
    delay(1000);

    pieParameter = random(0, 100);
    angle_new = (360 * pieParameter / 100);
    if (angle_new > angle_old)
        flag = 1;
    if (angle_new <= angle_old)
        flag = 0;

    switch (flag)
    {

    case 0: // case 0 exists when new pieParameter is lower than old

        Serial.println(" - flag = 0 - new value < previous");
        smallDial();
        for (j = angle_old; j > angle_new; j--)
        {
            sliceAngle = ((j * DEG2RAD) - (90 * DEG2RAD));
            x_old = x_new;
            y_old = y_new;
            x_new = (x_center + radius * cos(sliceAngle));
            y_new = (y_center + radius * sin(sliceAngle));
            tft.fillTriangle(x_center, y_center, x_new, y_new, x_old, y_old, GREY);
        }

        angle_old = angle_new;
        smallDial();
        break;

    case 1: // case 1 exists when new pieParameter is higher than old

        Serial.println(" - flag = 1 - new value > previous");
        smallDial();
        for (j = angle_old; j < angle_new; j++)
        {
            sliceAngle = ((j * DEG2RAD) - (90 * DEG2RAD));
            colorIndex = ((sliceAngle + (90 * DEG2RAD)) * 40.1071); // color index derived from actual angle - correction 360/256 for the 256 color lookup table
            if (colorIndex > 255)
                colorIndex = 255; // safety cap
            x_old = x_new;
            y_old = y_new;
            x_new = (x_center + radius * cos(sliceAngle));
            y_new = (y_center + radius * sin(sliceAngle));
            tft.fillTriangle(x_center, y_center, x_new, y_new, x_old, y_old, (lookupTable[colorIndex]));
        }

        angle_old = angle_new;
        smallDial();
        break;
    }
}

void smallDial()
{ // petit numerical dial

    tft.fillCircle(120, 40, 20, GREY);
    tft.drawCircle(120, 40, 20, CYAN);
    tft.setCursor(110, 35);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.print(pieParameter, 0);
}

#endif

#ifdef tachometer
// GC9A01_Nano_tachometer
//
// microcontroller: Arduino Nano
// display 240*240 circular SPI 3.3V TFT with GC9A01 controller
//
// purpose: simple needle meter - without flicker
//
// note: for demonstration purposes the speed accelarates to 'stopper'and then decelerates
// stopper is at 90 because speed is twice iteration here
//
// GC9A01       Nano
//  RST -------- NC
//  CST -------- 10
//  DC ---------  9
//  SDA -------- 11 - green wire
//  SCL -------- 13 - yellow wire
//
// thanks to everybody who provided inspiration
// Floris Wouterlood
// September 1, 2023
// public domain

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC 0
#define TFT_CS 5

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

// some principal color definitions
// RGB 565 color picker at https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GREY 0x2108
#define TEXT_COLOR 0xFFFF

#define DEG2RAD 0.0174532925

// note: all coordinates relative to center_x, center_y

float center_x = 120; // center x of tachometer - 240x240 TFT
float center_y = 120; // center y of tachometer - 240x240 TFT
float edge_x = 0;
float edge_y = 0;
float edge_x_old = 0;
float edge_y_old = 0;
float edge_x_out = 0;
float edge_y_out = 0;

float needletail_x = 0;
float needletail_y = 0;
float needletail_x_old = 0;
float needletail_y_old = 0;

int j;
float angle = 0;
int radius = 87;
float angle_circle = 0;
float angle_needle = 0;
int iteration = 0;
int stopper = 90;   // moment to reverse direction
int v = 1;          // needle in accerator or decelerator mode
                    // for for-next loops
float needle_x = 0; // needle pivot = center
float needle_y = 0; // needle pivot = center
float needle_x_old = 120;
float needle_y_old = 120;
int tachoSpeed = 0;
int cycleTime = 200;

void numericDial();
void serial_print_stuff();
void needle();
void drawBigDial();
void setup(void)
{

    Serial.begin(9600);
    tft.begin();
    tft.fillScreen(BLACK);
    tft.setRotation(0); // display in portrait
    tft.fillScreen(GREY);
    drawBigDial();
    needle();
    numericDial();
}

void loop()
{

    if (iteration == 0)
    {
        v = +1;
        delay(5 * cycleTime);
    }
    iteration = iteration + v;
    tachoSpeed = iteration * 2;
    serial_print_stuff();
    needle();
    numericDial();

    if (iteration == stopper)
    {
        v = -1;
        delay(5 * cycleTime);
    }

    delay(cycleTime);
}

void drawBigDial()
{

    tft.drawCircle(center_x, center_y, radius + 10, MAGENTA); // outer margin ring
    tft.drawCircle(center_x, center_y, radius + 8, BLUE);     // outer margin ring
    tft.drawCircle(center_x, center_y, radius + 7, CYAN);     // outer margin ring

    for (j = 0; j < 360; j = (j + 6)) // scale markers set 6 degrees apart
    {
        angle_circle = (j * DEG2RAD);
        edge_x = (center_x + (radius * cos(angle_circle)));
        edge_y = (center_y + (radius * sin(angle_circle)));
        edge_x_out = (center_x + ((radius + 8) * cos(angle_circle)));
        edge_y_out = (center_y + ((radius + 8) * sin(angle_circle)));
        tft.drawLine(edge_x, edge_y, edge_x_out, edge_y_out, WHITE);
    }

    for (j = 0; j < 271; j = (j + 90)) // scale markers set 6 degrees apart
    {
        angle_circle = (j * DEG2RAD);
        edge_x = (center_x + ((radius - 5) * cos(angle_circle)));
        edge_y = (center_y + ((radius - 5) * sin(angle_circle)));
        edge_x_out = (center_x + ((radius - 1) * cos(angle_circle)));
        edge_y_out = (center_y + ((radius - 1) * sin(angle_circle)));

        tft.fillCircle(edge_x, edge_y, 3, GREEN);
        tft.drawLine(edge_x, edge_y, edge_x_out, edge_y_out, GREEN);
    }

    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(center_x - 110, center_y - 5);
    tft.print("0");
    tft.setCursor(center_x - 5, center_y - 111);
    tft.print("50");
    tft.setCursor(center_x + 100, center_y - 5);
    tft.print("100");
    tft.setCursor(center_x - 10, center_y + 104);
    tft.print("150");
    tft.setCursor(center_x - 100, center_y + 50);
    tft.print("180");
    tft.fillCircle(center_x - 70, center_y + 50, 3, GREEN);
    tft.drawRoundRect((center_x - 100), (center_y + 5), 50, 22, 3, CYAN); // small numeric tacho dial
}

void needle()
{

    if (tachoSpeed > 180)
        tachoSpeed = 180; // needle limiter

    tft.drawLine(needle_x_old, needle_y_old, needletail_x_old, needletail_y_old, GREY);

    angle_needle = (((tachoSpeed)*DEG2RAD * 1.8) - 3.14); // unit of angle is radian

    needle_x = (center_x + ((radius - 10) * cos(angle_needle)));
    needle_y = (center_y + ((radius - 10) * sin(angle_needle)));
    needletail_x = (center_x - ((radius - 60) * cos(angle_needle - 6.28)));
    needletail_y = (center_x - ((radius - 60) * sin(angle_needle - 6.28)));

    needle_x_old = needle_x; // remember previous needle position
    needle_y_old = needle_y;
    needletail_x_old = needletail_x;
    needletail_y_old = needletail_y;

    tft.drawLine(needle_x, needle_y, needletail_x, needletail_y, RED);
    tft.fillCircle(center_x, center_y, 6, GREY); // restore needle pivot
    tft.drawCircle(center_x, center_y, 6, RED);  // restore needle pivot
}

void serial_print_stuff()
{

    if (tachoSpeed < 100)
        Serial.print("speed:  ");
    if (tachoSpeed >= 100)
        Serial.print("speed: ");
    Serial.print(tachoSpeed);
    Serial.println(" km/h");
}

void numericDial()
{

    tft.fillRect((center_x - 93), (center_y + 9), 40, 16, GREY);
    tft.setTextColor(YELLOW, BLACK);
    tft.setTextSize(2);

    if (tachoSpeed > 99)
    {
        tft.setCursor(center_x - 93, center_y + 9);
    }
    else
        tft.setCursor(center_x - 83, center_y + 9);

    tft.print(tachoSpeed, 1);
}

#endif

#ifdef voltage
// GCA901_Nano_voltage_meter
//
// grid voltage variation monitor (230V - 250V AC)
// rolling averaged voltage (of 10 readings) is sent to display
// NOTE: here voltage generated with random function
//
// microcontroller: Arduino Nano
// display 240*240 circular SPI 3.3V TFT with GC9A01 controller
//
// note: random function drives fluctuations of the parameter named 'volt'
// CG9A01    Arduino Nano
//  RST -------- NC
//  CST -------- 10
//  DC ---------  9
//  SDA -------- 11 - green wire
//  SCL -------- 13 - yellow wire
//
// Floris Wouterlood
// September 1, 2023
// public domain

// made for a 240*240 pixel circular display
// all x-y-coordinates relative to center = x = 120 and y = 120

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define TFT_DC 0
#define TFT_CS 5

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

#define BLACK 0x0000 // some extra colors
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
#define AFRICA 0xAB21 // current dial color

#define DEG2RAD 0.0174532925

int multiplier;
int frametime = 1000;
int x_pos;
int y_pos;
int center_x = 120; // center x of dial on 240*240 TFT display
int center_y = 120; // center y of dial on 240*240 TFT display
float pivot_x, pivot_y, pivot_x_old, pivot_y_old;
float p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, p5_x, p5_y;
float p1_x_old, p1_y_old, p2_x_old, p2_y_old, p3_x_old, p3_y_old;
float p4_x_old, p4_y_old, p5_x_old, p5_y_old;
float angleOffset = 3.14;
float arc_x;
float arc_y;
int radius = 120; // center y of circular scale
float angle_circle = 0;
float needleAngle = 0;
int iteration = 0;
int j;
float volt = 220;
int needle_multiplier = 1;
float needle_setter;

// voltage rolling averaging stuff
const byte nvalues = 10; // rolling average window size
static byte current = 0; // index for current value
static byte cvalues = 0; // count of values read (<= nvalues)
static float sum = 0;    // rolling sum
static float values[nvalues];
float averagedVoltage = 235; // to start with

float movingAverage(float value);
void displayNumerical();
void draw_pivot();
void create_dial();
void needle();

void setup()
{

    randomSeed(analogRead(0));

    tft.begin();
    Serial.begin(9600);
    Serial.println("");
    Serial.println("");
    tft.setRotation(0);

    tft.fillScreen(BLACK);
    tft.drawCircle(center_x, center_y, 120, BLACK);
    pivot_x = center_x;
    pivot_y = center_y + 50;

    p1_x_old = center_x;
    p1_y_old = center_y + 50;
    p2_x_old = center_x;
    p2_y_old = center_y + 50;
    p3_x_old = center_x;
    p3_y_old = center_y + 50;
    p4_x_old = center_x;
    p4_y_old = center_y + 50;
    p5_x_old = center_x;
    p5_y_old = center_y + 50;

    volt = 240; // initial value setting the needle
    create_dial();
    needle_setter = volt;
    needleAngle = (((needle_setter)*DEG2RAD * 1.8) - 3.14);
    needle();
    draw_pivot();
}

void loop()
{

    iteration++;
    Serial.println();
    Serial.print("iteration ");
    Serial.println(iteration);
    volt = random(230, 250); // voltage simulator
    Serial.print("volt out of smpt01B: ");
    Serial.println(volt);
    averagedVoltage = movingAverage(volt);
    Serial.print("averaged volt =      ");
    Serial.println(averagedVoltage);
    Serial.println();
    Serial.println();

    displayNumerical();
    needle_setter = averagedVoltage;
    needle();
    draw_pivot();

    delay(frametime);
}

void needle()
{ // dynamic needle management

    tft.drawLine(pivot_x, pivot_y, p1_x_old, p1_y_old, AFRICA);                           // remove old needle
    tft.fillTriangle(p1_x_old, p1_y_old, p2_x_old, p2_y_old, p3_x_old, p3_y_old, AFRICA); // remove old arrow head
    tft.fillTriangle(pivot_x, pivot_y, p4_x_old, p4_y_old, p5_x_old, p5_y_old, AFRICA);   // remove old arrow head

    needleAngle = (((needle_setter) * 0.01745331 * 1.8) - 3.14);
    p1_x = (pivot_x + ((radius)*cos(needleAngle))); // needle tip
    p1_y = (pivot_y + ((radius)*sin(needleAngle)));

    p2_x = (pivot_x + ((radius - 15) * cos(needleAngle - 0.05))); // needle triange left
    p2_y = (pivot_y + ((radius - 15) * sin(needleAngle - 0.05)));

    p3_x = (pivot_x + ((radius - 15) * cos(needleAngle + 0.05))); // needle triange right
    p3_y = (pivot_y + ((radius - 15) * sin(needleAngle + 0.05)));

    p4_x = (pivot_x + ((radius - 90) * cos(angleOffset + (needleAngle - 0.2)))); // needle triange left
    p4_y = (pivot_y + ((radius - 90) * sin(angleOffset + (needleAngle - 0.2))));

    p5_x = (pivot_x + ((radius - 90) * cos(angleOffset + (needleAngle + 0.2)))); // needle triange right
    p5_y = (pivot_y + ((radius - 90) * sin(angleOffset + (needleAngle + 0.2))));

    p1_x_old = p1_x;
    p1_y_old = p1_y; // remember previous needle position
    p2_x_old = p2_x;
    p2_y_old = p2_y;
    p3_x_old = p3_x;
    p3_y_old = p3_y;

    p4_x_old = p4_x;
    p4_y_old = p4_y; // remember previous needle counterweight position
    p5_x_old = p5_x;
    p5_y_old = p5_y;

    tft.drawLine(pivot_x, pivot_y, p1_x, p1_y, BLACK);                               // create needle
    tft.fillTriangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, BLACK);                     // create needle tip pointer
    tft.drawLine(center_x - 80, center_y + 70, center_x + 80, center_y + 70, WHITE); // repair floor
    tft.fillTriangle(pivot_x, pivot_y, p4_x, p4_y, p5_x, p5_y, BLACK);               // create needle counterweight
}

void create_dial()
{

    tft.fillCircle(center_x, center_y, 120, AFRICA); // general dial field
    tft.drawCircle(center_x, center_y, 118, GREY);
    tft.drawCircle(center_x, center_y, 117, BLACK);
    tft.drawCircle(center_x, center_y, 116, BLACK);
    tft.drawCircle(center_x, center_y, 115, GREY);

    for (j = 30; j < 75; j += 5)
    {
        needleAngle = ((j * DEG2RAD * 1.8) - 3.14);
        arc_x = (pivot_x + ((radius + 15) * cos(needleAngle))); // needle tip
        arc_y = (pivot_y + ((radius + 15) * sin(needleAngle)));
        tft.drawPixel(arc_x, arc_y, BLACK);
        tft.fillCircle(arc_x, arc_y, 2, BLACK);
    }

    tft.setTextColor(BLACK, AFRICA);
    tft.setTextSize(2);
    tft.setCursor(center_x + 15, center_y + 40);
    tft.print("V - AC");
    tft.drawLine(center_x - 80, center_y + 70, center_x + 80, center_y + 70, WHITE); // create floor
}

void draw_pivot()
{

    tft.fillCircle(pivot_x, pivot_y, 8, RED);
    tft.drawCircle(pivot_x, pivot_y, 8, BLACK);
    tft.drawCircle(pivot_x, pivot_y, 3, BLACK);
}

void displayNumerical()
{

    tft.fillRect(center_x - 82, center_y + 40, 62, 16, AFRICA);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.setCursor(center_x - 80, center_y + 40);
    tft.print(averagedVoltage, 1);
}

float movingAverage(float value)
{

    sum += value;
    if (cvalues == nvalues) // if the window is full, adjust the sum by deleting the oldest value
        sum -= values[current];

    values[current] = value; // replace the oldest with the latest

    if (++current >= nvalues)
        current = 0;

    if (cvalues < nvalues)
        cvalues += 1;

    return sum / cvalues;
}

#endif

#ifdef myclock
// internet_clock_GC9A010_ESP8266
// platform: ESP8266 Wemos d1 mini
// display: GC9A010 driven circular display 240*240 pixels
//
// clock by Bodmer - Clock example in TFT_eSPI library
//
// adapted and modified
// Floris Wouterlood
// November 1, 2022
// public domain

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

const char *ssid = "xxxxxxxxxxxxxxxx";     // network wifi credentials  - fill in your wifi network name
const char *password = "xxxxxxxxxxxxxxxx"; // network wifi credentials  - fill in your wifi key

const long utcOffsetInSeconds = 3600; // 3600 = western europe winter time - 7200 = western europe summer time
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define TFT_DC 0
#define TFT_CS 5
#define DEG2RAD 0.0174532925

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

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // saved H, M, S x & y coords
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0; // for next 1 second timeout

int hh = 0; // hours variable
int mm = 0; // minutes variable
int ss = 0; // seconds variable

// WiFiUDP ntpUDP; // define NTP client to get time
// NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

bool initial = 1;

void createDial();

void setup(void)
{
    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(BLACK);
    delay(200);
    tft.fillScreen(RED);
    delay(200);
    tft.fillScreen(GREEN);
    delay(200);
    tft.fillScreen(BLUE);
    delay(200);
    tft.fillScreen(BLACK);
    delay(200);
    tft.fillScreen(GREY);

    createDial();

    Serial.begin(9600);
    Serial.println();
    Serial.println();
    // WiFi.begin(ssid, password);

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }
    Serial.print("connection with ");
    Serial.println(ssid);
    Serial.println("-------------------------------");

    // timeClient.begin();
    // timeClient.update();
    Serial.print("internet server time: ");
    // Serial.println(timeClient.getFormattedTime());
    Serial.println("2024-4-14 1:54:00");

    // hh = timeClient.getHours();
    // mm = timeClient.getMinutes();
    // ss = timeClient.getSeconds();

    hh = 1;
    mm = 54;
    ss = 0;
}

void loop()
{
    if (targetTime < millis())
    {
        targetTime += 1000;
        ss++; // advance second
        if (ss == 60)
        {
            ss = 0;
            mm++; // advance minute
            if (mm > 59)
            {
                mm = 0;
                hh++; // advance hour
                if (hh > 23)
                {
                    hh = 0;
                    // timeClient.update(); // update at midnight
                }
            }
        }

        // pre-compute hand degrees, x & y coords for a fast screen update
        sdeg = ss * 6;                     // 0-59 -> 0-354
        mdeg = mm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
        hdeg = hh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
        hx = cos((hdeg - 90) * DEG2RAD);
        hy = sin((hdeg - 90) * DEG2RAD);
        mx = cos((mdeg - 90) * DEG2RAD);
        my = sin((mdeg - 90) * DEG2RAD);
        sx = cos((sdeg - 90) * DEG2RAD);
        sy = sin((sdeg - 90) * DEG2RAD);

        if (ss == 0 || initial)
        {
            initial = 0;
            tft.drawLine(ohx, ohy, 120, 121, BLACK); // erase hour and minute hand positions every minute
            ohx = hx * 62 + 121;
            ohy = hy * 62 + 121;
            tft.drawLine(omx, omy, 120, 121, BLACK);
            omx = mx * 84 + 120;
            omy = my * 84 + 121;
        }

        tft.drawLine(osx, osy, 120, 121, BLACK); // redraw new hand positions, hour and minute hands not erased here to avoid flicker
        osx = sx * 90 + 121;
        osy = sy * 90 + 121;
        tft.drawLine(osx, osy, 120, 121, RED);
        tft.drawLine(ohx, ohy, 120, 121, WHITE);
        tft.drawLine(omx, omy, 120, 121, WHITE);
        tft.drawLine(osx, osy, 120, 121, RED);
        tft.fillCircle(120, 121, 3, RED);
    }
}

void createDial()
{

    tft.setTextColor(WHITE, GREY);
    tft.fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
    tft.fillCircle(120, 120, 110, BLACK);

    for (int i = 0; i < 360; i += 30) // draw 12 line segments at the outer ring
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 114 + 120;
        yy0 = sy * 114 + 120;
        x1 = sx * 100 + 120;
        yy1 = sy * 100 + 120;
        tft.drawLine(x0, yy0, x1, yy1, GREEN);
    }

    for (int i = 0; i < 360; i += 6) // draw 60 dots - minute markers
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 102 + 120;
        yy0 = sy * 102 + 120;
        tft.drawPixel(x0, yy0, WHITE);

        if (i == 0 || i == 180)
            tft.fillCircle(x0, yy0, 2, WHITE); // draw main quadrant dots
        if (i == 90 || i == 270)
            tft.fillCircle(x0, yy0, 2, WHITE);
    }

    tft.fillCircle(120, 121, 3, WHITE); // pivot
    targetTime = millis() + 1000;
}

#endif

#ifdef myCompass

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

const char *ssid = "xxxxxxxxxxxxxxxx";     // network wifi credentials  - fill in your wifi network name
const char *password = "xxxxxxxxxxxxxxxx"; // network wifi credentials  - fill in your wifi key

const long utcOffsetInSeconds = 3600; // 3600 = western europe winter time - 7200 = western europe summer time
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define TFT_DC 0
#define TFT_CS 5
#define DEG2RAD 0.0174532925

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

Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // saved H, M, S x & y multipliers
float ox = 0, oy = 0, px = 0, py = 0, qx = 0, qy = 0, rx = 0, ry = 0;
float D1x = 0, D1y = 0, D2x = 0, D2y = 0, D3x = 0, D3y = 0;
float DDD = 33.3;
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // saved H, M, S x & y coords
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0; // for next 1 second timeout
float destination_Declination = 30;
int hh = 0; // hours variable
int mm = 0; // minutes variable
int ss = 0; // seconds variable

// WiFiUDP ntpUDP; // define NTP client to get time
// NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

bool initial = 1;

void createDial();

void Draw_green_ticks_bevels();
void Draw_points_azimuths();
void Draw_Destination(float);
void Draw_Compass(float);

void setup(void)
{
    // Serial.begin(9600);
    // 开启
    tft.begin();
    // 设置旋转
    tft.setRotation(0);
    tft.fillScreen(BLACK); // 全屏幕显示黑色
    delay(200);
    tft.fillScreen(RED); // 全屏幕显示红色
    delay(200);
    tft.fillScreen(GREEN); // 全屏幕显示绿色
    delay(200);
    tft.fillScreen(BLUE); // 全屏幕显示蓝色
    delay(200);
    tft.fillScreen(BLACK); // 全屏幕显示黑色
    delay(200);
    tft.fillScreen(GREY); // 全屏幕显示灰色

    createDial(); // 绘制表盘

    Serial.begin(9600);
    Serial.println();
    Serial.println();
    // WiFi.begin(ssid, password);

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }
    Serial.print("connection with ");
    Serial.println(ssid);
    Serial.println("-------------------------------");

    // timeClient.begin();
    // timeClient.update();
    Serial.print("internet server time: ");
    // Serial.println(timeClient.getFormattedTime());
    Serial.println("2024-4-14 1:54:00");

    // hh = timeClient.getHours();
    // mm = timeClient.getMinutes();
    // ss = timeClient.getSeconds();

    hh = 1;
    mm = 54;
    ss = 0;
}

void loop()
{

    // if (targetTime < millis())
    // {
    //     targetTime += 1000;
    ss++; // clockwise second
    // ss--; // anticlock second
    if (ss == 60)
    {
        ss = 0;
        // mm++; // advance minute
        // if (mm > 59)
        // {
        //     mm = 0;
        //     hh++; // advance hour
        //     if (hh > 23)
        //     {
        //         hh = 0;
        //         // timeClient.update(); // update at midnight
        //     }
        // }
    }

    // 通过角度来决定指针的朝向
    sdeg = ss * 6; // 0-59 -> 0-354
                   // sx = cos((sdeg - 90) * DEG2RAD);
                   // sy = sin((sdeg - 90) * DEG2RAD);

    /*--------------------------------秒针   works!-----------------------------------*/
    // tft.drawLine(osx, osy, 120, 121, BLACK); // redraw new hand positions, hour and minute hands not erased here to avoid flicker
    // osx = sx * 90 + 121;
    // osy = sy * 90 + 121;
    // tft.drawLine(osx, osy, 120, 121, RED);
    // tft.fillCircle(120, 121, 3, RED);
    /*---------------------------------秒针  works!----------------------------------*/
    // 清除上一次画的阴影
    tft.fillTriangle(ox, oy, px, py, rx, ry, BLACK);
    tft.fillTriangle(qx, qy, px, py, rx, ry, BLACK);
    tft.fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, BLACK);
    Draw_points_azimuths();
    Draw_green_ticks_bevels();
    Draw_Compass(sdeg);
    Draw_Destination(60);
    /*---------------------------------画一个菱形----------------------------------*/
    /*
        已知变量：
            圆点x坐标：120
            圆点y坐标：120
            r1：60
            r2：15
        未知变量：
            角度1：方向角
            角度2：角度1+90
        一个菱形由两个三角形组成，一共需要知道4个点的坐标
        A - B - C - D
        A : 上顶点
        B ：右顶点
        C ：下顶点
        D ：左顶点


    */

    // tft.fillCircle(D1x, D1y, 3, BLACK);
    // tft.fillCircle(D2x, D2y, 3, BLACK);
    // tft.fillCircle(D3x, D3y, 3, BLACK);

    // 重新画一遍方位字母和绿色刻度
    // 360/30 = 12，每22.5度，画一个刻度
    // for (float i = 0; i < 360; i += 22.5) // draw 16 line segments at the outer ring
    // {
    //     sx = cos((i - 90) * DEG2RAD);
    //     sy = sin((i - 90) * DEG2RAD);
    //     x0 = sx * 114 + 120;
    //     yy0 = sy * 114 + 120;
    //     x1 = sx * 100 + 120;
    //     yy1 = sy * 100 + 120;
    //     tft.drawLine(x0, yy0, x1, yy1, GREEN);

    //     if (i == 45)
    //     {
    //         // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
    //         tft.setTextSize(2);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 - 24, yy0 + 14);
    //         tft.print("NE");
    //     }
    //     else if (i == 135)
    //     {
    //         // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
    //         tft.setTextSize(2);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 - 24, yy0 - 30);
    //         tft.print("SE");
    //     }
    //     else if (i == 225)
    //     {
    //         tft.setTextSize(2);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 + 14, yy0 - 30);
    //         tft.print("SW");
    //     }
    //     else if (i == 315)
    //     {
    //         tft.setTextSize(2);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 + 14, yy0 + 14);
    //         tft.print("NW");
    //     }
    // }

    // for (int i = 0; i < 360; i += 6) // draw 60 dots - minute markers
    // {
    //     sx = cos((i - 90) * DEG2RAD);
    //     sy = sin((i - 90) * DEG2RAD);
    //     x0 = sx * 102 + 120;
    //     yy0 = sy * 102 + 120;
    //     tft.drawPixel(x0, yy0, WHITE);

    //     if (i == 0)
    //     {
    //         // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
    //         tft.setTextSize(3);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 - 7, yy0);
    //         tft.print("N");
    //     }

    //     if (i == 90)
    //     {
    //         tft.setTextSize(3);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 - 15, yy0 - 10);
    //         tft.print("E");
    //     }
    //     if (i == 180)
    //     {
    //         tft.setTextSize(3);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 - 7, yy0 - 20);
    //         tft.print("S");
    //     }
    //     if (i == 270)
    //     {
    //         tft.setTextSize(3);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(x0 + 5, yy0 - 10);
    //         tft.print("O");
    //     }
    // }

    // // 先画点A
    // // tft.drawLine(ox, oy, 120, 121, BLACK); // erase hour and minute hand positions every minute
    // ox = (120 + (70 * sin((-sdeg) * DEG2RAD)));
    // oy = (120 + (70 * cos((-sdeg) * DEG2RAD)));
    // // tft.drawLine(ox, oy, 120, 121, GREEN);

    // // 再画点B
    // // tft.drawLine(px, py, 120, 121, BLACK); // erase hour and minute hand positions every minute
    // px = (120 + 15 * sin((90 - sdeg) * DEG2RAD));
    // py = (120 + 15 * cos((90 - sdeg) * DEG2RAD));
    // // tft.drawLine(px, py, 120, 121, YELLOW);

    // // 再画点C
    // // tft.drawLine(qx, qy, 120, 121, BLACK); // erase hour and minute hand positions every minute
    // qx = (120 + 70 * sin((180 - sdeg) * DEG2RAD));
    // qy = (120 + 70 * cos((180 - sdeg) * DEG2RAD));
    // // tft.drawLine(qx, qy, 120, 121, GREEN);

    // // 再画点D
    // // tft.drawLine(rx, ry, 120, 121, BLACK); // erase hour and minute hand positions every minute
    // rx = (120 + 15 * sin((270 - sdeg) * DEG2RAD));
    // ry = (120 + 15 * cos((270 - sdeg) * DEG2RAD));
    // // tft.drawLine(rx, ry, 120, 121, YELLOW);

    // // tft.drawTriangle(ox, oy, px, py, rx, ry, RED);
    // tft.fillTriangle(ox, oy, px, py, rx, ry, RED);
    // // tft.drawTriangle(qx, qy, px, py, rx, ry, BLUE);
    // tft.fillTriangle(qx, qy, px, py, rx, ry, BLUE);
    // tft.fillCircle(120, 121, 3, RED);
    /*---------------------------------画一个菱形----------------------------------*/

    /*---------------------------------画目的地方向----------------------------------*/
    // 画出目的地的相对方向
    /*
        假设目的地方位与当前朝向的夹角为DDD
        就相当于从罗盘正N开始，顺时针+DDD的角度为当前目的地方向，顺时针累加
        需要分别计算三个点
        顶点A：
            x =  (120 + (110 * sin((-sdeg) * DEG2RAD)));
            y = (120 + (110 * cos((-sdeg) * DEG2RAD)));
        点B：
        点C：

    */

    // D1x = (120 + (100 * sin((-(sdeg + DDD)) * DEG2RAD)));
    // D1y = (120 + (100 * cos((-(sdeg + DDD)) * DEG2RAD)));

    // D2x = (120 + (80 * sin((-(sdeg - 7 + DDD)) * DEG2RAD)));
    // D2y = (120 + (80 * cos((-(sdeg - 7 + DDD)) * DEG2RAD)));

    // D3x = (120 + (80 * sin((-(7 + sdeg + DDD)) * DEG2RAD)));
    // D3y = (120 + (80 * cos((-(7 + sdeg + DDD)) * DEG2RAD)));

    // // DDD 3顶点
    // // tft.fillCircle(D1x, D1y, 3, RED);
    // // tft.fillCircle(D2x, D2y, 3, BLUE);
    // // tft.fillCircle(D3x, D3y, 3, GREEN);

    // tft.fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, GREEN);
    // }
    delay(100);
}

void createDial()
{

    tft.setTextColor(WHITE, GREY);
    tft.fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
    tft.fillCircle(120, 120, 110, BLACK);

    // 360/22.5 = 16，每22.5度，画一个刻度
    for (float i = 0; i < 360; i += 22.5) // draw 16 line segments at the outer ring
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 114 + 120;
        yy0 = sy * 114 + 120;
        x1 = sx * 100 + 120;
        yy1 = sy * 100 + 120;
        tft.drawLine(x0, yy0, x1, yy1, GREEN);

        if (i == 45)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 24, yy0 + 14);
            tft.print("NE");
        }
        else if (i == 135)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 24, yy0 - 30);
            tft.print("SE");
        }
        else if (i == 225)
        {
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 + 14, yy0 - 30);
            tft.print("SW");
        }
        else if (i == 315)
        {
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 + 14, yy0 + 14);
            tft.print("NW");
        }
    }

    for (int i = 0; i < 360; i += 6) // draw 60 dots - minute markers
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 102 + 120;
        yy0 = sy * 102 + 120;
        tft.drawPixel(x0, yy0, WHITE);

        if (i == 0)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 7, yy0);
            tft.print("N");
        }

        if (i == 90)
        {
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 15, yy0 - 10);
            tft.print("E");
        }
        if (i == 180)
        {
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 7, yy0 - 20);
            tft.print("S");
        }
        if (i == 270)
        {
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 + 5, yy0 - 10);
            tft.print("O");
        }
    }

    tft.fillCircle(120, 121, 3, WHITE); // pivot  中心圆点
    targetTime = millis() + 1000;
}

void Draw_green_ticks_bevels()
{
    for (float i = 0; i < 360; i += 22.5) // draw 16 line segments at the outer ring
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 114 + 120;
        yy0 = sy * 114 + 120;
        x1 = sx * 100 + 120;
        yy1 = sy * 100 + 120;
        tft.drawLine(x0, yy0, x1, yy1, GREEN);

        if (i == 45)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 24, yy0 + 14);
            tft.print("NE");
        }
        else if (i == 135)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 24, yy0 - 30);
            tft.print("SE");
        }
        else if (i == 225)
        {
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 + 14, yy0 - 30);
            tft.print("SW");
        }
        else if (i == 315)
        {
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 + 14, yy0 + 14);
            tft.print("NW");
        }
    }
}

void Draw_points_azimuths()
{
    for (int i = 0; i < 360; i += 6) // draw 60 dots - minute markers
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 102 + 120;
        yy0 = sy * 102 + 120;
        tft.drawPixel(x0, yy0, WHITE);

        if (i == 0)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 7, yy0);
            tft.print("N");
        }

        if (i == 90)
        {
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 15, yy0 - 10);
            tft.print("E");
        }
        if (i == 180)
        {
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 - 7, yy0 - 20);
            tft.print("S");
        }
        if (i == 270)
        {
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(x0 + 5, yy0 - 10);
            tft.print("O");
        }
    }
}
void Draw_Destination(float dest)
{
    D1x = (120 + (100 * sin((-(sdeg + dest)) * DEG2RAD)));
    D1y = (120 + (100 * cos((-(sdeg + dest)) * DEG2RAD)));

    D2x = (120 + (80 * sin((-(sdeg - 7 + dest)) * DEG2RAD)));
    D2y = (120 + (80 * cos((-(sdeg - 7 + dest)) * DEG2RAD)));

    D3x = (120 + (80 * sin((-(7 + sdeg + dest)) * DEG2RAD)));
    D3y = (120 + (80 * cos((-(7 + sdeg + dest)) * DEG2RAD)));

    // DDD 3顶点
    // tft.fillCircle(D1x, D1y, 3, RED);
    // tft.fillCircle(D2x, D2y, 3, BLUE);
    // tft.fillCircle(D3x, D3y, 3, GREEN);

    tft.fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, GREEN);
}

void Draw_Compass(float degree)
{

    // 先画点A
    // tft.drawLine(ox, oy, 120, 121, BLACK); // erase hour and minute hand positions every minute
    ox = (120 + (70 * sin((-degree) * DEG2RAD)));
    oy = (120 + (70 * cos((-degree) * DEG2RAD)));
    // tft.drawLine(ox, oy, 120, 121, GREEN);

    // 再画点B
    // tft.drawLine(px, py, 120, 121, BLACK); // erase hour and minute hand positions every minute
    px = (120 + 15 * sin((90 - degree) * DEG2RAD));
    py = (120 + 15 * cos((90 - degree) * DEG2RAD));
    // tft.drawLine(px, py, 120, 121, YELLOW);

    // 再画点C
    // tft.drawLine(qx, qy, 120, 121, BLACK); // erase hour and minute hand positions every minute
    qx = (120 + 70 * sin((180 - degree) * DEG2RAD));
    qy = (120 + 70 * cos((180 - degree) * DEG2RAD));
    // tft.drawLine(qx, qy, 120, 121, GREEN);

    // 再画点D
    // tft.drawLine(rx, ry, 120, 121, BLACK); // erase hour and minute hand positions every minute
    rx = (120 + 15 * sin((270 - degree) * DEG2RAD));
    ry = (120 + 15 * cos((270 - degree) * DEG2RAD));
    // tft.drawLine(rx, ry, 120, 121, YELLOW);

    // tft.drawTriangle(ox, oy, px, py, rx, ry, RED);
    tft.fillTriangle(ox, oy, px, py, rx, ry, RED);
    // tft.drawTriangle(qx, qy, px, py, rx, ry, BLUE);
    tft.fillTriangle(qx, qy, px, py, rx, ry, BLUE);
}

#endif
