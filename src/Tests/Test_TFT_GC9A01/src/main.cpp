
// #define dashboard
// #define CompassPlane
// #define Panda
// #define Rotated_Sprite
// #define watchESP
#define compass_zhao

#ifdef dashboard
// This example draws an animated dial with a rotating needle.

// The dial is a jpeg image, the needle is created using a rotated
// Sprite. The example operates by reading blocks of pixels from the
// TFT, thus the TFT setup must support reading from the TFT CGRAM.

// The sketch operates by creating a copy of the screen block where
// the needle will be drawn, the needle is then drawn on the screen.
// When the needle moves, the original copy of the screen area is
// pushed to the screen to over-write the needle graphic. A copy
// of the screen where the new position will be drawn is then made
// before drawing the needle in the new position. This technique
// allows the needle to move over other screen graphics.

// The sketch calculates the size of the buffer memory required and
// reserves the memory for the TFT block copy.

// Created by Bodmer 17/3/20 as an example to the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

#define NEEDLE_LENGTH 35         // Visible length
#define NEEDLE_WIDTH 5           // Width of needle - make it an odd number
#define NEEDLE_RADIUS 90         // Radius at tip
#define NEEDLE_COLOR1 TFT_MAROON // Needle periphery colour
#define NEEDLE_COLOR2 TFT_RED    // Needle centre colour
#define DIAL_CENTRE_X 120
#define DIAL_CENTRE_Y 120

// Font attached to this sketch
#include "NotoSansBold36.h"
#define AA_FONT_LARGE NotoSansBold36

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite needle = TFT_eSprite(&tft); // Sprite object for needle
TFT_eSprite spr = TFT_eSprite(&tft);    // Sprite for meter reading

// Jpeg image array attached to this sketch
#include "dial.h"
#include "plane.h"
#include "png_compass_plane.h"
#include "png_compass_transparant.h"

/*-------------------------从watchESP添加----------------------------*/
TFT_eSprite img = TFT_eSprite(&tft); // Sprite for the compass
/*-------------------------从watchESP添加----------------------------*/

/*-------------------------从watchESP添加----------------------------*/
#include "fonts.h"
double rad = 0.01745;
float x[360];
float y[360];

float px[360];
float py[360];

float lx[360];
float ly[360];

int r = 104;
int sx = 120;
int sy = 120;

#define color1 TFT_WHITE
#define color2 0x8410
#define color3 0x5ACB
#define color4 0x15B3
#define color5 0x00A3

String cc[12] = {"45", "40", "35", "30", "25", "20", "15", "10", "05", "0", "55", "50"};
String days[] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
int start[12];
int startP[60];

int lastAngle = 0;
float circle = 100;
bool dir = 0;
int rAngle = 359;

int angle = 0;
bool onOff = 0;
bool debounce = 0;

String h, m, s, d1, d2, m1, m2;

/*------------------------- 从watchESP添加----------------------------*/

// Include the jpeg decoder library
#include <TJpg_Decoder.h>
#include <PNGdec.h>
uint16_t *tft_buffer;
bool buffer_loaded = false;
uint16_t spr_width = 0;
uint16_t bg_color = 0;
PNG png;
// =======================================================================================
// This function will be called during decoding of the jpeg file
// =======================================================================================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    // Stop further decoding as image is running off bottom of screen
    if (y >= tft.height())
        return 0;

    // This function will clip the image block rendering automatically at the TFT boundaries
    tft.pushImage(x, y, w, h, bitmap);

    // Return 1 to decode next block
    return 1;
}
void createNeedle(void);
void plotNeedle(int16_t angle, uint16_t ms_delay);
// =======================================================================================
// Setup
// =======================================================================================
void setup()
{
    // Serial.begin(115200); // Debug only
    Serial.begin(9600); // Debug only
    // The byte order can be swapped (set true for TFT_eSPI)
    TJpgDec.setSwapBytes(true);
    // The jpeg decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);

    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    /*-------------------------从watchESP添加----------------------------*/
    // img.setSwapBytes(true);
    // img.createSprite(240, 240);
    // img.setTextDatum(4);
    // img.setFreeFont(&DSEG7_Modern_Bold_20);
    /*-------------------------从watchESP添加----------------------------*/

    /*-------------------------画表盘----------------------------*/

    //  Draw the dial
    // TJpgDec.drawJpg(0, 0, DSEG7_Modern_Bold_20Bitmaps, sizeof(DSEG7_Modern_Bold_20Bitmaps));
    TJpgDec.drawJpg(0, 0, dial, sizeof(dial));
    //  tft.drawCircle(DIAL_CENTRE_X, DIAL_CENTRE_Y, NEEDLE_RADIUS - NEEDLE_LENGTH, TFT_DARKGREY);

    // Load the font and create the Sprite for reporting the value

    /*-------------------------画屏幕中间的数字----------------------------*/
    spr.loadFont(AA_FONT_LARGE);
    spr_width = spr.textWidth("777"); // 7 is widest numeral in this font
    spr.createSprite(spr_width, spr.fontHeight());
    bg_color = tft.readPixel(120, 120); // Get colour from dial centre
    spr.fillSprite(bg_color);
    spr.setTextColor(TFT_WHITE, bg_color, true);
    spr.setTextDatum(MC_DATUM);
    spr.setTextPadding(spr_width);
    // spr.drawNumber(0, spr_width / 2, spr.fontHeight() / 2); // 根据第一个参数，在屏幕中间渲染数字
    spr.pushSprite(DIAL_CENTRE_X - spr_width / 2, DIAL_CENTRE_Y - spr.fontHeight() / 2);

    /*-------------------------画屏幕中间的文字----------------------------*/
    // Plot the label text
    tft.setTextColor(TFT_WHITE, bg_color);
    tft.setTextDatum(MC_DATUM);
    // tft.drawString("(degrees)", DIAL_CENTRE_X, DIAL_CENTRE_Y + 48, 2); // 在屏幕中间渲染文字“degree”

    // Define where the needle pivot point is on the TFT before
    // creating the needle so boundary calculation is correct
    tft.setPivot(DIAL_CENTRE_X, DIAL_CENTRE_Y);

    // Create the needle Sprite
    createNeedle();

    /*-------------------------根据参数渲染红色指针和数字----------------------------*/
    // Reset needle position to 0
    // plotNeedle(0, 0); // 根据参数渲染红色指针

    delay(2000);
}

// =======================================================================================
// Loop
// =======================================================================================
void loop()
{
    /*-------------------------从watchESP添加----------------------------*/
    rAngle = rAngle - 2;

    // int now = __DATE__;

    img.fillSprite(TFT_BLACK);
    img.fillCircle(sx, sy, 124, color5);

    img.setTextColor(TFT_WHITE, color5);

    img.drawString(days[5], circle, 120, 2);
    for (int i = 0; i < 12; i++)
        if (start[i] + angle < 360)
        {
            img.drawString(cc[i], x[start[i] + angle], y[start[i] + angle], 2);
            img.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
        }
        else
        {
            img.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360], 2);
            img.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
        }

    img.setFreeFont(&DSEG7_Modern_Bold_20);
    img.drawString(s, sx, sy - 36);
    img.setFreeFont(&DSEG7_Classic_Regular_28);
    img.drawString(h + ":" + m, sx, sy + 28);
    img.setTextFont(0);

    img.fillRect(70, 86, 12, 20, color3);
    img.fillRect(84, 86, 12, 20, color3);
    img.fillRect(150, 86, 12, 20, color3);
    img.fillRect(164, 86, 12, 20, color3);

    img.setTextColor(0x35D7, TFT_BLACK);
    img.drawString("MONTH", 84, 78);
    img.drawString("DAY", 162, 78);
    img.setTextColor(TFT_ORANGE, TFT_BLACK);
    img.drawString("VOLOS PROJECTS", 120, 174);
    img.drawString("***", 120, 104);

    img.setTextColor(TFT_WHITE, color3);
    img.drawString(m1, 77, 96, 2);
    img.drawString(m2, 91, 96, 2);

    img.drawString(d1, 157, 96, 2);
    img.drawString(d2, 171, 96, 2);

    for (int i = 0; i < 60; i++)
        if (startP[i] + angle < 360)
            img.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color1);
        else
            img.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color1);

    img.fillTriangle(sx - 1, sy - 70, sx - 5, sy - 56, sx + 4, sy - 56, TFT_ORANGE);
    img.fillCircle(px[rAngle], py[rAngle], 6, TFT_RED);
    img.pushSprite(0, 0);

    /*-------------------------从watchESP添加----------------------------*/

    // uint16_t angle = random(241); // random speed in range 0 to 240

    // Plot needle at random angle in range 0 to 240, speed 40ms per increment
    // plotNeedle(angle, 30);

    // Pause at new position
    // delay(2500);
}

// =======================================================================================
// Create the needle Sprite
// =======================================================================================
void createNeedle(void)
{
    needle.setColorDepth(16);
    needle.createSprite(NEEDLE_WIDTH, NEEDLE_LENGTH); // create the needle Sprite

    needle.fillSprite(TFT_BLACK); // Fill with black

    // Define needle pivot point relative to top left corner of Sprite
    uint16_t piv_x = NEEDLE_WIDTH / 2; // pivot x in Sprite (middle)
    uint16_t piv_y = NEEDLE_RADIUS;    // pivot y in Sprite
    needle.setPivot(piv_x, piv_y);     // Set pivot point in this Sprite

    // Draw the red needle in the Sprite
    needle.fillRect(0, 0, NEEDLE_WIDTH, NEEDLE_LENGTH, TFT_MAROON);
    needle.fillRect(1, 1, NEEDLE_WIDTH - 2, NEEDLE_LENGTH - 2, TFT_RED);

    // Bounding box parameters to be populated
    int16_t min_x;
    int16_t min_y;
    int16_t max_x;
    int16_t max_y;

    // Work out the worst case area that must be grabbed from the TFT,
    // this is at a 45 degree rotation
    needle.getRotatedBounds(45, &min_x, &min_y, &max_x, &max_y);

    // Calculate the size and allocate the buffer for the grabbed TFT area
    tft_buffer = (uint16_t *)malloc(((max_x - min_x) + 2) * ((max_y - min_y) + 2) * 2);
}

// =======================================================================================
// Move the needle to a new position
// =======================================================================================
void plotNeedle(int16_t angle, uint16_t ms_delay)
{
    static int16_t old_angle = -120; // Starts at -120 degrees

    // Bounding box parameters
    static int16_t min_x;
    static int16_t min_y;
    static int16_t max_x;
    static int16_t max_y;

    if (angle < 0)
        angle = 0; // Limit angle to emulate needle end stops
    if (angle > 240)
        angle = 240;

    angle -= 120; // Starts at -120 degrees

    // Move the needle until new angle reached
    while (angle != old_angle || !buffer_loaded)
    {

        if (old_angle < angle)
            old_angle++;
        else
            old_angle--;

        // Only plot needle at even values to improve plotting performance
        if ((old_angle & 1) == 0)
        {
            if (buffer_loaded)
            {
                // Paste back the original needle free image area
                tft.pushRect(min_x, min_y, 1 + max_x - min_x, 1 + max_y - min_y, tft_buffer);
            }

            if (needle.getRotatedBounds(old_angle, &min_x, &min_y, &max_x, &max_y))
            {
                // Grab a copy of the area before needle is drawn
                tft.readRect(min_x, min_y, 1 + max_x - min_x, 1 + max_y - min_y, tft_buffer);
                buffer_loaded = true;
            }

            // Draw the needle in the new position, black in needle image is transparent
            needle.pushRotated(old_angle, TFT_BLACK);

            // Wait before next update
            delay(ms_delay);
        }

        // Update the number at the centre of the dial
        spr.setTextColor(TFT_WHITE, bg_color, true);
        spr.drawNumber(old_angle + 120, spr_width / 2, spr.fontHeight() / 2);
        spr.pushSprite(120 - spr_width / 2, 120 - spr.fontHeight() / 2);

        // Slow needle down slightly as it approaches the new position
        if (abs(old_angle - angle) < 10)
            ms_delay += ms_delay / 5;
    }
}

// =======================================================================================

#endif

#ifdef CompassPlane

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <heading.h>
#include <plane.h>
#include "dial.h"
#include "png_compass_transparant.h"
#include "png_compass_plane.h"
#include <PNGdec.h>
#include <TJpg_Decoder.h> // render un img en format array dans un ecran de TFT

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite fb = TFT_eSprite(&tft);
TFT_eSprite dial_s = TFT_eSprite(&tft);
TFT_eSprite plane_s = TFT_eSprite(&tft);

int i = 0;

void setup()
{

    tft.begin();
    tft.setRotation(0);

    fb.setColorDepth(16);
    fb.createSprite(240, 240);
    fb.setPivot(120, 120);

    dial_s.setColorDepth(8);
    dial_s.createSprite(240, 240);
    dial_s.setPivot(120, 120);
    dial_s.pushImage(00, 00, 240, 240, heading);

    plane_s.setColorDepth(16);
    plane_s.createSprite(121, 190);
    plane_s.pushImage(00, 00, 121, 190, plane);

    tft.setPivot(120, 120);
}

void loop()
{
    delay(20);
    dial_s.pushRotated(&fb, i++, 0xc);
    plane_s.pushToSprite(&fb, 60, 7, 0);
    fb.pushSprite(0, 0);
}

#endif

#ifdef Panda

// This example renders a png file that is stored in a FLASH array
// using the PNGdec library (available via library manager).

// Note: The PNGDEC required lots of RAM to work (~40kbytes) so
// this sketch is will not run on smaller memory processors (e.g.
// ESP8266, STM32F103 etc.)

// Image files can be converted to arrays using the tool here:
// https://notisrac.github.io/FileToCArray/
// To use this tool:
//   1. Drag and drop file on "Browse..." button
//   2. Tick box "Treat as binary"
//   3. Click "Convert"
//   4. Click "Save as file" and move the header file to sketch folder
//   5. Open the sketch in IDE
//   6. Include the header file containing the array (panda.h in this example)

// Include the PNG decoder library
#include <PNGdec.h>
#include "panda.h" // Image is stored here in an 8-bit array
#include "png_compass_plane.h"
#include "png_compass_transparant.h"

PNG png; // PNG decoder instance

#define MAX_IMAGE_WIDTH 239 // Adjust for your images

int16_t xpos = 0;
int16_t ypos = 0;

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
void pngDraw(PNGDRAW *pDraw);
void pngPlane(PNGDRAW *pDraw);
//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
    Serial.begin(115200);
    Serial.println("\n\n Using the PNGdec library");

    // Initialise the TFT
    tft.begin();
    tft.fillScreen(TFT_BLACK);

    Serial.println("\r\nInitialisation done.");

    int16_t rc = png.openFLASH((uint8_t *)png_compass_transparant, sizeof(png_compass_transparant), pngDraw);
    if (rc == PNG_SUCCESS)
    {
        // Serial.println("Successfully opened png file");
        // Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        tft.startWrite();
        uint32_t dt = millis();
        rc = png.decode(NULL, 0);
        // Serial.print(millis() - dt);
        // Serial.println("ms");
        tft.endWrite();
        // png.close(); // not needed for memory->memory decode
    }
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{

    // int16_t rc = png.openFLASH((uint8_t *)png_compass_transparant, sizeof(png_compass_transparant), pngDraw);
    // if (rc == PNG_SUCCESS)
    // {
    //     // Serial.println("Successfully opened png file");
    //     // Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    //     tft.startWrite();
    //     uint32_t dt = millis();
    //     rc = png.decode(NULL, 0);
    //     // Serial.print(millis() - dt);
    //     // Serial.println("ms");
    //     tft.endWrite();
    //     // png.close(); // not needed for memory->memory decode
    // }
    //
    // int16_t plane = png.openFLASH((uint8_t *)png_compass_plane, sizeof(png_compass_plane), pngPlane);
    // if (plane == PNG_SUCCESS)
    // {
    //     // Serial.println("Successfully opened png file");
    //     // Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    //     tft.startWrite();
    //     uint32_t dt = millis();
    //     plane = png.decode(NULL, 0);
    //     Serial.print(millis() - dt);
    //     Serial.println("ms");
    //     tft.endWrite();
    //     // png.close(); // not needed for memory->memory decode
    // }

    // delay(3000);
    // tft.fillScreen(random(0x10000));
}

//=========================================v==========================================
//                                      pngDraw
//====================================================================================
// This next function will be called during decoding of the png file to
// render each image line to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
// Callback function to draw pixels to the display
void pngDraw(PNGDRAW *pDraw)
{
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    tft.pushImage(0, 0 + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void pngPlane(PNGDRAW *pDraw)
{
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    tft.pushImage(49, 49 + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}
#endif

#ifdef Rotated_Sprite
// This example plots a rotated Sprite to the screen using the pushRotated()
// function. It is written for a 240 x 320 TFT screen.

// Two rotation pivot points must be set, one for the Sprite and one for the TFT
// using setPivot(). These pivot points do not need to be within the visible screen
// or Sprite boundary.

// When the Sprite is rotated and pushed to the TFT with pushRotated(angle) it will be
// drawn so that the two pivot points coincide. This makes rotation about a point on the
// screen very simple. The rotation is clockwise with increasing angle. The angle is in
// degrees, an angle of 0 means no Sprite rotation.

// The pushRotated() function works with 1, 4, 8 and 16-bit per pixel (bpp) Sprites.

// The original Sprite is unchanged so can be plotted again at a different angle.

// Optionally a transparent colour can be defined, pixels of this colour will
// not be plotted to the TFT.

// For 1 bpp Sprites the foreground and background colours are defined with the
// function spr.setBitmapColor(foregroundColor, backgroundColor).

// For 4 bpp Sprites the colour map index is used instead of the 16-bit colour
// e.g. spr.setTextColor(5); // Green text in default colour map
// See "Transparent_Sprite_Demo_4bit" example for default colour map details

// Created by Bodmer 6/1/19 as an example to the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); // TFT object

TFT_eSprite spr = TFT_eSprite(&tft); // Sprite object

void drawX(int x, int y);
void showMessage(String msg);

// =======================================================================================
// Setup
// =======================================================================================

void setup()
{
    Serial.begin(250000); // Debug only

    tft.begin(); // initialize
    tft.setRotation(0);
}

// =======================================================================================
// Loop
// =======================================================================================

void loop()
{

    int xw = tft.width() / 2; // xw, yh is middle of screen
    int yh = tft.height() / 2;

    // showMessage("90 degree angles");
    // tft.setPivot(xw, yh); // Set pivot to middle of TFT screen
    // drawX(xw, yh);        // Show where screen pivot is

    // // Create the Sprite
    // spr.setColorDepth(8);      // Create an 8bpp Sprite of 60x30 pixels
    // spr.createSprite(64, 30);  // 8bpp requires 64 * 30 = 1920 bytes
    // spr.setPivot(32, 55);      // Set pivot relative to top left corner of Sprite
    // spr.fillSprite(TFT_BLACK); // Fill the Sprite with black

    // spr.setTextColor(TFT_GREEN);        // Green text
    // spr.setTextDatum(MC_DATUM);         // Middle centre datum
    // spr.drawString("Hello", 32, 15, 4); // Plot text, font 4, in Sprite at 30, 15

    // spr.pushRotated(0);
    // spr.pushRotated(90);
    // spr.pushRotated(180);
    // spr.pushRotated(270);

    // delay(2000);

    // showMessage("45 degree angles");
    // drawX(xw, yh); // Show where screen pivot is

    // spr.pushRotated(45);
    // spr.pushRotated(135);
    // spr.pushRotated(225);
    // spr.pushRotated(315);

    // delay(2000); // Pause so we see it

    // showMessage("Moved Sprite pivot point");
    // drawX(xw, yh); // Show where screen pivot is

    // spr.setPivot(-20, 15); // Change just the Sprite pivot point

    // spr.pushRotated(45);
    // spr.pushRotated(135);
    // spr.pushRotated(225);
    // spr.pushRotated(315);

    // delay(2000); // Pause so we see it

    // showMessage("Moved TFT pivot point");
    // tft.setPivot(100, 100); // Change just the TFT pivot point
    // drawX(100, 100);        // Show where pivot is

    // spr.pushRotated(45);
    // spr.pushRotated(135);
    // spr.pushRotated(225);
    // spr.pushRotated(315);

    // delay(2000); // Pause so we see it

    // showMessage("Transparent rotations");
    // tft.fillCircle(xw, yh, 70, TFT_DARKGREY); // Draw a filled circle

    // tft.setPivot(xw, yh); // Set pivot to middle of screen
    // drawX(xw, yh);        // Show where pivot is

    // spr.deleteSprite();

    // spr.setColorDepth(8);     // Create a 8bpp Sprite
    // spr.createSprite(40, 30); // Create a new Sprite 40x30
    // spr.setPivot(20, 70);     // Set Sprite pivot at 20,80

    // spr.setTextColor(TFT_RED);  // Red text in Sprite
    // spr.setTextDatum(MC_DATUM); // Middle centre datum

    // int num = 1;

    // for (int16_t angle = 30; angle <= 360; angle += 30)
    // {
    //     spr.fillSprite(TFT_BLACK);         // Clear the Sprite
    //     spr.drawNumber(num, 20, 15, 4);    // Plot number, in Sprite at 20,15 and with font 4
    //     spr.pushRotated(angle, TFT_BLACK); // Plot rotated Sprite, black being transparent
    //     num++;
    // }

    // spr.setTextColor(TFT_WHITE); // White text in Sprite
    // spr.setPivot(-75, 15);       // Set Sprite pivot at -75,15

    // for (int16_t angle = -90; angle < 270; angle += 30)
    // {
    //     spr.fillSprite(TFT_BLACK);             // Clear the Sprite
    //     spr.drawNumber(angle + 90, 20, 15, 4); // Plot number, in Sprite at 20,15 and with font 4
    //     spr.pushRotated(angle, TFT_BLACK);     // Plot rotated Sprite, black being transparent
    //     num++;
    // }

    delay(8000); // Pause so we see it

    spr.deleteSprite();
}

// =======================================================================================
// Draw an X centered on x,y
// =======================================================================================

void drawX(int x, int y)
{
    tft.drawLine(x - 5, y - 5, x + 5, y + 5, TFT_WHITE);
    tft.drawLine(x - 5, y + 5, x + 5, y - 5, TFT_WHITE);
}

// =======================================================================================
// Show a message at the top of the screen
// =======================================================================================

void showMessage(String msg)
{
    // Clear the screen areas
    tft.fillRect(0, 0, tft.width(), 20, TFT_BLACK);
    tft.fillRect(0, 20, tft.width(), tft.height() - 20, TFT_BLUE);

    uint8_t td = tft.getTextDatum(); // Get current datum

    tft.setTextDatum(TC_DATUM); // Set new datum

    tft.drawString(msg, tft.width() / 2, 2, 2); // Message in font 2

    tft.setTextDatum(td); // Restore old datum
}

// =======================================================================================

#endif

#ifdef watchESP
#include <TFT_eSPI.h>
#include "fonts.h"
#include "time.h"
// #include "RTClib.h"

// RTC_DS3231 rtc; // 一块芯片，硬件时钟电路（RTC）

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

#define color1 TFT_WHITE
#define color2 0x8410
#define color3 0x5ACB
#define color4 0x15B3
#define color5 0x00A3

volatile int counter = 0;
float VALUE;
float lastValue = 0;

double rad = 0.01745;
float x[360];
float y[360];

float px[360];
float py[360];

float lx[360];
float ly[360];

int r = 104;
int sx = 120;
int sy = 120;

String cc[12] = {"45", "40", "35", "30", "25", "20", "15", "10", "05", "0", "55", "50"};
String days[] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
int start[12];
int startP[60];

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

int angle = 0;
bool onOff = 0;
bool debounce = 0;

String h, m, s, d1, d2, m1, m2;

void setup()
{

    // if (!rtc.begin())
    // {
    //     Serial.println("Couldn't find RTC");
    // }
    pinMode(2, OUTPUT);
    pinMode(0, INPUT_PULLUP);
    pinMode(35, INPUT_PULLUP);
    pinMode(13, INPUT_PULLUP);

    digitalWrite(2, 0);

    ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
    ledcAttachPin(5, pwmLedChannelTFT);
    ledcWrite(pwmLedChannelTFT, 200);

    tft.init();

    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);

    img.setSwapBytes(true);
    img.createSprite(240, 240);
    img.setTextDatum(4);

    int b = 0;
    int b2 = 0;

    for (int i = 0; i < 360; i++)
    {
        x[i] = (r * cos(rad * i)) + sx;
        y[i] = (r * sin(rad * i)) + sy;
        px[i] = ((r - 16) * cos(rad * i)) + sx;
        py[i] = ((r - 16) * sin(rad * i)) + sy;

        lx[i] = ((r - 26) * cos(rad * i)) + sx;
        ly[i] = ((r - 26) * sin(rad * i)) + sy;

        if (i % 30 == 0)
        {
            start[b] = i;
            b++;
        }

        if (i % 6 == 0)
        {
            startP[b2] = i;
            b2++;
        }
    }
}

int lastAngle = 0;
float circle = 100;
bool dir = 0;
int rAngle = 359;

void loop()
{

    rAngle = rAngle - 2;
    time_t now;
    // DateTime now = rtc.now();

    angle = now * 3.14;

    // s = String(now.second());
    // m = String(now.minute());
    // h = String(now.hour());
    s = String(now * 5);
    m = String(now * 6);
    h = String(now * 7);

    if (m.toInt() < 10)
        m = "0" + m;

    if (h.toInt() < 10)
        h = "0" + h;

    if (s.toInt() < 10)
        s = "0" + s;

    // if (now.day() > 10)
    // {
    //     d1 = now.day() / 10;
    //     d2 = now.day() % 10;
    // }
    // else
    // {
    //     d1 = "0";
    //     d2 = String(now.day());
    // }
    d1 = now * 7 / 10;
    d2 = now * 7 % 10;
    // if (now.month() > 10)
    // {
    //     m1 = now.month() / 10;
    //     m2 = now.month() % 10;
    // }
    // else
    // {
    //     m1 = "0";
    //     m2 = String(now.month());
    // }
    m1 = now * 9 / 10;
    m2 = now * 9 % 10;
    if (angle >= 360)
        angle = 0;

    if (rAngle <= 0)
        rAngle = 359;

    if (dir == 0)
        circle = circle + 0.5;
    else
        circle = circle - 0.5;

    if (circle > 140)
        dir = !dir;

    if (circle < 100)
        dir = !dir;

    if (angle > -1)
    {
        lastAngle = angle;

        VALUE = ((angle - 270) / 3.60) * -1;
        if (VALUE < 0)
            VALUE = VALUE + 100;

        img.fillSprite(TFT_BLACK);
        img.fillCircle(sx, sy, 124, color5);

        img.setTextColor(TFT_WHITE, color5);

        // img.drawString(days[now.dayOfTheWeek()], circle, 120, 2);
        img.drawString(days[5], circle, 120, 2);

        for (int i = 0; i < 12; i++)
            if (start[i] + angle < 360)
            {
                img.drawString(cc[i], x[start[i] + angle], y[start[i] + angle], 2);
                img.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
            }
            else
            {
                img.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360], 2);
                img.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
            }

        img.setFreeFont(&DSEG7_Modern_Bold_20);
        img.drawString(s, sx, sy - 36);
        img.setFreeFont(&DSEG7_Classic_Regular_28);
        img.drawString(h + ":" + m, sx, sy + 28);
        img.setTextFont(0);

        img.fillRect(70, 86, 12, 20, color3);
        img.fillRect(84, 86, 12, 20, color3);
        img.fillRect(150, 86, 12, 20, color3);
        img.fillRect(164, 86, 12, 20, color3);

        img.setTextColor(0x35D7, TFT_BLACK);
        img.drawString("MONTH", 84, 78);
        img.drawString("DAY", 162, 78);
        img.setTextColor(TFT_ORANGE, TFT_BLACK);
        img.drawString("VOLOS PROJECTS", 120, 174);
        img.drawString("***", 120, 104);

        img.setTextColor(TFT_WHITE, color3);
        img.drawString(m1, 77, 96, 2);
        img.drawString(m2, 91, 96, 2);

        img.drawString(d1, 157, 96, 2);
        img.drawString(d2, 171, 96, 2);

        for (int i = 0; i < 60; i++)
            if (startP[i] + angle < 360)
                img.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color1);
            else
                img.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color1);

        img.fillTriangle(sx - 1, sy - 70, sx - 5, sy - 56, sx + 4, sy - 56, TFT_ORANGE);
        img.fillCircle(px[rAngle], py[rAngle], 6, TFT_RED);
        img.pushSprite(0, 0);
    }
}
#endif

#ifdef compass_zhao
#define NEEDLE_LENGTH 35         // Visible length
#define NEEDLE_WIDTH 5           // Width of needle - make it an odd number
#define NEEDLE_RADIUS 90         // Radius at tip
#define NEEDLE_COLOR1 TFT_MAROON // Needle periphery colour
#define NEEDLE_COLOR2 TFT_RED    // Needle centre colour
#define DIAL_CENTRE_X 120
#define DIAL_CENTRE_Y 120

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite needle = TFT_eSprite(&tft); // Sprite object for needle
TFT_eSprite spr = TFT_eSprite(&tft);    // Sprite for meter reading

// Font attached to this sketch
#include "NotoSansBold36.h"
#define AA_FONT_LARGE NotoSansBold36
// Jpeg image array attached to this sketch
#include "dial.h"
#include "plane.h"
#include "png_compass_plane.h"
#include "png_compass_transparant.h"

// Include the jpeg decoder library
#include <TJpg_Decoder.h>
#include <PNGdec.h>
uint16_t *tft_buffer;
bool buffer_loaded = false;
uint16_t spr_width = 0;
uint16_t bg_color = 0;
PNG png;

void setup()
{
    // Serial.begin(115200); // Debug only
    Serial.begin(9600); // Debug only
    // The byte order can be swapped (set true for TFT_eSPI)
    TJpgDec.setSwapBytes(true);
    // The jpeg decoder must be given the exact name of the rendering function above
    // TJpgDec.setCallback(tft_output);

    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    /*-------------------------从watchESP添加----------------------------*/
    // img.setSwapBytes(true);
    // img.createSprite(240, 240);
    // img.setTextDatum(4);
    // img.setFreeFont(&DSEG7_Modern_Bold_20);
    /*-------------------------从watchESP添加----------------------------*/

    /*-------------------------画表盘----------------------------*/

    //  Draw the dial
    // TJpgDec.drawJpg(0, 0, DSEG7_Modern_Bold_20Bitmaps, sizeof(DSEG7_Modern_Bold_20Bitmaps));
    TJpgDec.drawJpg(0, 0, dial, sizeof(dial));
    //  tft.drawCircle(DIAL_CENTRE_X, DIAL_CENTRE_Y, NEEDLE_RADIUS - NEEDLE_LENGTH, TFT_DARKGREY);

    // Load the font and create the Sprite for reporting the value

    /*-------------------------画屏幕中间的数字----------------------------*/
    spr.loadFont(AA_FONT_LARGE);
    spr_width = spr.textWidth("777"); // 7 is widest numeral in this font
    spr.createSprite(spr_width, spr.fontHeight());
    bg_color = tft.readPixel(120, 120); // Get colour from dial centre
    spr.fillSprite(bg_color);
    spr.setTextColor(TFT_WHITE, bg_color, true);
    spr.setTextDatum(MC_DATUM);
    spr.setTextPadding(spr_width);
    // spr.drawNumber(0, spr_width / 2, spr.fontHeight() / 2); // 根据第一个参数，在屏幕中间渲染数字
    spr.pushSprite(DIAL_CENTRE_X - spr_width / 2, DIAL_CENTRE_Y - spr.fontHeight() / 2);

    /*-------------------------画屏幕中间的文字----------------------------*/
    // Plot the label text
    tft.setTextColor(TFT_WHITE, bg_color);
    tft.setTextDatum(MC_DATUM);
    // tft.drawString("(degrees)", DIAL_CENTRE_X, DIAL_CENTRE_Y + 48, 2); // 在屏幕中间渲染文字“degree”

    // Define where the needle pivot point is on the TFT before
    // creating the needle so boundary calculation is correct
    tft.setPivot(DIAL_CENTRE_X, DIAL_CENTRE_Y);

    // Create the needle Sprite
    // createNeedle();

    /*-------------------------根据参数渲染红色指针和数字----------------------------*/
    // Reset needle position to 0
    // plotNeedle(0, 0); // 根据参数渲染红色指针

    delay(2000);
}

void loop()
{

    uint16_t angle = random(241); // random speed in range 0 to 240

    // Plot needle at random angle in range 0 to 240, speed 40ms per increment
    // plotNeedle(angle, 30);

    // Pause at new position
    delay(2500);
}

#endif