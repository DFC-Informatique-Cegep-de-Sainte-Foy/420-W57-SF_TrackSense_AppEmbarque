
// #define watchESP
// #define TFT_Test_Cours_01 // 文字、背景色
// #define TFT_Test_Cours_02 // 飞行员仪表盘
// #define TFT_Test_Cours_Compass_needle // 旋转sprite
#define Compass_rotated // 旋转sprite
// #define Render_Image_font // jpg照片 -> 数组 ->渲染
// #define Render_Image_TTGO
// #define Sprites
// #define Rotated_Sprites

#ifdef watchESP
#include <TFT_eSPI.h>
#include "fonts.h"
#include "time.h"
// #include "RTClib.h"

// RTC_DS3231 rtc;

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
    img.createSprite(240, 240); // 原值 240
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
    // DateTime now = rtc.now();
    int now = 17;
    // angle = now.second() * 6;
    // angle = 12;

    // s = String(now.second());
    // m = String(now.minute());
    // h = String(now.hour());
    s = String(now + 10);
    m = String(now * 2);
    h = String(now);

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
    d1 = "0";
    d2 = String("17");

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
    m1 = "0";
    m2 = String("7");

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
        img.drawString(days[4], circle, 120, 2);

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

#ifdef TFT_Test_Cours_01
#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();
void setup()
{
    tft.init();
    // 设置屏幕背景色
    tft.fillScreen(TFT_GREEN);
    // 显示文字位置和大小
    tft.setTextColor(TFT_RED);
    tft.drawString("Hello World!", 100, 10);
    tft.setTextColor(TFT_BLUE);
    tft.drawString("Font 2", 100, 24, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Font 4", 100, 54, 4);
    // tft.drawString("Font 6", 100, 100, 6);
    // tft.drawString("Font 7", 100, 150, 7);
    // tft.drawString("Font 8", 100, 170, 8);
    tft.setTextColor(TFT_YELLOW, TFT_BROWN);
}

int number = 0;
void loop()
{
    tft.drawString(String(number), 30, 100, 7);
    number++;
    delay(1000);
}
#endif

#ifdef TFT_Test_Cours_02

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

int start[12];
int startP[60];
int angle = 0;
float x[360]; // outer point
float y[360];

float lx[360]; // text
float ly[360];

float px[360]; // ineer point
float py[360];

String cc[10] = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90"};

#define color1 TFT_WHITE
#define color2 0x8410
#define color3 0x3828
#define color4 0xF2DF
#define color5 0x00A3
#define color6 0x00A3
void setup()
{
    tft.init();
    // 设置屏幕背景色
    tft.fillScreen(TFT_BLACK);
    sprite.createSprite(239, 239); // 创建一块背景，x轴239个像素，y轴239个像素
    sprite.fillSprite(TFT_CYAN);
    sprite.pushSprite(0, 0);
    // tft.drawPixel(60, 100, TFT_RED);
    // tft.drawPixel(120, 120, TFT_GREEN);

    // tft.drawLine(0, 0, 120, 120, TFT_WHITE);
    // tft.drawLine(120, 120, 240, 240, TFT_RED);

    // tft.drawCircle(50, 140, 40, TFT_YELLOW);
    // tft.fillCircle(180, 180, 30, TFT_YELLOW);

    // tft.drawRect(20, 40, 30, 50, TFT_BLUE);
    // tft.fillRect(160, 40, 30, 50, TFT_BLUE);

    // // 显示文字位置和大小
    // tft.setTextColor(TFT_RED);
    // tft.drawString("Hello World!", 100, 10);
    // tft.setTextColor(TFT_BLUE);
    // tft.drawString("Font 2", 100, 24, 2);
    // tft.setTextColor(TFT_YELLOW);
    // tft.drawString("Font 4", 100, 54, 4);
    // // tft.drawString("Font 6", 100, 100, 6);
    // // tft.drawString("Font 7", 100, 150, 7);
    // // tft.drawString("Font 8", 100, 170, 8);
    // tft.setTextColor(TFT_YELLOW, TFT_BROWN);
    // 中心方框
    tft.drawRect(117, 117, 7, 7, TFT_YELLOW);
    // 中心水平线
    tft.fillRect(25, 117, 46, 7, TFT_YELLOW);
    tft.fillRect(170, 117, 46, 7, TFT_YELLOW);
    tft.fillRect(64, 124, 7, 15, TFT_YELLOW);
    tft.fillRect(170, 124, 7, 15, TFT_YELLOW);
    // Pitch échelle

    // Yaw échelle

    // Roll échelle

    // Destination
    tft.drawTriangle(119, 25, 111, 40, 127, 40, TFT_CYAN);
    // Compass

    // 画刻度
    for (int i = 0; i < 10; i++)
        if (start[i] + angle < 360)
        {
            sprite.drawString(cc[i], x[start[i] + angle], y[start[i] + angle]);
            sprite.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
        }
        else
        {
            sprite.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360]);
            sprite.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
        }

    float v = angle / 3.60;
    for (int i = 0; i < 60; i++)
        if (startP[i] + angle < 360)
            sprite.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color1);
        else
            sprite.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color1);
}

int number = 0;
void loop()
{
    // tft.drawString(String(number), 30, 100, 7);
    // number++;
    // delay(1000);
}
#endif

#ifdef TFT_Test_Cours_Compass_needle

#include <TFT_eSPI.h>
#include <SPI.h>
#include <font.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite needle = TFT_eSprite(&tft);
TFT_eSprite compass = TFT_eSprite(&tft);
TFT_eSprite back = TFT_eSprite(&tft);

void setup()
{
    tft.init();
    // 设置屏幕背景色
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(0);
    // 设定画笔位置
    // tft.setPivot(85, 85);           // 屏幕中心
    needle.createSprite(20, 80);    // 创建一块x轴20个像素，y轴80个像素的红色方块
    back.createSprite(239, 239);    // 创建一块背景，x轴239个像素，y轴239个像素
    compass.createSprite(200, 200); // 创建一块背景，x轴230个像素，y轴230个像素
}
int angle = 0;
void loop()
{
    back.fillSprite(TFT_BLACK);   // 给背景填充黑色
    needle.fillSprite(TFT_BLACK); // 给方块填充红色

    // compass gauge
    // back.fillCircle(tft.width() / 2, tft.height() / 2, 100, TFT_SILVER); // 在整个屏幕的中心点为圆点，绘制半径120的圆面

    // compass
    compass.drawLine(120, 15, 120, 40, TFT_RED);
    compass.drawString("N", 111, 0, 4);

    // for (int i = 0; i < 12; i++)
    // {
    //     if (start[i] + angle < 360)
    //     {
    //         img.drawString(cc[i], x[start[i] + angle], y[start[i] + angle]);
    //         img.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
    //     }
    //     else
    //     {
    //         img.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360]);
    //         img.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
    //     }
    // }

    // for (int i = 0; i < 60; i++)
    // {
    //     if (startP[i] + angle < 360)
    //     {
    //         img.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color2);
    //     }
    //     else
    //     {
    //         img.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color2);
    //     }
    // }

    // needle
    needle.drawWedgeLine(10, 0, 10, 40, 1, 10, TFT_RED);   // 在needle上画一个红色指针
    needle.drawWedgeLine(10, 40, 10, 80, 10, 1, TFT_BLUE); // 在needle上画一个蓝色指针
    needle.fillCircle(10, 40, 10, TFT_WHITE);              // 指南针中心的白色圆点

    // publish sprites
    compass.pushToSprite(&back, 0, 0, TFT_BLACK);
    // compass.pushRotated(&back, angle, TFT_BLACK);
    needle.pushRotated(&back, angle, TFT_BLACK); // 旋转angle度后显示在另一个“back”的sprite上，注意：以sprite图形的中心旋转,黑色不会被渲染
    back.pushSprite(0, 0);                       // 最后将“back”sprite从（0,0）点显示在屏幕上

    angle++;
    if (angle == 360)
    {
        angle = 0;
    }
}
#endif

#ifdef Render_Image_font
/*
    渲染一个图片步骤：
    1 - 从网上找到图片，调整大小，并转换成.h文件中的数组
    2 - 创建一个TFT_eSPI对象
    3 - 使用tft.pushImage(x轴，y轴，图片宽，图片高，图片名)此函数来渲染图片
    注：  图片宽和图片高必须完全与图片本身尺寸完全一致！！！

*/
/*
    使用某字体渲染
    1 - 从网站上找到喜欢的字体和配置
    2 - 将头文件下载到本地
    3 - 在项目中引入
    4 - 使用函数：
        tft.setFreeFont(字体名：在头文件最后面);
        tft.drawString("String",x轴，y轴);
*/

#include "TFT_eSPI.h"
#include "house.h"
#include "font.h"
#include "compass_transparant.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
// TFT_eSprite back = TFT_eSprite(&tft);

int imageW = 240;
int imageH = 240;

int screenW = 240;
int screenH = 240;
int m = imageW;

void setup()
{
    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);

    // 渲染图片
    tft.fillScreen(TFT_BLACK);
    tft.pushImage(0, 0, 128, 128, house); // 图片宽和图片高必须完全与图片本身尺寸完全一致！！！

    // 使用字体
    tft.setFreeFont(&Orbitron_Medium_28);
    tft.drawString("House", 120, 120);
}

int angle = 0;
void loop()
{
}
#endif

#ifdef Render_Image_TTGO
#include "TFT_eSPI.h"

TFT_eSPI lcd = TFT_eSPI();
TFT_eSprite compass = TFT_eSprite(&lcd); // 仪表盘
TFT_eSprite back = TFT_eSprite(&lcd);    // 背景

#define color1 TFT_WHITE
#define color2 0x8410
#define color3 0x3828
#define color4 0xF2DF
#define color5 0x00A3
#define color6 0x00A3

// circuit gauge variables

int sx = 96; // center x
int sy = 96; // center y
int r = 100;
double rad = 0.01745;
float x[360]; // outer point
float y[360];

float px[360]; // ineer point
float py[360];

float lx[360]; // text
float ly[360];

int start[12];
int startP[60];

String cc[10] = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90"};

void setup()
{
    lcd.init();
    lcd.setRotation(0);
    lcd.fillScreen(TFT_BLACK);

    // 设定画笔位置
    // 设定画笔位置
    lcd.setPivot(85, 85);           // 屏幕中心
    compass.createSprite(238, 238); // 创建一块x轴20个像素，y轴80个像素的红色方块
    back.createSprite(239, 239);    // 创建一块背景，x轴239个像素，y轴239个像素

    // lcd.setPivot(120, 120);        // 屏幕中心
    // sprite.createSprite(238, 238); // 创建一块背景，x轴239个像素，y轴239个像素
    // sprite.fillSprite(TFT_BLUE);
    // sprite.setTextDatum(4);

    int b = 0;
    int b2 = 0;

    for (int i = 0; i < 360; i++)
    {
        x[i] = (r * cos(rad * i)) + sx;
        y[i] = (r * sin(rad * i)) + sy;
        px[i] = ((r - 12) * cos(rad * i)) + sx;
        py[i] = ((r - 12) * sin(rad * i)) + sy;

        lx[i] = ((r - 18) * cos(rad * i)) + sx;
        ly[i] = ((r - 18) * sin(rad * i)) + sy;

        if (i % 36 == 0)
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

int angle = 0;
int values[20] = {0};
int values2[20] = {0};
int curent = 0;
int counter = 0;
int counter_last = 2;
int calib = 20;

int curentTime = 0;
int period = 500;

int term[6] = {0};
int term2[6] = {0};

int t[6] = {0};
int t2[6] = {0};
bool c = 0;
bool c1;
int number = 0;
bool onOff = 0;
int debounce = 0;

void loop()
{

    angle++;
    if (angle == 360)
    {
        angle = 0;
    }

    // // sprite.fillRoundRect(0, 0, 140, 140, 3, color6);
    // sprite.fillRoundRect(0, 0, 200, 200, 1, TFT_CYAN);
    // sprite.setTextFont(0);

    // 画刻度
    for (int i = 0; i < 10; i++)
        if (start[i] + angle < 360)
        {
            compass.drawString(cc[i], x[start[i] + angle], y[start[i] + angle]);
            compass.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
        }
        else
        {
            compass.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360]);
            compass.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
        }

    float v = angle / 3.60;
    for (int i = 0; i < 60; i++)
        if (startP[i] + angle < 360)
            compass.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color1);
        else
            compass.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color1);

    // // sprite.fillTriangle(120, 15, 127, 30, 113, 30, TFT_RED); // 红色三角形，指北

    compass.pushRotated(&back, angle, TFT_BLACK);
    back.pushSprite(0, 0);
}
#endif

#ifdef Compass_rotated
/*
  ESP32_Compass - Programme to demonstrate functionality of HMC5883 triple
  axis magnetic sensor in conjunction with an LCD display.  Graphical code
  adaptation from Volos Projects https://www.youtube.com/@VolosProjects
  for rotating display.
  https://www.youtube.com/c/jordanrubin6502
  2023 Jordan Rubin.
*/

#include <Arduino.h>
#include <TFT_eSPI.h>
// #include <QMC5883LCompass.h>
#include "fonts.h"

#define color1 TFT_WHITE
#define color2 0x8410
#define color3 TFT_ORANGE
#define color4 0x15B3
#define color5 0x00A3

// QMC5883LCompass compass;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

float lastValue = 0;
double rad = 0.01745;
float x[360];
float y[360];
float px[360];
float py[360];
float lx[360];
float ly[360];
int r = 100;
int sx = 110;
int sy = 116;
int angle = 1;
int lastAngle = 0;
String lastHeading = "";
String cc[12] = {"0", "30", "60", "90", "120", "150", "180", "210", "240", "270", "300", "330"};
int start[12];  // was 10
int startP[60]; // was 60

void setup()
{
    Serial.begin(115200);
    // compass.init();
    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    // tft.setTextDatum(4);
    // tft.setTextColor(color4, TFT_BLACK);
    // tft.drawString("COMPASS PROGRAMME", 120, 4);
    img.setSwapBytes(true);
    // img.createSprite(220, 105);
    img.createSprite(220, 220);
    img.setTextDatum(4);
    int b, b2 = 0;
    for (int i = 0; i < 360; i++)
    {
        x[i] = (r * cos(rad * i)) + sx;
        y[i] = (r * sin(rad * i)) + sy;
        px[i] = ((r - 16) * cos(rad * i)) + sx;
        py[i] = ((r - 16) * sin(rad * i)) + sy;
        lx[i] = ((r - 24) * cos(rad * i)) + sx;
        ly[i] = ((r - 24) * sin(rad * i)) + sy;
        if (i % 30 == 0)
        { // 12th was 10th at %36
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

void loop()
{
    // compass.read();
    // int value = compass.getAzimuth();
    // angle = value;
    // angle = 90;
    // String heading;
    // if (((angle >= 338) && (angle <= 359)) || ((angle >= 0) && (angle <= 22)))
    // {
    //     heading = " N";
    // }
    // else if ((angle >= 23) && (angle <= 67))
    // {
    //     heading = "NE";
    // }
    // else if ((angle >= 68) && (angle <= 113))
    // {
    //     heading = " E";
    // }
    // else if ((angle >= 114) && (angle <= 157))
    // {
    //     heading = "SE";
    // }
    // else if ((angle >= 158) && (angle <= 202))
    // {
    //     heading = " S";
    // }
    // else if ((angle >= 203) && (angle <= 248))
    // {
    //     heading = "SW";
    // }
    // else if ((angle >= 249) && (angle <= 292))
    // {
    //     heading = " W";
    // }
    // else if ((angle >= 293) && (angle <= 337))
    // {
    //     heading = "NW";
    // }
    // if (heading != lastHeading)
    // {
    //     tft.setTextColor(TFT_BLACK, TFT_BLACK);
    //     tft.setFreeFont(&Slackey_Regular_16);
    //     tft.drawString(lastHeading, 120, 18);
    //     tft.setTextColor(color3, TFT_BLACK);
    //     tft.setFreeFont(&Slackey_Regular_16);
    //     tft.drawString(heading, 120, 18);
    //     lastHeading = heading;
    // }
    Serial.print("A: ");
    Serial.println(angle);
    if (angle != lastAngle)
    {
        lastAngle = angle;
        angle = (angle - 270) * -1;
        if (angle < 0)
        {
            angle = angle + 360;
        }
        // img.fillSprite(TFT_BLACK);
        // img.fillCircle(sx, sy, 124, color5);
        // img.setTextColor(TFT_WHITE, color5);
        // img.setFreeFont(&FreeSans9pt7b);
        for (int i = 0; i < 12; i++)
        {
            if (start[i] + angle < 360)
            {
                img.drawString(cc[i], x[start[i] + angle], y[start[i] + angle]);
                img.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
            }
            else
            {
                img.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360]);
                img.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
            }
        }
        // img.setFreeFont(&DSEG7_Modern_Bold_20);
        // img.drawString(String(angle), sx, sy - 22);
        // img.setTextFont(0);
        // img.drawString("AZMUTH", sx, sy - 42);
        for (int i = 0; i < 60; i++)
        {
            if (startP[i] + angle < 360)
            {
                img.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color2);
            }
            else
            {
                img.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color2);
            }
        }
        img.fillTriangle(sx - 1, sy - 70, sx - 5, sy - 56, sx + 4, sy - 56, TFT_ORANGE);
        // img.pushSprite(10, 30);
        img.pushSprite(25, 25);
        // img.pushRotated(&tft);
    }
    // delay(250);
    angle++;
}
#endif

#ifdef Sprites
/*
  ESP32_Compass - Programme to demonstrate functionality of HMC5883 triple
  axis magnetic sensor in conjunction with an LCD display.  Graphical code
  adaptation from Volos Projects https://www.youtube.com/@VolosProjects
  for rotating display.
  https://www.youtube.com/c/jordanrubin6502
  2023 Jordan Rubin.
*/

#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr_main = TFT_eSprite(&tft);
TFT_eSprite spr_01 = TFT_eSprite(&tft);

#define full_width 240
#define full_height 240

void setup()
{
    // 1 - 不能缺少的步骤，初始化屏幕
    tft.init();
    // 2 - 设置屏幕朝向
    tft.setRotation(2);
    spr_main.createSprite(full_width, full_height, 8); // 创建一个根屏幕大小一样的Sprite
    spr_main.setSwapBytes(true);
}

void loop()
{
    tft.fillScreen(TFT_BLUE);
    // 设置缓冲区颜色
    spr_main.fillSprite(TFT_RED);
    // 用sprite来画圆，而不是让TFT来画圆
    spr_main.fillCircle(120, 120, 200, TFT_BROWN);
    // 此时，整个Sprite上就已经画好了一个棕色的原型，坐标是sprite的坐标
    // 把整个sprite渲染到TFT上
    spr_main.pushSprite(0, 0); // 这里的坐标是整个屏幕的坐标
}
#endif

#ifdef Rotated_Sprites
/*====================================================================================

  This example draws a jpeg image in a Sprite then plot a rotated copy of the Sprite
  to the TFT.

  The jpeg used in in the sketch Data folder (press Ctrl+K to see folder)

  The jpeg must be uploaded to the ESP8266 or ESP32 SPIFFS by using the Tools menu
  sketch data upload option of the Arduino IDE. If you do not have that option it can
  be added. Close the Serial Monitor window before uploading to avoid an error message!

  To add the upload option for the ESP8266 see:
  http://www.esp8266.com/viewtopic.php?f=32&t=10081
  https://github.com/esp8266/arduino-esp8266fs-plugin/releases

  To add the upload option for the ESP32 see:
  https://github.com/me-no-dev/arduino-esp32fs-plugin

  Created by Bodmer 6/1/19 as an example to the TFT_eSPI library:
  https://github.com/Bodmer/TFT_eSPI

  Extension functions in the TFT_eFEX library are used to list SPIFFS files and render
  the jpeg to the TFT and to the Sprite:
  https://github.com/Bodmer/TFT_eFEX

  To render the Jpeg image the JPEGDecoder library is needed, this can be obtained
  with the IDE library manager, or downloaded from here:
  https://github.com/Bodmer/JPEGDecoder

  ==================================================================================*/

//====================================================================================
//                                  Libraries
//====================================================================================
// Call up the SPIFFS FLASH filing system, this is part of the ESP Core
#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
#include "SPIFFS.h" // Needed for ESP32 only
#endif

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>                // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();           // Invoke custom library
TFT_eSprite spr = TFT_eSprite(&tft); // Create Sprite object "spr" with pointer to "tft" object

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>          // Include the function extension library
TFT_eFEX fex = TFT_eFEX(&tft); // Create TFT_eFX object "fex" with pointer to "tft" object

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
    Serial.begin(250000); // Used for messages

    tft.begin();
    tft.setRotation(0); // 0 & 2 Portrait. 1 & 3 landscape
    tft.fillScreen(TFT_BLACK);

    // Create a sprite to hold the jpeg (or part of it)
    spr.createSprite(80, 64);

    // Initialise SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS initialisation failed!");
        while (1)
            yield(); // Stay here twiddling thumbs waiting
    }
    Serial.println("\r\nInitialisation done.\r\n");

    // Lists the files so you can see what is in the SPIFFS
    fex.listSPIFFS();

    // Note the / before the SPIFFS file name must be present, this means the file is in
    // the root directory of the SPIFFS, e.g. "/tiger.jpg" for a file called "tiger.jpg"

    // Send jpeg info to serial port
    fex.jpegInfo("/Eye_80x64.jpg");

    // Draw jpeg image in Sprite spr at 0,0
    fex.drawJpeg("/Eye_80x64.jpg", 0, 0, &spr);
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{

    tft.fillScreen(random(0xFFFF));

    // Set the TFT pivot point to the centre of the screen
    tft.setPivot(tft.width() / 2, tft.height() / 2);

    // Set Sprite pivot point to centre of Sprite
    spr.setPivot(spr.width() / 2, spr.height() / 2);

    // Push Sprite to the TFT at 0,0 (not rotated)
    spr.pushSprite(0, 0);

    delay(1000);

    // Push copies of Sprite rotated through increasing angles 0-360 degrees
    // with 45 degree increments
    for (int16_t angle = 0; angle <= 360; angle += 45)
    {
        spr.pushRotated(angle);
        delay(500);
    }

    delay(2000);

    // Move Sprite pivot to a point above the image at 40,-60
    // (Note: Top left corner is Sprite coordinate 0,0)
    // The TFT pivot point has already been set to middle of screen.
    /*                    .Pivot point at 40,-60
                          ^
                          |
                         -60
                    < 40 >|
                    ______V______
                   |             |
                   |   Sprite    |
                   |_____________|
    */
    spr.setPivot(40, -60);

    // Push Sprite to screen rotated about the new pivot points
    // negative angle rotates Sprite anticlockwise
    for (int16_t angle = 330; angle >= 0; angle -= 30)
    {
        spr.pushRotated(angle);
        yield(); // Stop watchdog triggering
    }

    delay(5000);
}
//====================================================================================
#endif