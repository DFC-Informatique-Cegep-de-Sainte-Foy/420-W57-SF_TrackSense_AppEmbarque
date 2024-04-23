#pragma once
#include <Arduino.h>
#include "Interfaces/IGSM.h"
#include "Configurations.h"
#include "TSProperties.h"
#include <TinyGsmClient.h>

class GSMTiny : public IGSM
{
private:
    TSProperties *_TSProperties;
    TinyGsm *modem;

    bool _isInitialized;
    bool _isGpsOn;
    bool _isModemOn;
    bool _isGPSFixed;

    unsigned long _timeSpamBuzzer;

    double _distanceMetersBetweenLastPointAndCurrentPoint;
    unsigned long _maxDistanceTresholdInMeters;

    unsigned long _lastReadTimeMS;

    unsigned long _durationS;

    double _lastValidLatitude;
    double _lastValidLongitude;

    u_int8_t _status;
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
    bool _estEnvoye = false;

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
    double distanceBetweenInMeters(double lat1, double long1, double lat2, double long2); // 移植

    /*_________________Envoyer SMS_____________________*/
    void envoyerLocation();
    void envoyerSMS(float, float);
    String mylong = ""; // for storing the longittude value
    String mylati = ""; // for storing the latitude value
    String textForSMS;
    char buff[10];
    // Modification pour localiser le user story a corriger pour iteration 2

public:
    GSMTiny(TSProperties *TSProperties);
    ~GSMTiny();

    void init() override;
    void tick() override;
    bool isInitialized() override;
    void tick_test() override;
};
