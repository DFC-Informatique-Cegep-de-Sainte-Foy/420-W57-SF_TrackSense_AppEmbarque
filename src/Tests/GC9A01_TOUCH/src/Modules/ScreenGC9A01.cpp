#include "Modules/ScreenGC9A01.h"

/* Fonts */
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
// #include <Fonts/BebasNeue_Regular18pt7b.h> // Font logo TrackSense
// #include <Fonts/BebasNeue_Regular6pt7b.h>  // Font logo TrackSense
// #include <Fonts/BebasNeue_Regular24pt7b.h> // Font logo TrackSense

ScreenGC9A01::ScreenGC9A01(TSProperties *TSProperties)
    : _TSProperties(TSProperties),
      _lastBuffer(0),
      status(false)
{
    this->tft = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
    this->canvas = new GFXcanvas16(TFT_WIDTH, TFT_HEIGHT);

    this->tft->begin(80000000);
    this->setRotation(this->_TSProperties->PropertiesScreen.ScreenRotation);
    this->canvas->setTextWrap(false);

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH); // Backlight on // TODO: Mettre dans une méthode pour allumer/éteindre le rétroéclairage

    // tft->setFont(&BebasNeue_Regular24pt7b);
    // tft->setFont(&FreeSansBold12pt7b);

    this->drawBackgroundColor();
}

ScreenGC9A01::~ScreenGC9A01()
{
    delete this->tft;
}

/*


    Elements


*/
#pragma region Elements

void ScreenGC9A01::drawLogoTS() // TODO : Ajouter des fonctions pour dessiner le logo TS en Light Mode : this->drawfillCircle()
{
    int16_t coordX = 17;
    int16_t coordY = 65;
    uint16_t width = 42;
    uint16_t height = 56;

    int16_t coordX_ = 0;
    int16_t coordY_ = 0;

    this->canvas->setTextSize(7);
    this->canvas->setFont();
    this->canvas->getTextBounds("R", coordX, coordY, &coordX_, &coordY_, &width, &height);

    int widthWithoutSpace = width * 0.80952381;
    int heightWithoutSpace = height * 0.875;

    int coordY2 = coordY + height + 1; // "premier E" coordX = 122     // 65 + 56 = 121

    // Draw "TRA"
    this->setTextColor();
    this->canvas->setCursor(coordX, coordY);
    this->canvas->printf("%-3s", "TRA");
    // Draw "C" of "TRACK"
    this->setTextColor(GC9A01A_RED, GC9A01A_BLACK, GC9A01A_RED, GC9A01A_WHITE);
    this->canvas->setCursor(coordX + width * 3, coordY);
    this->canvas->printf("%-1s", "C");
    // Draw "K" of "TRACK"
    this->setTextColor();
    this->canvas->setCursor(coordX + width * 4, coordY);
    this->canvas->printf("%-1s", "K");

    // Draw special "S"
    this->canvas->fillCircle(coordX + width * 0.761904762, coordY2 + (height * 0.160714286), 4, GC9A01A_RED);
    this->canvas->fillCircle(coordX + width * 0.547619048, coordY2 + (height * 0.053571429), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.285714286, coordY2 + (height * 0.089285714), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.142857143, coordY2 + (height * 0.250000000), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.309523810, coordY2 + (height * 0.392857143), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.523809524, coordY2 + (height * 0.500000000), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.714285714, coordY2 + (height * 0.642857143), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.547619048, coordY2 + (height * 0.803571429), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.285714286, coordY2 + (height * 0.839285714), 4, GC9A01A_WHITE);
    this->canvas->fillCircle(coordX + width * 0.095238095, coordY2 + (height * 0.714285714), 4, GC9A01A_RED);

    // Draw "ENSE"
    this->setTextColor();
    this->canvas->setCursor(coordX + width, coordY2);
    this->canvas->printf("%-4s", "ENSE");

    // Draw circle
    // Comme le nombre de pixel est pair (240), le centre est entre 2 pixels. On ne peut pas mettre de fraction de pixel... Donc pour avoir un cercle centré, il faut dessiner 4 cercles...
    this->canvas->drawCircle(119, 119, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
    this->canvas->drawCircle(120, 120, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
    this->canvas->drawCircle(119, 120, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120
    this->canvas->drawCircle(120, 119, 120, GC9A01A_WHITE); // Center X = 119.5 (0 to 239)    // Center Y = 119.5 (0 to 239)    // rayon = 120

    this->canvas->setFont(&FreeSans9pt7b);
}

void ScreenGC9A01::drawBattery(int16_t coordX, int16_t coordY, int16_t largeurX, uint pourcentage)
{
    double hauteurY = largeurX / 2;
    double zoneBarreVerteX = largeurX * 0.875;
    double barreVerteX = zoneBarreVerteX * 0.9;
    double barreVerteY = hauteurY * 0.8;
    double coordBarreVerteX = coordX + (zoneBarreVerteX - barreVerteX) / 2;
    double coordBarreVerteY = coordY + (hauteurY - barreVerteY) / 2;

    this->canvas->drawRect(coordX, coordY, zoneBarreVerteX, hauteurY, GC9A01A_WHITE);                                   // Contour
    this->canvas->fillRect(coordX + zoneBarreVerteX, coordY + hauteurY / 4, hauteurY / 4, hauteurY / 2, GC9A01A_WHITE); // ti boute        + hauteurY / 2 - 16/2

    switch (pourcentage)
    {
    case 0 ... 20:
        this->canvas->fillRect(coordBarreVerteX, coordBarreVerteY, (barreVerteX * pourcentage / 100), barreVerteY, GC9A01A_RED); // niveau #1
        break;

    case 21 ... 40:
        this->canvas->fillRect(coordBarreVerteX, coordBarreVerteY, (barreVerteX * pourcentage / 100), barreVerteY, GC9A01A_YELLOW); // niveau #1
        break;

    case 41 ... 100:
        this->canvas->fillRect(coordBarreVerteX, coordBarreVerteY, (barreVerteX * pourcentage / 100), barreVerteY, GC9A01A_GREEN); // niveau #1
        break;

    default:
        break;
    }

    if (largeurX < 100)
    {
        this->canvas->setTextSize(1);
    }
    else if (largeurX < 200)
    {
        this->canvas->setTextSize(2);
    }
    else
    {
        this->canvas->setTextSize(3);
    }

    this->canvas->setCursor((coordX + largeurX) * 1.01, coordBarreVerteY + hauteurY / 2);
    this->setTextColor();
    const String strBatteryLevel = String(pourcentage) + "%";
    this->canvas->printf("%-3s", strBatteryLevel.c_str());
}

void ScreenGC9A01::drawIsRideStarted(int16_t coordX, int16_t coordY, int16_t largeurX)
{
    double hauteurY = largeurX;

    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
    {
        this->canvas->fillTriangle(coordX, coordY, coordX + largeurX, coordY + hauteurY / 2, coordX, coordY + hauteurY, GC9A01A_GREEN);
    }
    else
    {
        this->canvas->fillRect(coordX, coordY, largeurX, hauteurY, GC9A01A_RED);
    }
}

void ScreenGC9A01::drawError()
{
    this->canvas->fillScreen(GC9A01A_RED);
    this->setTextColor(GC9A01A_BLACK, GC9A01A_RED, GC9A01A_WHITE, GC9A01A_RED);
    this->canvas->setTextSize(5);
    this->canvas->setCursor(15, 110);

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        this->canvas->printf("%-9s", "ERREUR !");
    }
    else // Anglais
    {
        this->canvas->printf("%-8s", "ERROR !");
    }
}

void ScreenGC9A01::drawIsGPSValid(int16_t coordX, int16_t coordY, int16_t largeurX)
{
    double hauteurY = largeurX;

    if (this->_TSProperties->PropertiesGPS.IsFixValid && this->_TSProperties->PropertiesGPS.IsGPSFixed)
    {
        this->canvas->fillTriangle(coordX, coordY, coordX + largeurX, coordY + hauteurY / 2, coordX, coordY + hauteurY, GC9A01A_GREEN);
    }
    else if (!this->_TSProperties->PropertiesGPS.IsFixValid && this->_TSProperties->PropertiesGPS.IsGPSFixed)
    {
        this->canvas->fillTriangle(coordX, coordY, coordX + largeurX, coordY + hauteurY / 2, coordX, coordY + hauteurY, GC9A01A_YELLOW);
    }
    else
    {
        this->canvas->fillRect(coordX, coordY, largeurX, hauteurY, GC9A01A_RED);
    }
}

void ScreenGC9A01::drawStatistics(String title, String value, String unit, int16_t titleCoordX, int16_t valueCoordX, int16_t unitCoordX, int16_t coordY)
{
    this->setTextSize(1);
    this->setFont(1);
    this->printText(title, titleCoordX, coordY);
    this->printText(unit, unitCoordX, coordY);
    this->setFont(2);
    this->printText(value, valueCoordX, coordY);
}

void ScreenGC9A01::drawCompass(float degree)
{
    this->sdeg = degree;
    // 当第一次或重新进入罗盘屏幕时，准备罗盘，只画一次罗盘，之后每次loop不再画罗盘
    if (!this->status)
    {
        Serial.println("Compass---->Entrer");
        // 画一次罗盘
        this->tft->setTextColor(WHITE, GREY);
        this->tft->fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
        this->tft->fillCircle(120, 120, 110, BLACK);
        // 更新屏幕状态
        this->status = true;
    }
    // this->tft->fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
    // this->tft->fillCircle(120, 120, 110, BLACK);
    // 数值有变动时，再清除上一次画的阴影，如果没有变动，则保留当前图形
    if (lastDegree != degree)
    {
        Serial.println("Compass---->Change");
        lastDegree = degree; // 更新数值
        this->tft->fillTriangle(ox, oy, px, py, rx, ry, BLACK);
        this->tft->fillTriangle(qx, qy, px, py, rx, ry, BLACK);
        this->tft->fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, BLACK);
        delayMicroseconds(80);
    }
    Draw_points_azimuths();
    Draw_green_ticks_bevels();
    Draw_Compass(degree);
    Draw_Destination(60);
}

#pragma endregion Elements

/*


    Drawing Tools


*/
#pragma region DrawingTools

void ScreenGC9A01::drawOnScreen()
{
    uint16_t temp = this->calculateScreenBuffer();

    if (this->_lastBuffer != temp || this->_TSProperties->PropertiesScreen.IsScreenRotationChanged)
    {
        this->_lastBuffer = temp;
        this->_TSProperties->PropertiesScreen.IsScreenRotationChanged = false;

        this->tft->drawRGBBitmap(0, 0, this->canvas->getBuffer(), this->canvas->width(), this->canvas->height());
    }
}

uint16_t ScreenGC9A01::calculateScreenBuffer()
{
    uint16_t width = this->canvas->width();
    uint16_t height = this->canvas->height();
    unsigned long temp = 0;

    for (uint16_t y = 0; y < height; ++y)
    {
        for (uint16_t x = 0; x < width; ++x)
        {
            uint16_t pixel = this->canvas->getPixel(x, y);
            temp += pixel;
        }
    }

    return temp;
}

int ScreenGC9A01::calculateXCoordTextToCenter(String text)
{
    /*
        @brief    Helper to determine size of a PROGMEM string with current
    font/size. Pass string and a cursor position, returns UL corner and W,H.
        @param    str     The flash-memory ascii string to measure
        @param    x       The current cursor X
        @param    y       The current cursor Y
        @param    x1      The boundary X coordinate, set by function
        @param    y1      The boundary Y coordinate, set by function
        @param    w      The boundary width, set by function
        @param    h      The boundary height, set by function
    */
    uint16_t textWidth, textHeight;
    int16_t x = 0, y = 0;

    this->canvas->getTextBounds(text, 10, 10, &x, &y, &textWidth, &textHeight);

    return (TFT_WIDTH - textWidth) / 2;
}

void ScreenGC9A01::setFont(uint id)
{

    switch (id)
    {
    case 0:
        this->canvas->setFont();
        break;

    case 1:
        this->canvas->setFont(&FreeSans9pt7b);
        break;

    case 2:
        this->canvas->setFont(&FreeSansBold9pt7b);
        break;

    case 3:
        this->canvas->setFont(&FreeSansOblique9pt7b);
        break;

    case 4:
        this->canvas->setFont(&FreeSans18pt7b);
        break;

    default:
        // this->canvas->setFont(&FreeSans9pt7b);
        this->canvas->setFont();
        break;
    }
}

int ScreenGC9A01::calculateXCoordItemToCenter(uint16_t lengthInPixels)
{
    return (TFT_WIDTH - lengthInPixels) / 2;
}

void ScreenGC9A01::drawBackgroundColor(uint16_t darkModeColor, uint16_t lightModeColor)
{

    if (this->_TSProperties->PropertiesScreen.IsDarkMode)
    {
        this->canvas->fillScreen(darkModeColor);
    }
    else
    {
        this->canvas->fillScreen(lightModeColor);
    }
}

void ScreenGC9A01::setTextColor(uint16_t textDarkModeColor,
                                uint16_t backgroundDarkModeColor,
                                uint16_t textLightModeColor,
                                uint16_t backgroundLightModeColor)
{

    if (this->_TSProperties->PropertiesScreen.IsDarkMode)
    {
        this->canvas->setTextColor(textDarkModeColor, backgroundDarkModeColor);
    }
    else
    {
        this->canvas->setTextColor(textLightModeColor, backgroundLightModeColor);
    }
}

void ScreenGC9A01::setRotation(u_int8_t rotation)
{
    this->canvas->setRotation(rotation);
}

void ScreenGC9A01::setTextSize(uint8_t size)
{
    this->canvas->setTextSize(size);
}

void ScreenGC9A01::setTextWrap(boolean wrap)
{
    this->canvas->setTextWrap(wrap);
}

void ScreenGC9A01::printText(String text, int16_t coordX, int16_t coordY)
{
    String text2 = "%-" + String(text.length()) + "s";
    const char *formatChar = text2.c_str();

    this->canvas->setCursor(coordX, coordY);
    this->canvas->printf(formatChar, text.c_str());
}

void ScreenGC9A01::drawRect(int16_t x, int16_t y, int16_t width, int16_t height,
                            uint16_t darkModeColor, uint16_t lightModeColor)
{
    if (this->_TSProperties->PropertiesScreen.IsDarkMode)
    {
        this->canvas->drawRect(x, y, width, height, darkModeColor);
    }
    else
    {
        this->canvas->drawRect(x, y, width, height, lightModeColor);
    }
}

void ScreenGC9A01::drawFillRect(int16_t x, int16_t y, int16_t width, int16_t height,
                                uint16_t darkModeColor, uint16_t lightModeColor)
{
    if (this->_TSProperties->PropertiesScreen.IsDarkMode)
    {
        this->canvas->fillRect(x, y, width, height, darkModeColor);
    }
    else
    {
        this->canvas->fillRect(x, y, width, height, lightModeColor);
    }
}

void ScreenGC9A01::Draw_green_ticks_bevels()
{
    for (float i = 0; i < 360; i += 22.5) // draw 16 line segments at the outer ring
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 114 + 120;
        yy0 = sy * 114 + 120;
        x1 = sx * 100 + 120;
        yy1 = sy * 100 + 120;
        this->tft->drawLine(x0, yy0, x1, yy1, GREEN);

        if (i == 45)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            this->tft->setTextSize(2);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 - 24, yy0 + 14);
            this->tft->print("NE");
        }
        else if (i == 135)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            this->tft->setTextSize(2);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 - 24, yy0 - 30);
            this->tft->print("SE");
        }
        else if (i == 225)
        {
            this->tft->setTextSize(2);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 + 14, yy0 - 30);
            this->tft->print("SW");
        }
        else if (i == 315)
        {
            this->tft->setTextSize(2);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 + 14, yy0 + 14);
            this->tft->print("NW");
        }
    }
}

void ScreenGC9A01::Draw_points_azimuths()
{
    for (int i = 0; i < 360; i += 6) // draw 60 dots - minute markers
    {
        sx = cos((i - 90) * DEG2RAD);
        sy = sin((i - 90) * DEG2RAD);
        x0 = sx * 102 + 120;
        yy0 = sy * 102 + 120;
        this->tft->drawPixel(x0, yy0, WHITE);

        if (i == 0)
        {
            // tft.fillCircle(x0, yy0, 2, WHITE); // draw N
            this->tft->setTextSize(3);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 - 7, yy0);
            this->tft->print("N");
        }

        if (i == 90)
        {
            this->tft->setTextSize(3);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 - 15, yy0 - 10);
            this->tft->print("E");
        }
        if (i == 180)
        {
            this->tft->setTextSize(3);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 - 7, yy0 - 20);
            this->tft->print("S");
        }
        if (i == 270)
        {
            this->tft->setTextSize(3);
            this->tft->setTextColor(WHITE);
            this->tft->setCursor(x0 + 5, yy0 - 10);
            this->tft->print("O");
        }
    }
}

void ScreenGC9A01::Draw_Destination(float dest)
{
    // // 根据目的地点相对于当前坐标的位置，分成4种情况，在当前坐标的东北、东南、西南、西北
    // // 当前屏幕是翻转180度的，就是0,0坐标由左上，变到了右下
    // // 如果在东北： 90度-dest,因为整体翻转了180度，所以最终还要再加180度：90-dest+180
    // float degreeAjuste = 0.0;
    // float DX = _TSProperties->PropertiesCurrentRide.longitude_destination; // 目的地经度
    // float DY = _TSProperties->PropertiesCurrentRide.latitude_destination;  // 目的地维度
    // float OX = _TSProperties->PropertiesGPS.Longitude;                     // 当前经度
    // float OY = _TSProperties->PropertiesGPS.Latitude;                      // 当前维度

    // if (DX > OX && DY > OY)
    // {
    //     // 目的地在当前的东北
    //     degreeAjuste = 180 + 90 - (dest);
    // }
    // else if (DX > OX && DY < OY)
    // {
    //     // 目的地在当前的东南
    //     degreeAjuste = 180 + 90 + (dest);
    // }
    // else if (DX < OX && DY < OY)
    // {
    //     // 目的地在当前的西南
    //     degreeAjuste = 180 + 90 + (dest);
    // }
    // else if (DX < OX && DY > OY)
    // {
    //     // 目的地在当前的西北
    //     degreeAjuste = 180 + 90 + (dest);
    // }

    // D1x = (120 + (100 * sin((-(180 + 90 - (dest))) * DEG2RAD)));
    // D1y = (120 + (100 * cos((-(180 + 90 - (dest))) * DEG2RAD)));

    // D2x = (120 + (80 * sin((-(180 + 90 - (dest - 7))) * DEG2RAD)));
    // D2y = (120 + (80 * cos((-(180 + 90 - (dest - 7))) * DEG2RAD)));

    // D3x = (120 + (80 * sin((-(180 + 90 - (dest + 7))) * DEG2RAD)));
    // D3y = (120 + (80 * cos((-(180 + 90 - (dest + 7))) * DEG2RAD)));

    // dest = 30;

    D1x = (120 + (100 * sin((-((dest))) * DEG2RAD)));
    D1y = (120 + (100 * cos((-((dest))) * DEG2RAD)));

    D2x = (120 + (80 * sin((-((dest - 7))) * DEG2RAD)));
    D2y = (120 + (80 * cos((-((dest - 7))) * DEG2RAD)));

    D3x = (120 + (80 * sin((-((dest + 7))) * DEG2RAD)));
    D3y = (120 + (80 * cos((-((dest + 7))) * DEG2RAD)));
    // 如果在西北:90度+dest,因为翻转了180度，所以最终再加180度：90+dest+180

    // 如果在东南

    // 如果在西南

    // DDD 3顶点
    // tft.fillCircle(D1x, D1y, 3, RED);
    // tft.fillCircle(D2x, D2y, 3, BLUE);
    // tft.fillCircle(D3x, D3y, 3, GREEN);

    this->tft->fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, GREEN);
}

void ScreenGC9A01::Draw_Compass(float degree)
{
    this->sdeg = degree;
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
    this->tft->fillTriangle(ox, oy, px, py, rx, ry, BLUE);
    // tft.drawTriangle(qx, qy, px, py, rx, ry, BLUE);
    this->tft->fillTriangle(qx, qy, px, py, rx, ry, RED);
}

void ScreenGC9A01::Draw_Cadran_Compass()
{
    // 画背景
    // this->tft->fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
    // this->tft->fillCircle(120, 120, 110, BLACK);
    // 画绿色刻度和斜角
    Draw_green_ticks_bevels();
    // 画60个刻度和方位角
    Draw_points_azimuths();
}
void ScreenGC9A01::cleanNeedleCompass()
{
    this->tft->fillTriangle(ox, oy, px, py, rx, ry, BLACK);
    this->tft->fillTriangle(qx, qy, px, py, rx, ry, BLACK);
    this->tft->fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, BLACK);
}
#pragma endregion DrawingTools

/*


    Tests


*/
#pragma region Tests

void ScreenGC9A01::testGPS()
{
    char *formatChar = (char *)"%-21s";

    if (this->_TSProperties->PropertiesGPS.IsFixValid && this->_TSProperties->PropertiesGPS.UsedSatellites >= 4)
    {
        this->setTextColor(GC9A01A_GREEN, GC9A01A_BLACK, GC9A01A_DARKGREEN, GC9A01A_WHITE);
    }
    else if (this->_TSProperties->PropertiesGPS.IsFixValid && this->_TSProperties->PropertiesGPS.UsedSatellites < 4)
    {
        this->setTextColor(GC9A01A_CYAN, GC9A01A_BLACK, GC9A01A_DARKCYAN, GC9A01A_WHITE);
    }
    else
    {
        this->setTextColor(GC9A01A_RED, GC9A01A_BLACK, GC9A01A_RED, GC9A01A_WHITE);
    }

    this->canvas->setTextSize(1);
    this->setFont(1);

    this->canvas->setCursor(40, 40);
    String strCounterGoodValue = "Good:   " + String(this->_TSProperties->PropertiesGPS.CounterGoodValue);
    this->canvas->printf("%-15s", strCounterGoodValue.c_str());

    this->canvas->setCursor(30, 60);
    String strCounterTotal = "Total:   " + String(this->_TSProperties->PropertiesGPS.CounterTotal);
    this->canvas->printf("%-11s", strCounterTotal.c_str());

    this->canvas->setCursor(15, 85);
    String strUsedSatellite = "Used Sat:   " + String(this->_TSProperties->PropertiesGPS.UsedSatellites);
    this->canvas->printf(formatChar, strUsedSatellite.c_str());

    this->canvas->setCursor(2, 110);
    String strLatitude = "Lat:   " + String(this->_TSProperties->PropertiesGPS.Latitude, 10);
    this->canvas->printf(formatChar, strLatitude.c_str());

    this->canvas->setCursor(2, 130);
    String strLongitude = "Lon:   " + String(this->_TSProperties->PropertiesGPS.Longitude, 10);
    this->canvas->printf(formatChar, strLongitude.c_str());

    this->canvas->setCursor(12, 150);
    String strAltitude = "Alt:   " + String(this->_TSProperties->PropertiesGPS.Altitude, 8);
    this->canvas->printf(formatChar, strAltitude.c_str());

    this->canvas->setCursor(20, 170);
    String strSpeed = "Speed:   " + String(this->_TSProperties->PropertiesGPS.Speed, 4);
    this->canvas->printf(formatChar, strSpeed.c_str());

    this->canvas->setCursor(40, 190);
    String strAccuracy = "Accu:   " + String(this->_TSProperties->PropertiesGPS.Accuracy, 4);
    this->canvas->printf(formatChar, strAccuracy.c_str());

    // this->drawBattery(100, 5, 50, this->_TSProperties->PropertiesBattery.BatteryLevelPourcentage);
}

void ScreenGC9A01::testButtonsScreen()
{
    this->setTextColor();

    this->canvas->setTextSize(3);
    this->canvas->setCursor(35, 50);

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        this->canvas->printf("%-10s", "Accueil");
    }
    else // Anglais
    {
        this->canvas->printf("%-10s", "Home Page");
    }

    this->drawBattery(70, 140, 100, this->_TSProperties->PropertiesBattery.BatteryLevelPourcentage);

    char *formatChar = (char *)"%-33s";

    this->canvas->setCursor(2, 100);
    this->canvas->setTextSize(2);
    this->setTextColor();

    int isButton1Pressed = this->_TSProperties->PropertiesButtons.Button1State;
    int isButton2Pressed = this->_TSProperties->PropertiesButtons.Button2State;

    if (isButton1Pressed == 0 & isButton2Pressed == 0) // not pressed
    {
        /* Nothing */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Aucun bouton pressé !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "No button pressed !");
        }
    }
    else if (isButton1Pressed == 1 & isButton2Pressed == 0) // short press button 1
    {
        /* Change Screen Menu Up */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode)
        {
            this->canvas->printf(formatChar, "Bouton 1 pression COURT !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Button 1 SHORT press !");
        }
    }
    else if (isButton1Pressed == 0 & isButton2Pressed == 1) // short press button 2
    {
        /* Change Screen Menu Down */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Bouton 2 pression COURT !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Button 2 SHORT press !");
        }
    }
    else if (isButton1Pressed == 2 & isButton2Pressed == 0) // long press button 1
    {
        /* Start/Stop Ride */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Bouton 1 pression LONGUE !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Button 1 LONG press !");
        }
    }
    else if (isButton1Pressed == 0 & isButton2Pressed == 2) // long press button 2
    {
        /* Pause/Restart Ride */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Bouton 2 pression LONGUE !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Button 2 LONG press !");
        }
    }
    else if (isButton1Pressed == 3 & isButton2Pressed == 0) // double short press button 1
    {
        /* Trigger The Buzzer */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Bouton 1 DOUBLE pression COURT !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Button 1 DOUBLE SHORT press !");
        }
    }
    else if (isButton1Pressed == 0 & isButton2Pressed == 3) // double short press button 2
    {
        /* Trigger The Buzzer */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Bouton 2 DOUBLE pression COURT !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Button 2 DOUBLE SHORT press !");
        }
    }
    else if (isButton1Pressed == 1 & isButton2Pressed == 1) // short press button 1 and 2
    {
        /* Activate GoHome Mode */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Boutons 1 and 2 pression COURT !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Buttons 1 and 2 SHORT press !");
        }
    }
    else if (isButton1Pressed == 2 & isButton2Pressed == 2) // long press button 1 and 2
    {
        /* Trigger The Buzzer */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "Boutons 1 and 2 pression LONGUE !");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "Buttons 1 and 2 LONG press !");
        }
    }
    else
    {
        /* Nothing Good Happened... */
        if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
        {
            this->canvas->printf(formatChar, "BOUTONS ERREUR !!!");
        }
        else // Anglais
        {
            this->canvas->printf(formatChar, "BUTTONS ERROR !!!");
        }
    }
}

#pragma endregion Tests