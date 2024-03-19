#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

// #define BOARD_ESP32DEV 1
#define BOARD_ESP32DOIT_DEVKIT_V1 2
#define SPI_TFCard false
#define RUN_DEMO false

#if BOARD == BOARD_ESP32DEV

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

/*
    @brief  Instantiate Adafruit GC9A01A driver with software SPI
    @param  cs    Chip select pin #   05
    @param  dc    Data/Command pin #    19
    @param  mosi  SPI MOSI pin #    23
    @param  sclk  SPI Clock pin #   18
    @param  rst   Reset pin # (optional, pass -1 if unused)
    @param  miso  SPI MISO pin # (optional, pass -1 if unused)
*/
#endif

#endif

#if BOARD == BOARD_ESP32DOIT_DEVKIT_V1

#define TFT_BL_BLK 14       // LED back-light
#define TFT_CS_SS 5         // Chip select control pin
#define TFT_DC 16           // Data Command control pin
#define TFT_RES_RST 13      // Reset pin (could connect to Arduino RESET pin)
#define TFT_SDA_DIN_MOSI 23 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCL_CLK_SCK 18  // In some display driver board, it might be written as "SCL" and so on.

// Adafruit_GC9A01A(int8_t _CS, int8_t _DC, int8_t _MOSI, int8_t _SCLK, int8_t _RST = -1, int8_t _MISO = -1);

#endif

// Hardware SPI on Feather or other boards
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

unsigned long testFillScreen();
unsigned long testText();
unsigned long testLines(uint16_t color);
unsigned long testFastLines(uint16_t color1, uint16_t color2);
unsigned long testRects(uint16_t color);
unsigned long testFilledRects(uint16_t color1, uint16_t color2);
unsigned long testFilledCircles(uint8_t radius, uint16_t color);
unsigned long testCircles(uint8_t radius, uint16_t color);
unsigned long testTriangles();
unsigned long testFilledTriangles();
unsigned long testRoundRects();
unsigned long testFilledRoundRects();

void testMaisonScreen();
void runScreenDemo();
void testInterfaceAccueil();
void dessinerBatteryLogo(int16_t coordX, int16_t coordY, int16_t largeurX, int pourcentage);

void setup()
{
    pinMode(33, INPUT);
    pinMode(32, OUTPUT);
    // tone(32, 500, 5000);

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    Serial.begin(115200);
    Serial.println("GC9A01A Test!");

    tft.begin();
    tft.fillScreen(GC9A01A_BLACK);

    Serial.println(F("Setup Done!"));
}

void loop(void)
{
    testInterfaceAccueil();

    // testMaisonScreen();

#if RUN_DEMO == true
    runScreenDemo();
#endif
}

void testInterfaceAccueil()
{
    tft.setCursor(140, 85);
    tft.write("Bonjour");

    tft.setCursor(20, 120);
    // Serial.println(tft.width());
    tft.setCursor(20, 140);
    // Serial.println(tft.height());

    tft.startWrite();
    tft.writeFillRect(90, 180, 50, 50, GC9A01A_YELLOW);
    tft.writeFillRect(140, 180, 50, 50, GC9A01A_DARKCYAN);
    tft.setTextColor(GC9A01A_WHITE);
    tft.setCursor(110, 200);
    tft.write("Hello World !");
    tft.endWrite();

    tft.fillRect(175, 110, 50, 50, GC9A01A_RED);
    tft.drawRect(175, 110, 50, 50, GC9A01A_WHITE);

    // Comme le nombre de pixel est pair (240), le centre est entre 2 pixels. On ne peut pas mettre de fraction de pixel... Donc pour avoir un cercle centré, il faut dessiner 4 cercles...
    tft.drawCircle(119, 119, 120, GC9A01A_BLUE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
    tft.drawCircle(120, 120, 120, GC9A01A_BLUE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
    tft.drawCircle(119, 120, 120, GC9A01A_BLUE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
    tft.drawCircle(120, 119, 120, GC9A01A_BLUE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120

    tft.drawPixel(0, 119, GC9A01A_RED);   // ouest / gauche
    tft.drawPixel(239, 119, GC9A01A_RED); // est / droite
    tft.drawPixel(119, 0, GC9A01A_RED);   // nord / haut
    tft.drawPixel(119, 239, GC9A01A_RED); // sud / bas

    dessinerBatteryLogo(tft.width() / 2 - 30, 30, 60, 100);
    dessinerBatteryLogo(20, 100, 150, 50);
    dessinerBatteryLogo(20, 70, 50, 50);
    dessinerBatteryLogo(100, 70, 20, 50);
    dessinerBatteryLogo(150, 70, 10, 50);
}

void dessinerBatteryLogo(int16_t coordX, int16_t coordY, int16_t largeurX, int pourcentage)
{
    double hauteurY = largeurX / 2;
    double zoneBarreVerteX = largeurX * 0.875;
    double barreVerteX = zoneBarreVerteX / 3;
    double barreVerteY = hauteurY * 0.8;
    double coordBarreVerteX = coordX + (barreVerteX - barreVerteX * 0.875);
    double coordBarreVerteY = coordY + (hauteurY - barreVerteY) / 2;

    // tft.drawRect(coordX, coordY, largeurX, hauteurY, GC9A01A_RED);  // Contour
    tft.drawRect(coordX, coordY, zoneBarreVerteX, hauteurY, GC9A01A_WHITE);                                   // Contour
    tft.fillRect(coordX + zoneBarreVerteX, coordY + hauteurY / 4, hauteurY / 4, hauteurY / 2, GC9A01A_WHITE); // ti boute        + hauteurY / 2 - 16/2

    tft.fillRect((coordBarreVerteX), coordBarreVerteY, (barreVerteX * 0.8125), barreVerteY, GC9A01A_GREEN);                   // niveau #1
    tft.fillRect((coordBarreVerteX + barreVerteX), coordBarreVerteY, (barreVerteX * 0.8125), barreVerteY, GC9A01A_GREEN);     // niveau #2
    tft.fillRect((coordBarreVerteX + barreVerteX * 2), coordBarreVerteY, (barreVerteX * 0.8125), barreVerteY, GC9A01A_GREEN); // niveau #3
}

void testMaisonScreen()
{
    // tft.fillScreen(GC9A01A_BLACK);
    tft.setCursor(40, 100);
    // tft.setTextColor(GC9A01A_WHITE);
    tft.setTextSize(2);

    int valeur = digitalRead(33);

    char buffer[15]; // 1 caractère de plus que le nombre de lettres à cacher !

    if (valeur == HIGH)
    {
        // tft.setCursor(40, 100);
        // tft.setTextColor(GC9A01A_BLACK);
        // tft.print("Pas appuyer...");
        tft.setCursor(40, 100);
        tft.setTextColor(GC9A01A_WHITE, GC9A01A_BLACK); // mettre GC9A01A_BLACK pour le fond
        // tft.print("Appuyer !");
        tft.printf("%-14s", "Appuyer !");

        // sprintf(buffer, "%-14s", "Appuyer !");
        // tft.print(buffer);
    }
    else
    {
        // tft.setCursor(40, 100);
        // tft.setTextColor(GC9A01A_BLACK);
        // tft.print("Appuyer !");
        tft.setCursor(40, 100);
        tft.setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
        // tft.print("Pas appuyer...");
        tft.printf("%-14s", "Pas appuyer...");

        // sprintf(buffer, "%-14s", "Pas appuyer...");
        // tft.print(buffer);
    }
}

void runScreenDemo()
{

    Serial.println(F("Benchmark                Time (microseconds)"));
    delay(10);
    Serial.print(F("Screen fill              "));
    Serial.println(testFillScreen());
    delay(500);

    Serial.print(F("Text                     "));
    Serial.println(testText());
    delay(3000);

    Serial.print(F("Lines                    "));
    Serial.println(testLines(GC9A01A_CYAN));
    delay(500);

    Serial.print(F("Horiz/Vert Lines         "));
    Serial.println(testFastLines(GC9A01A_RED, GC9A01A_BLUE));
    delay(500);

    Serial.print(F("Rectangles (outline)     "));
    Serial.println(testRects(GC9A01A_GREEN));
    delay(500);

    Serial.print(F("Rectangles (filled)      "));
    Serial.println(testFilledRects(GC9A01A_YELLOW, GC9A01A_MAGENTA));
    delay(500);

    Serial.print(F("Circles (filled)         "));
    Serial.println(testFilledCircles(10, GC9A01A_MAGENTA));

    Serial.print(F("Circles (outline)        "));
    Serial.println(testCircles(10, GC9A01A_WHITE));
    delay(500);

    Serial.print(F("Triangles (outline)      "));
    Serial.println(testTriangles());
    delay(500);

    Serial.print(F("Triangles (filled)       "));
    Serial.println(testFilledTriangles());
    delay(500);

    Serial.print(F("Rounded rects (outline)  "));
    Serial.println(testRoundRects());
    delay(500);

    Serial.print(F("Rounded rects (filled)   "));
    Serial.println(testFilledRoundRects());
    delay(500);

    Serial.println(F("Test Screen Demo Done!"));
    Serial.println(F("Enter Infinite LOOP !!!"));

    for (uint8_t rotation = 0; rotation < 4; rotation++)
    {
        tft.setRotation(rotation);
        testText();
        delay(1000);
    }
}

unsigned long testFillScreen()
{
    unsigned long start = micros();
    tft.fillScreen(GC9A01A_BLACK);
    yield();
    tft.fillScreen(GC9A01A_RED);
    yield();
    tft.fillScreen(GC9A01A_GREEN);
    yield();
    tft.fillScreen(GC9A01A_BLUE);
    yield();
    tft.fillScreen(GC9A01A_BLACK);
    yield();
    return micros() - start;
}

unsigned long testText()
{
    tft.fillScreen(GC9A01A_BLACK);
    unsigned long start = micros();
    tft.setCursor(0, 0);
    tft.setTextColor(GC9A01A_WHITE);
    tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(GC9A01A_YELLOW);
    tft.setTextSize(2);
    tft.println(1234.56);
    tft.setTextColor(GC9A01A_RED);
    tft.setTextSize(3);
    tft.println(0xDEADBEEF, HEX);
    tft.println();
    tft.setTextColor(GC9A01A_GREEN);
    tft.setTextSize(5);
    tft.println("Groop");
    tft.setTextSize(2);
    tft.println("I implore thee,");
    tft.setTextSize(1);
    tft.println("my foonting turlingdromes.");
    tft.println("And hooptiously drangle me");
    tft.println("with crinkly bindlewurdles,");
    tft.println("Or I will rend thee");
    tft.println("in the gobberwarts");
    tft.println("with my blurglecruncheon,");
    tft.println("see if I don't!");
    return micros() - start;
}

unsigned long testLines(uint16_t color)
{
    unsigned long start, t;
    int x1, y1, x2, y2,
        w = tft.width(),
        h = tft.height();

    tft.fillScreen(GC9A01A_BLACK);
    yield();

    x1 = y1 = 0;
    y2 = h - 1;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    x2 = w - 1;
    for (y2 = 0; y2 < h; y2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    t = micros() - start; // fillScreen doesn't count against timing

    yield();
    tft.fillScreen(GC9A01A_BLACK);
    yield();

    x1 = w - 1;
    y1 = 0;
    y2 = h - 1;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    x2 = 0;
    for (y2 = 0; y2 < h; y2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    t += micros() - start;

    yield();
    tft.fillScreen(GC9A01A_BLACK);
    yield();

    x1 = 0;
    y1 = h - 1;
    y2 = 0;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    x2 = w - 1;
    for (y2 = 0; y2 < h; y2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    t += micros() - start;

    yield();
    tft.fillScreen(GC9A01A_BLACK);
    yield();

    x1 = w - 1;
    y1 = h - 1;
    y2 = 0;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);
    x2 = 0;
    for (y2 = 0; y2 < h; y2 += 6)
        tft.drawLine(x1, y1, x2, y2, color);

    yield();
    return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2)
{
    unsigned long start;
    int x, y, w = tft.width(), h = tft.height();

    tft.fillScreen(GC9A01A_BLACK);
    start = micros();
    for (y = 0; y < h; y += 5)
        tft.drawFastHLine(0, y, w, color1);
    for (x = 0; x < w; x += 5)
        tft.drawFastVLine(x, 0, h, color2);

    return micros() - start;
}

unsigned long testRects(uint16_t color)
{
    unsigned long start;
    int n, i, i2,
        cx = tft.width() / 2,
        cy = tft.height() / 2;

    tft.fillScreen(GC9A01A_BLACK);
    n = min(tft.width(), tft.height());
    start = micros();
    for (i = 2; i < n; i += 6)
    {
        i2 = i / 2;
        tft.drawRect(cx - i2, cy - i2, i, i, color);
    }

    return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2)
{
    unsigned long start, t = 0;
    int n, i, i2,
        cx = tft.width() / 2 - 1,
        cy = tft.height() / 2 - 1;

    tft.fillScreen(GC9A01A_BLACK);
    n = min(tft.width(), tft.height());
    for (i = n; i > 0; i -= 6)
    {
        i2 = i / 2;
        start = micros();
        tft.fillRect(cx - i2, cy - i2, i, i, color1);
        t += micros() - start;
        // Outlines are not included in timing results
        tft.drawRect(cx - i2, cy - i2, i, i, color2);
        yield();
    }

    return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color)
{
    unsigned long start;
    int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

    tft.fillScreen(GC9A01A_BLACK);
    start = micros();
    for (x = radius; x < w; x += r2)
    {
        for (y = radius; y < h; y += r2)
        {
            tft.fillCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color)
{
    unsigned long start;
    int x, y, r2 = radius * 2,
              w = tft.width() + radius,
              h = tft.height() + radius;

    // Screen is not cleared for this one -- this is
    // intentional and does not affect the reported time.
    start = micros();
    for (x = 0; x < w; x += r2)
    {
        for (y = 0; y < h; y += r2)
        {
            tft.drawCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long testTriangles()
{
    unsigned long start;
    int n, i, cx = tft.width() / 2 - 1,
              cy = tft.height() / 2 - 1;

    tft.fillScreen(GC9A01A_BLACK);
    n = min(cx, cy);
    start = micros();
    for (i = 0; i < n; i += 5)
    {
        tft.drawTriangle(
            cx, cy - i,     // peak
            cx - i, cy + i, // bottom left
            cx + i, cy + i, // bottom right
            tft.color565(i, i, i));
    }

    return micros() - start;
}

unsigned long testFilledTriangles()
{
    unsigned long start, t = 0;
    int i, cx = tft.width() / 2 - 1,
           cy = tft.height() / 2 - 1;

    tft.fillScreen(GC9A01A_BLACK);
    start = micros();
    for (i = min(cx, cy); i > 10; i -= 5)
    {
        start = micros();
        tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(0, i * 10, i * 10));
        t += micros() - start;
        tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(i * 10, i * 10, 0));
        yield();
    }

    return t;
}

unsigned long testRoundRects()
{
    unsigned long start;
    int w, i, i2,
        cx = tft.width() / 2 - 1,
        cy = tft.height() / 2 - 1;

    tft.fillScreen(GC9A01A_BLACK);
    w = min(tft.width(), tft.height());
    start = micros();
    for (i = 0; i < w; i += 6)
    {
        i2 = i / 2;
        tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
    }

    return micros() - start;
}

unsigned long testFilledRoundRects()
{
    unsigned long start;
    int i, i2,
        cx = tft.width() / 2 - 1,
        cy = tft.height() / 2 - 1;

    tft.fillScreen(GC9A01A_BLACK);
    start = micros();
    for (i = min(tft.width(), tft.height()); i > 20; i -= 6)
    {
        i2 = i / 2;
        tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
        yield();
    }

    return micros() - start;
}
