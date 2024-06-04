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
    // Lorsque vous accédez à l'écran de la boussole pour la première fois ou que vous y entrez à nouveau,
    // préparez la boussole et dessinez la boussole une seule fois, puis ne dessinez plus la boussole dans chaque boucle.
    if (!this->status)
    {
        Serial.println("Compass---->Entrer");
        // Dessine une boussole
        this->tft->setTextColor(WHITE, GREY);
        this->tft->fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
        this->tft->fillCircle(120, 120, 110, BLACK);
        // Mettre à jour l'état de l'écran
        this->status = true;
    }
    // this->tft->fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
    // this->tft->fillCircle(120, 120, 110, BLACK);
    // Lorsque la valeur change, la dernière ombre dessinée est effacée.
    // S'il n'y a pas de changement, la forme actuelle est conservée.
    if (lastDegree != degree)
    {
        Serial.println("Compass---->Change");
        lastDegree = degree; // Mettre à jour la valeur
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

void ScreenGC9A01::drawBoutonTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3)
{
    this->canvas->fillTriangle(x1, y1, x2, y2, x3, y3, GREEN);
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
    this->tft->fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, BLACK);

    // // Selon la position du point de destination par rapport aux coordonnées actuelles,
    // // il est divisé en quatre situations : nord-est, sud-est, sud-ouest, nord-ouest des coordonnées actuelles,
    // // l'écran actuel est inversé de 180 degrés, c'est-à-dire que les coordonnées 0,0 changent ;
    // // du haut à gauche vers le bas à droite ; si Nord-Est : 90 degrés-dest,
    // // car l'ensemble est inversé de 180 degrés, donc au final 180 degrés seront ajoutés : 90-dest+180
    // float degreeAjuste = 0.0;
    // float DX = _TSProperties->PropertiesCurrentRide.longitude_destination;
    // float DY = _TSProperties->PropertiesCurrentRide.latitude_destination;
    // float OX = _TSProperties->PropertiesGPS.Longitude;
    // float OY = _TSProperties->PropertiesGPS.Latitude;

    // if (DX > OX && DY > OY)
    // {
    //     // La destination est dans le Nord-Est actuel
    //     degreeAjuste = 180 + 90 - (dest);
    // }
    // else if (DX > OX && DY < OY)
    // {
    //     // La destination est dans le sud-est actuel
    //     degreeAjuste = 180 + 90 + (dest);
    // }
    // else if (DX < OX && DY < OY)
    // {
    //     // La destination est dans le sud-ouest actuel
    //     degreeAjuste = 180 + 90 + (dest);
    // }
    // else if (DX < OX && DY > OY)
    // {
    //     // La destination est dans le nord-ouest actuel
    //     degreeAjuste = 180 + 90 + (dest);
    // }

    // D1x = (120 + (100 * sin((-(180 + 90 - (dest))) * DEG2RAD)));
    // D1y = (120 + (100 * cos((-(180 + 90 - (dest))) * DEG2RAD)));

    // D2x = (120 + (80 * sin((-(180 + 90 - (dest - 7))) * DEG2RAD)));
    // D2y = (120 + (80 * cos((-(180 + 90 - (dest - 7))) * DEG2RAD)));

    // D3x = (120 + (80 * sin((-(180 + 90 - (dest + 7))) * DEG2RAD)));
    // D3y = (120 + (80 * cos((-(180 + 90 - (dest + 7))) * DEG2RAD)));

    // dest = 30;

    D1x = (120 + (100 * sin((180 - ((dest))) * DEG2RAD)));
    D1y = (120 + (100 * cos((180 - ((dest))) * DEG2RAD)));

    D2x = (120 + (80 * sin((180 - ((dest - 7))) * DEG2RAD)));
    D2y = (120 + (80 * cos((180 - ((dest - 7))) * DEG2RAD)));

    D3x = (120 + (80 * sin((180 - ((dest + 7))) * DEG2RAD)));
    D3y = (120 + (80 * cos((180 - ((dest + 7))) * DEG2RAD)));

    // tft.fillCircle(D1x, D1y, 3, RED);
    // tft.fillCircle(D2x, D2y, 3, BLUE);
    // tft.fillCircle(D3x, D3y, 3, GREEN);

    this->tft->fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, GREEN);
}

void ScreenGC9A01::Draw_Compass(float degree)
{
    this->sdeg = degree;
    // Point A
    // tft.drawLine(ox, oy, 120, 121, BLACK); // erase hour and minute hand positions every minute
    ox = (120 + (70 * sin((-degree) * DEG2RAD)));
    oy = (120 + (70 * cos((-degree) * DEG2RAD)));
    // tft.drawLine(ox, oy, 120, 121, GREEN);

    // point B
    // tft.drawLine(px, py, 120, 121, BLACK); // erase hour and minute hand positions every minute
    px = (120 + 15 * sin((90 - degree) * DEG2RAD));
    py = (120 + 15 * cos((90 - degree) * DEG2RAD));
    // tft.drawLine(px, py, 120, 121, YELLOW);

    // Point C
    // tft.drawLine(qx, qy, 120, 121, BLACK); // erase hour and minute hand positions every minute
    qx = (120 + 70 * sin((180 - degree) * DEG2RAD));
    qy = (120 + 70 * cos((180 - degree) * DEG2RAD));
    // tft.drawLine(qx, qy, 120, 121, GREEN);

    // Point D
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
    // draw background
    // this->tft->fillCircle(120, 120, 118, BORDEAUX); // creates outer ring
    // this->tft->fillCircle(120, 120, 110, BLACK);
    // Dessinez des graduations vertes et des biseaux
    Draw_green_ticks_bevels();
    // Dessinez 60 échelles et azimuts
    Draw_points_azimuths();
}

void ScreenGC9A01::cleanNeedleCompass()
{
    this->tft->fillTriangle(ox, oy, px, py, rx, ry, BLACK);
    this->tft->fillTriangle(qx, qy, px, py, rx, ry, BLACK);
    this->tft->fillTriangle(D1x, D1y, D2x, D2y, D3x, D3y, BLACK);
}

void ScreenGC9A01::drawFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    this->tft->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void ScreenGC9A01::drawGoHomePage()
{
    // Triangle Vert point a Home
    this->Draw_Destination(calculerDirectionDegree(this->_TSProperties->PropertiesGPS.Home_Longitude, this->_TSProperties->PropertiesGPS.Home_Latitude));
    // flèche pointant vers le nord
    this->Draw_FlecheNord(this->_TSProperties->PropertiesCompass.heading);
    this->Draw_Distance();
}
float ScreenGC9A01::calculerDirectionDegree(float p_longitude_destination, float p_latitude_destination)
{
    double newDirectionDestinationRAD = 0.0;
    float newDirectionDestinationDegree = 0.0;

    float newHeading = _TSProperties->PropertiesCompass.heading;
    float DX = p_longitude_destination;                // longitude de destination
    float DY = p_latitude_destination;                 // dimension de destination
    float OX = _TSProperties->PropertiesGPS.Longitude; // longitude actuelle
    float OY = _TSProperties->PropertiesGPS.Latitude;  // dimension actuelle

    float param1 = (90 - OY) * (DY - OY);
    float param2 = sqrt((90 - DY) * (90 - DY));
    float param3 = sqrt((DX - OX) * (DX - OX) + (DY - OY) * (DY - OY));
    newDirectionDestinationRAD = cos(param1 / (param2 * param3));
    newDirectionDestinationDegree = newDirectionDestinationRAD * RAD2DEG;
    newDirectionDestinationDegree += 180;
    newDirectionDestinationDegree += _TSProperties->PropertiesCompass.heading;
    Serial.println("Degree---->" + String(newDirectionDestinationDegree));
    return newDirectionDestinationDegree;
}

void ScreenGC9A01::Draw_FlecheNord(float azimuth)
{
    this->tft->drawLine(directionNordx1, directionNordy1, directionNordEndX, directionNordEndY, BLACK);
    this->tft->fillTriangle(directionNordx1, directionNordy1, directionNordx2, directionNordy2, directionNordx3, directionNordy3, BLACK);

    // TODO:
    this->sdeg = azimuth;
    // Point A
    directionNordx1 = (120 + (70 * sin((-azimuth) * DEG2RAD)));
    directionNordy1 = (120 + (70 * cos((-azimuth) * DEG2RAD)));

    // Point end
    directionNordEndX = (120 + 70 * sin((180 - azimuth) * DEG2RAD));
    directionNordEndY = (120 + 70 * cos((180 - azimuth) * DEG2RAD));

    this->tft->drawLine(directionNordx1, directionNordy1, directionNordEndX, directionNordEndY, WHITE);

    directionNordx2 = (120 + (50 * sin((-((azimuth - 10))) * DEG2RAD)));
    directionNordy2 = (120 + (50 * cos((-((azimuth - 10))) * DEG2RAD)));

    directionNordx3 = (120 + (50 * sin((-((azimuth + 10))) * DEG2RAD)));
    directionNordy3 = (120 + (50 * cos((-((azimuth + 10))) * DEG2RAD)));

    this->tft->fillTriangle(directionNordx1, directionNordy1, directionNordx2, directionNordy2, directionNordx3, directionNordy3, WHITE);
}

void ScreenGC9A01::Draw_Distance()
{
    this->tft->setTextSize(3);
    this->tft->setTextColor(WHITE);
    this->tft->setCursor(120 - 35, 120 + 70);
    this->tft->print(String(this->_TSProperties->PropertiesGPS.Distance2Home) + " m");
    // this->tft->print("1500 m");
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