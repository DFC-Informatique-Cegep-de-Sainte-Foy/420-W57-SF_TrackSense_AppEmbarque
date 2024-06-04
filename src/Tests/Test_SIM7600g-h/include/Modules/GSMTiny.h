#pragma once
#include <Arduino.h>
#include "Interfaces/IGSM.h"
#include "Configurations.h"
#include "TSProperties.h"
#include <TinyGsmClient.h>
#include "Adafruit_FONA.h"
#include <HardwareSerial.h>

// LilyGO T-SIM7000G Pinout
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4
#define LED_PIN 12

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial // HardwareSerial Serial(0);
// Set serial for AT commands
#define SerialAT Serial1 // HardwareSerial Serial1(1);

class GSMTiny : public IGSM
{
private:
    TSProperties *_TSProperties;
    TinyGsm *modem = new TinyGsm(SerialAT);
    HardwareSerial *fonaSerial = &SerialAT;
    Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

    const int FONA_RST = 34;
    const int RELAY_PIN = 13;
    char replybuffer[255];
    uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
    String smsString = "";
    char fonaNotificationBuffer[64]; // for notifications from the FONA
    char smsBuffer[250];

    /*_________________Envoyer SMS_____________________*/
    unsigned long timeout;
    char charArray[20];
    unsigned char data_buffer[4] = {0};
    void envoyerSMS();
    String mylong = ""; // for storing the longittude value
    String mylati = ""; // for storing the latitude value
    String textForSMS;
    char buff[10];
// #define SMS_TARGET "+14182558849" // TrackSense send sms to this telephone
#define SMS_TARGET "+15813070531" // TrackSense send sms to this telephone

    bool _isInitialized;
    bool _isGpsOn;
    bool _isModemOn;
    bool _isGPSFixed;

    double _distanceMetersBetweenLastPointAndCurrentPoint;
    unsigned long _maxDistanceTresholdInMeters;

    unsigned long _lastReadTimeMS;

    unsigned long _durationS;

    double _lastValidLatitude;
    double _lastValidLongitude;

    float _latitude;
    float _longitude;
    float _altitude;
    float _speed;
    int _visibleSatellites;
    int _usedSatellites;
    float _accuracy;
    int _year;
    int _month;
    int _day;
    int _hour;
    int _minute;
    int _seconde;

    String getDate();
    String getTime();
    String getDatetime();

    void modemPowerOn();
    void modemPowerOff();
    void modemRestart();

    bool readDatas();
    void gpsPowerOn();
    void gpsPowerOff();
    void gpsRestart();
    void setWorkModeGPS();

    bool isFixValid();
    bool isGPSFixed();
    void saveGPSDatasToTSProperties();
    void saveCurrentRideDatasToTSProperties();

    /* Méthode provient de TinyGPS++ */
    double distanceBetweenInMeters(double lat1, double long1, double lat2, double long2);

public:
    GSMTiny(TSProperties *TSProperties);
    ~GSMTiny();

    void init() override;
    void tick() override;
    bool isInitialized() override;
};
