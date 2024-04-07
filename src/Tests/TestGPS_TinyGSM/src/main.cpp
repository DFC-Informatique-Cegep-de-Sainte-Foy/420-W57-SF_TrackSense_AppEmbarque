/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/lilygo-t-sim7000g-esp32-gps-data/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#define DISABLE_GPS_AND_MODEM FALSE

void printGPSOnScreen(float lat, float lon, float speed, float alt, float accuracy, int usat, int counterTotal, int counterGoodValue);
void enableGPSAndModem();
void disableGPSAndModem();
void showScreenWaitingPage();

#define TINY_GSM_MODEM_SIM7600  // change le modele du T-SIM
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// LilyGO T-SIM7000G Pinout
#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4

#define LED_PIN 12

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands
#define SerialAT Serial1

TinyGsm modem(SerialAT);

/* Screen */
#define TFT_BL_BLK -1
#define TFT_CS_SS 05
#define TFT_DC 00
#define TFT_RES_RST -1
#define TFT_SDA_DIN_MOSI 23
#define TFT_SCL_CLK_SCK 18
// #define TFT_MISO 19

Adafruit_GC9A01A tft(TFT_CS_SS, TFT_DC, TFT_SDA_DIN_MOSI, TFT_SCL_CLK_SCK, TFT_RES_RST);

void setup()
{
    SerialMon.begin(115200);
    SerialMon.println("Place your board outside to catch satelite signal");

    /* Set Screen */
    tft.begin();
    showScreenWaitingPage();

    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    // Turn on the modem
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);

    delay(1000);

    // Set module baud rate and UART pins
    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    if (!modem.restart())
    {
        Serial.println("Failed to restart modem, attempting to continue without restarting");
        modem.init();
    }

    // Print modem info
    String modemName = modem.getModemName();
    delay(500);
    SerialMon.println("Modem Name: " + modemName); // SIMCom SIM7000

    String modemInfo = modem.getModemInfo();
    delay(500);
    SerialMon.println("Modem Info: " + modemInfo); // vide
}

void loop()
{
    // Set SIM7000G GPIO4 HIGH ,turn on GPS power
    // CMD:AT+SGPIO=0,4,1,1
    // Only in version 20200415 is there a function to control GPS power
    modem.sendAT("+SGPIO=0,4,1,1");
    if (modem.waitResponse(10000L) != 1)
    {
        SerialMon.println(" SGPIO=0,4,1,1 false "); // SGPIO=0,4,1,1 false
    }

    modem.enableGPS();

    delay(15000);

    // Init String on Screen
    tft.fillScreen(GC9A01A_BLACK);
    printGPSOnScreen(0, 0, 0, 0, 0, 0, -1, -1);

    float lat = 0;
    float lon = 0;
    float speed = 0;
    float alt = 0;
    int vsat = 0;
    int usat = 0;
    float accuracy = 0;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;

    int counterGoodValue = 0;
    int counterTotal = 1;

    for (int8_t i = 100; i; i--)
    {
        /* Print on LCD Screen */
        printGPSOnScreen(lat, lon, speed, alt, accuracy, usat, counterTotal, counterGoodValue);

        SerialMon.println("=======================================");
        SerialMon.println("Requesting current GPS/GNSS/GLONASS location");

        if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                         &year, &month, &day, &hour, &min, &sec))
        {
            SerialMon.println("Latitude: " + String(lat, 10) + "\tLongitude: " + String(lon, 10));
            SerialMon.println("Speed: " + String(speed) + "\tAltitude: " + String(alt));
            SerialMon.println("Visible Satellites: " + String(vsat) + "\tUsed Satellites: " + String(usat));
            SerialMon.println("Accuracy: " + String(accuracy));
            SerialMon.println("Year: " + String(year) + "\tMonth: " + String(month) + "\tDay: " + String(day));
            SerialMon.println("Hour: " + String(hour) + "\tMinute: " + String(min) + "\tSecond: " + String(sec));

            SerialMon.println();
            SerialMon.println("Retrieving GPS/GNSS/GLONASS location again as a string");
            String gps_raw = modem.getGPSraw();
            SerialMon.println("GPS/GNSS Based Location String: " + gps_raw);

            counterGoodValue++;

            SerialMon.println("=======================================");
            delay(1000L);
        }
        else
        {
            SerialMon.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 10s.");
            delay(10000L);
        }

        counterTotal++;
    }

    printGPSOnScreen(lat, lon, speed, alt, accuracy, usat, counterTotal, counterGoodValue);

    SerialMon.println("Disabling GPS");
    modem.disableGPS();

    // Set SIM7000G GPIO4 LOW ,turn off GPS power
    // CMD:AT+SGPIO=0,4,1,0
    // Only in version 20200415 is there a function to control GPS power
    modem.sendAT("+SGPIO=0,4,1,0");
    if (modem.waitResponse(10000L) != 1)
    {
        SerialMon.println(" SGPIO=0,4,1,0 false ");
    }

    delay(200);
    // Do nothing forevermore
    while (true)
    {
        modem.maintain();
    }
}

void showScreenWaitingPage()
{
    tft.fillScreen(GC9A01A_BLACK);
    tft.setRotation(0);
    tft.setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
    tft.setCursor(30, 40);
    tft.setTextSize(3);
    tft.printf("%-11s", "TrackSense");
    tft.setCursor(10, 150);
    tft.printf("%-13s", "Initializing");
    tft.setCursor(90, 180);
    tft.printf("%-4s", "GPS");

    tft.setTextColor(GC9A01A_DARKGREEN, GC9A01A_YELLOW);
    tft.setCursor(0, 90);
    tft.setTextSize(2);
    tft.printf("%-50s", "Place your board outside to catch satelite signal");
}

void printGPSOnScreen(float lat, float lon, float speed, float alt, float accuracy, int usat, int counterTotal, int counterGoodValue)
{
    char *formatChar = (char *)"%-19s";
    bool locationIsValid = false;

    if (lat != 0 && lon != 0)
    {
        locationIsValid = true;
    }

    if (locationIsValid && usat >= 4)
    {
        tft.setTextColor(GC9A01A_GREEN, GC9A01A_BLACK);
    }
    else if (locationIsValid && usat < 4)
    {
        tft.setTextColor(GC9A01A_YELLOW, GC9A01A_BLACK);
    }
    else
    {
        tft.setTextColor(GC9A01A_RED, GC9A01A_BLACK);
    }

    tft.setTextSize(2);

    tft.setCursor(40, 40);
    String strCounterGoodValue = "Good : " + String(counterGoodValue);
    tft.printf("%-15s", strCounterGoodValue.c_str());

    tft.setCursor(30, 60);
    String strCounterTotal = "Total : " + String(counterTotal);
    tft.printf("%-11s", strCounterTotal.c_str());

    // tft.setCursor(30, 90);
    // String strIsValid = "IsFixValid : " + String(locationIsValid ? "true" : "false");
    // tft.printf(formatChar, strIsValid.c_str());

    tft.setCursor(15, 85);
    String strUsedSatellite = "Used Sat : " + String(usat);
    tft.printf(formatChar, strUsedSatellite.c_str());

    tft.setCursor(2, 110);
    String strLatitude = "Lat : " + String(lat, 10);
    tft.printf(formatChar, strLatitude.c_str());

    tft.setCursor(2, 130);
    String strLongitude = "Lon : " + String(lon, 10);
    tft.printf(formatChar, strLongitude.c_str());

    tft.setCursor(12, 150);
    String strAltitude = "Alt : " + String(alt, 8);
    tft.printf(formatChar, strAltitude.c_str());

    tft.setCursor(20, 170);
    String strSpeed = "Speed : " + String(speed, 4);
    tft.printf(formatChar, strSpeed.c_str());

    tft.setCursor(40, 190);
    String strAccuracy = "Accu : " + String(accuracy, 4);
    tft.printf(formatChar, strAccuracy.c_str());
}

// Coordonnées Borne Fontaine : 46.8428955078, -71.4045486450
// Coordonnées Borne Fontaine : 46.8428916931, -71.4045486450
// Coordonnées Borne Fontaine : 46.8429985046, -71.4049835205