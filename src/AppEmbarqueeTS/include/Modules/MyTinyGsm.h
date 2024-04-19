// #pragma once
// #include <Arduino.h>
// #include "Configurations.h"
// #include <TinyGsmClient.h>
// #include "Adafruit_FONA.h"
// #include "TSProperties.h"
// #include "Interfaces/IGSM.h"

// class MyTinyGsm : public IGSM
// {
// private:
//     // CONFIGURATION
//     const int FONA_RST = 34;
//     const int RELAY_PIN = 13;
//     TinyGsm *_modem;
//     HardwareSerial *fonaSerial = &SerialAT;
//     // Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);
//     Adafruit_FONA_3G *_fona;
//     TSProperties *_TSProperties;
//     // char replybuffer[255];
//     // uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
//     // String smsString = "";

//     // LTE
//     String senderNum;
//     String msgRecu;
//     String msgAEnvoyer;

//     char fonaNotificationBuffer[64]; // for notifications from the FONA
//     char smsBuffer[250];             // BUFFER POUR LIRE SMS RECU

//     unsigned long timeout;
//     // char charArray[20];
//     // unsigned char data_buffer[4] = {0};
//     char buff[10];
//     int slot = 0; // this will be the slot number of the SMS

//     // GPS info
//     bool _isInitialized;
//     bool _isGpsOn;
//     bool _isModemOn;
//     bool _isGPSFixed;
//     float _latitude;
//     float _longitude;
//     float _altitude;
//     float _speed;
//     int _visibleSatellites;
//     int _usedSatellites;
//     float _accuracy;
//     int _year;
//     int _month;
//     int _day;
//     int _hour;
//     int _minute;
//     int _seconde;
//     double _distanceMetersBetweenLastPointAndCurrentPoint;
//     unsigned long _lastReadTimeMS;
//     unsigned long _durationS;
//     double _lastValidLatitude;
//     double _lastValidLongitude;
//     unsigned long _maxDistanceTresholdInMeters;

// public:
//     MyTinyGsm(TSProperties *);
//     ~MyTinyGsm();
//     bool _estEnvoye = false;
//     bool detecteSMS();
//     String getSender();
//     String getSMS();
//     void getGPS();

//     // Interfaces
//     void init() override;
//     void tick() override;
//     bool isInitialized() override;
//     void tick_test() override;

//     // API
//     bool EnvoyerSMS(const String &telephone, const String &text);
//     bool EnvoyerGPS(const String &telephone);

//     // utilites
//     /* Méthode provient de TinyGPS++ */
//     // utilites-gps
//     double distanceBetweenInMeters(double lat1, double long1, double lat2, double long2);
//     void saveGPSDatasToTSProperties();
//     void saveCurrentRideDatasToTSProperties();
//     bool isFixValid();
//     bool isGPSFixed();
//     String getDate();
//     String getTime();
//     String getDatetime();
//     void gpsPowerOn();
//     void gpsPowerOff();
//     void gpsRestart();
//     void setWorkModeGPS();
//     bool readDatas(); // 获取gps数据，相当于getGPS

//     void modemPowerOn();
//     void modemPowerOff();
//     void modemRestart();
// };
