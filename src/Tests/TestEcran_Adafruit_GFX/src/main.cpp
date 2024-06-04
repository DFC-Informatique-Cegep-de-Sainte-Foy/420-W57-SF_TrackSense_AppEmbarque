/***
This example is intended to demonstrate the use of getPixel() versus
getRawPixel() and the fast horizontal and vertical drawing routines
in the GFXcanvas family of classes,

When using the GFXcanvas* classes as the image buffer for a hardware driver,
there is a need to get individual pixel color values at given physical
coordinates. Rather than subclasses or client classes call getBuffer() and
reinterpret the byte layout of the buffer, two methods are added to each of the
GFXcanvas* classes that allow fetching of specific pixel values.

  * getPixel(x, y)   : Gets the pixel color value at the rotated coordinates in
the image.
  * getRawPixel(x,y) : Gets the pixel color value at the unrotated coordinates
in the image. This is useful for getting the pixel value to map to a hardware
pixel location. This method was made protected as only the hardware driver
should be accessing it.

The GFXcanvas*SerialDemo classes in this example will print to Serial the
contents of the underlying GFXcanvas buffer in both the current rotated layout
and the underlying physical layout.
***/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
// #include "GFXcanvasSerialDemo.h"
#include <Arduino.h>

#define SPI_TFCard false

#if SPI_TFCard

#define TFT_BL_BLK -1       // LED back-light
#define TFT_CS_SS 12        // 25       // Chip select control pin
#define TFT_DC 25           // 02          // Data Command control pin
#define TFT_RES_RST -1      // Reset pin (could connect to Arduino RESET pin)
#define TFT_SDA_DIN_MOSI 15 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCL_CLK_SCK 14  // In some display driver board, it might be written as "SCL" and so on.
                            // #define TFT_MISO 02          // Ne semble pas être utile

#else // VSPI

#define TFT_BL_BLK -1       // LED back-light
#define TFT_CS_SS 05        // 25     // Chip select control pin
#define TFT_DC 00           // 19        // Data Command control pin
#define TFT_RES_RST -1      // Reset pin (could connect to Arduino RESET pin)
#define TFT_SDA_DIN_MOSI 23 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCL_CLK_SCK 18  // In some display driver board, it might be written as "SCL" and so on.
// #define TFT_MISO 19          // Ne semble pas être utile
#define TFT_MOSI
#define TFT_CLK
#define TFT_RST
#endif

// #define TFT_WIDTH_  240
// #define TFT_HEIGHT_ 240
#define TFT_WIDTH_ 239
#define TFT_HEIGHT_ 239

#include <Fonts/BebasNeue_Regular18pt7b.h> // Font logo TrackSense
// #include <Fonts/BebasNeue_Regular6pt7b.h>  // Font logo TrackSense
// #include <Fonts/BebasNeue_Regular24pt7b.h> // Font logo TrackSense
// #include <Fonts/FreeSansBold9pt7b.h> // Vraiment le meilleur à date
// #include <Fonts/FreeSansBold12pt7b.h> // Vraiment le meilleur à date
// #include <Fonts/FreeSansBold18pt7b.h> // Vraiment le meilleur à date
// #include <Fonts/FreeSansBold24pt7b.h> // Vraiment le meilleur à date

void drawLogoTS();

// Hardware SPI on Feather or other boards
// Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
Adafruit_GC9A01A *tft = new Adafruit_GC9A01A(TFT_CS_SS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
// This is an incomplete Arduino example to minimally show
// the canvas drawing approach. A real program would #include
// a display library header and declare a global 'display',
// also including and enabling a custom font.

// Then, in ADDITION to all that, there's...
// GFXcanvas1 canvas(TFT_WIDTH_, TFT_HEIGHT_); // 1-bit, 120x30 pixels
// GFXcanvas8 canvas(TFT_WIDTH_, TFT_HEIGHT_); // 8-bit, 120x30 pixels
// GFXcanvas16 canvas(TFT_WIDTH_, TFT_HEIGHT_); // 16-bit, 120x30 pixels
GFXcanvas16 *canvas = new GFXcanvas16(TFT_WIDTH_, TFT_HEIGHT_); // 16-bit, 120x30 pixels

void setup()
{
  Serial.begin(115200);

  // Display init and font select would take place here.
  // See later examples for that.
  // tft = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
  tft->begin();
  tft->fillScreen(0x07FF); // Clear screen to blue
  // canvas->flush();
  // canvas->getTextBounds("R", 0, 0, NULL, NULL, NULL, NULL);

  // Text might exceed width of canvas, so disable wrapping:
  canvas->setTextWrap(false);
  // canvas->fillScreen(0x03EF); // Clear canvas (not display)
  canvas->setFont(&BebasNeue_Regular18pt7b);
  canvas->setTextSize(1);
  canvas->setTextColor(GC9A01A_YELLOW, GC9A01A_BLUE);
}

void loop()
{
  Serial.println("Entering loop");

  // Draw something on the canvas:
  canvas->fillScreen(0x001F);               // Clear canvas (not display)
  canvas->setCursor(60, 100);               // Pos. is BASE LINE when using fonts!
  canvas->fillCircle(100, 100, 60, 0x780F); // Draw a circle
  canvas->print(millis());                  // Print elapsed time in milliseconds

  // drawLogoTS();
  /*
      Draw a RAM-resident 1-bit image at the specified (x,y) position, using the specified foreground (for set bits) and background (unset bits) colors.

      Parameters:
      x –     Top left corner x coordinate
      y –     Top left corner y coordinate
      bitmap – byte array with monochrome bitmap
      w –     Width of bitmap in pixels
      h –     Height of bitmap in pixels
      color – 16-bit 5-6-5 Color to draw pixels with
      bg –    16-bit 5-6-5 Color to draw background with
  */
  // Copy canvas to screen at upper-left corner.
  Serial.println("Drawing canvas to screen");
  // tft->drawBitmap(80, 80, canvas->getBuffer(), canvas->width(), canvas->height(), 0xFFFF, 0x0000);
  // tft->drawBitmap(0, 0, canvas->getBuffer(), canvas->width(), canvas->height(), 0x0000, 0xFFFF);
  tft->drawRGBBitmap(0, 0, canvas->getBuffer(), canvas->width(), canvas->height());
  Serial.println("Drawing canvas to screen done");
  delay(500);
}

void drawLogoTS() // TODO : Ajouter des fonctions pour dessiner le logo TS en Light Mode : canvas->drawfillCircle()
{
  int16_t coordX = 17; // "T" coordX = 16
  int16_t coordY = 65; // "T" coordX = 65
  canvas->setTextSize(7);

  int16_t coordX_ = 0;
  int16_t coordY_ = 0;

  uint16_t width = 42;  // "T" width = 42
  uint16_t height = 56; // "T" height = 56

  // canvas->getTextBounds("R", coordX, coordY, &coordX_, &coordY_, &width, &height);

  int widthWithoutSpace = width * 0.80952381; // widthWithoutSpace = 42 * 0.80952381 = 34
  int heightWithoutSpace = height * 0.875;    // heightWithoutSpace = 56 * 0.875 = 49

  int coordY2 = coordY + height + 1; // "premier E" coordX = 122     // 65 + 56 = 121

  // Draw "TRA"
  canvas->setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
  canvas->setCursor(coordX, coordY);
  canvas->printf("%-3s", "TRA");
  // Draw "C" of "TRACK"
  canvas->setTextColor(GC9A01A_RED, GC9A01A_BLACK);
  canvas->setCursor(coordX + width * 3, coordY);
  canvas->printf("%-1s", "C");
  // Draw "K" of "TRACK"
  canvas->setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
  canvas->setCursor(coordX + width * 4, coordY);
  canvas->printf("%-1s", "K");

  // Draw special "S"
  canvas->fillCircle(coordX + width * 0.761904762, coordY2 + (height * 0.160714286), 4, GC9A01A_RED);
  canvas->fillCircle(coordX + width * 0.547619048, coordY2 + (height * 0.053571429), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.285714286, coordY2 + (height * 0.089285714), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.142857143, coordY2 + (height * 0.250000000), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.309523810, coordY2 + (height * 0.392857143), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.523809524, coordY2 + (height * 0.500000000), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.714285714, coordY2 + (height * 0.642857143), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.547619048, coordY2 + (height * 0.803571429), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.285714286, coordY2 + (height * 0.839285714), 4, GC9A01A_WHITE);
  canvas->fillCircle(coordX + width * 0.095238095, coordY2 + (height * 0.714285714), 4, GC9A01A_RED);

  // Draw "ENSE"
  canvas->setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
  canvas->setCursor(coordX + width, coordY2);
  canvas->printf("%-4s", "ENSE");

  // Draw circle
  // Comme le nombre de pixel est pair (240), le centre est entre 2 pixels. On ne peut pas mettre de fraction de pixel... Donc pour avoir un cercle centré, il faut dessiner 4 cercles...
  canvas->drawCircle(119, 119, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
  canvas->drawCircle(120, 120, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
  canvas->drawCircle(119, 120, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
  canvas->drawCircle(120, 119, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
}