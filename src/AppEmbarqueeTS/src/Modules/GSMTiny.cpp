#include "Modules/GSMTiny.h"

GSMTiny::GSMTiny(TSProperties *TSProperties) : _TSProperties(TSProperties),
                                               modem(nullptr),
                                               _isInitialized(false),
                                               _latitude(0),
                                               _longitude(0),
                                               _speed(0),
                                               _altitude(0),
                                               _visibleSatellites(0),
                                               _usedSatellites(0),
                                               _accuracy(0),
                                               _year(0),
                                               _month(0),
                                               _day(0),
                                               _hour(0),
                                               _minute(0),
                                               _seconde(0),
                                               _isGpsOn(false),
                                               _isModemOn(false),
                                               _isGPSFixed(false),
                                               _distanceMetersBetweenLastPointAndCurrentPoint(0),
                                               _maxDistanceTresholdInMeters(MINIMUM_DISTANCE_TO_ADD_POSITION),
                                               _lastValidLatitude(0),
                                               _lastValidLongitude(0),
                                               _durationS(0),
                                               _lastReadTimeMS(0),
                                               _timeSpamBuzzer(0)
{
    this->modem = new TinyGsm(SerialAT);
    pinMode(PWR_PIN, OUTPUT);
    // // Set LED OFF
    // pinMode(PIN_LED, OUTPUT);
    // DEBUG_STRING_LN(DEBUG_TS_GSM, "Set LED OFF");
    // digitalWrite(PIN_LED, HIGH);
    this->init();
}

GSMTiny::~GSMTiny()
{
    ;
}

void GSMTiny::init()
{
    this->_TSProperties->PropertiesTS.IsInitializedGSM = false;

    this->modemPowerOn();

    // Set GSM module baud rate
    SerialAT.begin(GPS_UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    // Restart takes quite some time
    // To skip it, call init() instead of restart() // It take less time to init than restart
    DEBUG_STRING_LN(DEBUG_TS_GSM, "----- Initializing modem -----");

    if (this->modem->testAT())
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Modem is finally functional");
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Modem initialized.");
    }
    else
    {
        if (!this->modem->restart())
        {
            DEBUG_STRING_LN(DEBUG_TS_GSM, "Failed to restart modem, attempting to continue without restarting");

            if (!this->modem->init())
            {
                DEBUG_STRING_LN(DEBUG_TS_GSM, "Failed to initialize modem.");
            }
        }
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Modem is not functional");
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Modem not initialized.");
    }

    // this->gpsRestart();
    this->gpsPowerOn();
    this->setWorkModeGPS();

    this->_isInitialized = true;
    this->_TSProperties->PropertiesTS.IsInitializedGSM = true;

    this->gpsPowerOff();
    DEBUG_STRING_LN(DEBUG_TS_GSM, "----- Initialization modem finished -----");
}

void GSMTiny::tick()
{
    // Serial.println("6--- GSM --> tick");
    // Si l'état actuel du TS est l'état déchu et qu'aucun message texte n'a encore été envoyé,
    // envoyez un message texte
    if (this->_TSProperties->PropertiesGPS.estChute && !_estEnvoye)
    {
        Serial.println("Chute detecte!");
        this->envoyerLocation();
        Serial.println("SMS send!");
        _estEnvoye = true;
    }

    long actualTime = millis();
    // Serial.println("6---1 Detecter Chute");
#if DEBUG_GPS
    Serial.println("=======================================");
    Serial.println("Tick GSM");
    Serial.println("IsRideStarted : " + String(this->_TSProperties->PropertiesCurrentRide.IsRideStarted));
    Serial.println("IsRideFinished : " + String(this->_TSProperties->PropertiesCurrentRide.IsRideFinished));
#endif

    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && this->_TSProperties->PropertiesCurrentRide.IsRideFinished == false)
    {
        // est arrive?
        if (!this->_TSProperties->PropertiesCurrentRide.estArrive && this->distanceBetweenInMeters(this->_latitude, this->_longitude, this->_TSProperties->PropertiesCurrentRide.latitude_destination, this->_TSProperties->PropertiesCurrentRide.longitude_destination) < MINIMUM_DISTANCE)
        {
            Serial.println("Arrivee!");
            this->_TSProperties->PropertiesCurrentRide.estArrive = true;
            _timeSpamBuzzer = millis();
        }
        if (millis() - _timeSpamBuzzer < 5000)
        {
            digitalWrite(PIN_BUZZER, HIGH);
            delay(500);
            digitalWrite(PIN_BUZZER, LOW);
        }

        // Serial.println("6---2 Turn On GPS");
        if (this->_isGpsOn == false && this->_isModemOn == true && this->_isInitialized == true)
        {
            this->gpsPowerOn();
        }
        /*---------------------------Au démarrage de l'itinéraire, au démarrage du GPS, si l'intervalle de temps est supérieur au seuil, les données sont lues-------------------------------------*/
        //         if (actualTime - this->_lastReadTimeMS > 1000)
        //         {
        //             this->_durationS = (actualTime - this->_TSProperties->PropertiesCurrentRide.StartTimeMS) / 1000;
        //             this->_TSProperties->PropertiesCurrentRide.DurationS = this->_durationS;
        //             DEBUG_STRING_LN(DEBUG_TS_GPS, "DurationS : " + String(this->_durationS));

        //             this->_lastReadTimeMS = actualTime;

        //             if (this->readDatas())
        //             {
        //                 this->saveGPSDatasToTSProperties();

        //                 if (this->_TSProperties->PropertiesGPS.IsFixValid)
        //                 {
        //                     if (this->_lastValidLatitude == 0 && this->_lastValidLongitude == 0)
        //                     {
        //                         this->_lastValidLatitude = this->_latitude;
        //                         this->_lastValidLongitude = this->_longitude;
        //                     }
        //                     this->_distanceMetersBetweenLastPointAndCurrentPoint = this->distanceBetweenInMeters(this->_lastValidLatitude, this->_lastValidLongitude, this->_latitude, this->_longitude);

        //                     if (this->_distanceMetersBetweenLastPointAndCurrentPoint > this->_maxDistanceTresholdInMeters)
        //                     {
        //                         this->saveCurrentRideDatasToTSProperties();
        // #if DEBUG_TS_GPS
        //                         Serial.println("Distance between last point and current point : " + String(this->_distanceMetersBetweenLastPointAndCurrentPoint));

        //                         Serial.println("PointID : " + String(this->_TSProperties->PropertiesCurrentRide.PointID));
        //                         Serial.println("NbPoints : " + String(this->_TSProperties->PropertiesCurrentRide.NbPoints));
        //                         Serial.println("DistanceTotalMeters : " + String(this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters, 2));
        //                         Serial.println("lat : " + String(this->_latitude, 10));
        //                         Serial.println("long : " + String(this->_longitude, 10));
        //                         Serial.println("last lat : " + String(this->_lastValidLatitude, 10));
        //                         Serial.println("last long : " + String(this->_lastValidLongitude, 10));
        // #endif

        //                         this->_lastValidLatitude = this->_latitude;
        //                         this->_lastValidLongitude = this->_longitude;
        //                     }
        //                 }
        // Serial.print("lat : " + String(this->_latitude, 10));
        // Serial.println(" long : " + String(this->_longitude, 10));
        //             }
        //             else
        //             {
        //                 DEBUG_STRING_LN(DEBUG_TS_GPS, "Write GPS : Location is not Valid");
        //             }
        //         }

        // Serial.println("6---4 Read GPS");
        /*
        La lecture des informations GPS est très lente. Il ne les lit que presque toutes les 5 secondes,
        ce qui ralentira sérieusement la reconnaissance des autres capteurs par le système.
        Par conséquent, une condition est définie ici.
        Il ne lit pas les données GPS à chaque fois que le programme boucle ici.
        mais toutes les 5 secondes. Lire une fois par seconde, c'est-à-dire :
        Lorsque le programme s'exécute ici, si l'heure de la dernière lecture est comparée à l'heure actuelle,
        elle ne sera lue que lorsqu'elle est supérieure à 5 secondes,
        donc une variable est nécessaire : lastReadTime, qui est comparé à l'heure actuelle.
        Les données GPS ne sont lues qu'une seule fois lorsqu'elles sont supérieures à 5 secondes,
        donc au final, le programme sera ralenti toutes les 5 secondes.
        La solution finale devrait être d'utiliser. plusieurs threads,
        un thread de programme et un thread de lecture de données GPS,
        pour fonctionner en parallèle ne s'affectent pas.

        */
        if (millis() - _lastReadTimeMS > 5000)
        {
            this->_durationS = (actualTime - this->_TSProperties->PropertiesCurrentRide.StartTimeMS) / 1000;
            this->_TSProperties->PropertiesCurrentRide.DurationS = this->_durationS;
            // TODO update duration Trajet

            DEBUG_STRING_LN(DEBUG_TS_GPS, "DurationS : " + String(this->_durationS));

            // Lire GPS datas
            if (this->readDatas())
            {
                // Serial.println("6---5 Save GPS");
                this->saveGPSDatasToTSProperties();

                if (this->_TSProperties->PropertiesGPS.IsFixValid)
                {
                    if (this->_lastValidLatitude == 0 && this->_lastValidLongitude == 0)
                    {
                        this->_lastValidLatitude = this->_latitude;
                        this->_lastValidLongitude = this->_longitude;
                        // update location de depart
                        _TSProperties->PropertiesCurrentRide.latitude_point_depart = this->_latitude;
                        _TSProperties->PropertiesCurrentRide.longitude_destination = this->_longitude;
                    }
                    // Serial.println("6---6 Calculer Distance");
                    this->_distanceMetersBetweenLastPointAndCurrentPoint = this->distanceBetweenInMeters(this->_lastValidLatitude, this->_lastValidLongitude, this->_latitude, this->_longitude);

                    if (this->_distanceMetersBetweenLastPointAndCurrentPoint > this->_maxDistanceTresholdInMeters)
                    {
                        // Serial.println("6---7 Save Ride");
                        this->saveCurrentRideDatasToTSProperties();
#if DEBUG_TS_GPS
                        Serial.println("Distance between last point and current point : " + String(this->_distanceMetersBetweenLastPointAndCurrentPoint));

                        Serial.println("PointID : " + String(this->_TSProperties->PropertiesCurrentRide.PointID));
                        Serial.println("NbPoints : " + String(this->_TSProperties->PropertiesCurrentRide.NbPoints));
                        Serial.println("DistanceTotalMeters : " + String(this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters, 2));
                        Serial.println("lat : " + String(this->_latitude, 10));
                        Serial.println("long : " + String(this->_longitude, 10));
                        Serial.println("last lat : " + String(this->_lastValidLatitude, 10));
                        Serial.println("last long : " + String(this->_lastValidLongitude, 10));
#endif

                        this->_lastValidLatitude = this->_latitude;
                        this->_lastValidLongitude = this->_longitude;
                    }
                }
                // Serial.print("lat : " + String(this->_latitude, 10));
                // Serial.println(" long : " + String(this->_longitude, 10));
                Serial.println(" Distance de Destination : " + String(distanceBetweenInMeters(this->_latitude, this->_longitude, this->_TSProperties->PropertiesCurrentRide.latitude_destination, this->_TSProperties->PropertiesCurrentRide.longitude_destination)));
            }
            else
            {
                DEBUG_STRING_LN(DEBUG_TS_GPS, "Write GPS : Location is not Valid");
            }

            this->_TSProperties->PropertiesCurrentRide.AverageSpeedKMPH = (this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters / this->_durationS) * 3.6;
        }

        // /*---------------------------Au démarrage de l'itinéraire, le GPS démarre. Au démarrage du GPS, si une chute est détectée, un message texte sera envoyé.-------------------------------------*/
        // Si l'état actuel du TS est l'état déchu et qu'aucun message texte n'a encore été envoyé,
        // envoyez un message texte
        // Parce que nous sommes déjà en état de conduite, le GPS est déjà activé, il n'est donc pas nécessaire de relire les informations GPS, il suffit de lire la longitude et la latitude directement à partir de l'état TS.
        // Serial.println("6---3 Detecter Chute(2)");
        if (this->_TSProperties->PropertiesGPS.estChute && !_estEnvoye)
        {
            Serial.println("Chute detecte!");
            this->envoyerSMS(this->_TSProperties->PropertiesGPS.Latitude, this->_TSProperties->PropertiesGPS.Longitude);
            Serial.println("SMS send!");
            _estEnvoye = true;
        }
    }
    else
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Write GPS : Ride is not Started");

        if (this->_isGpsOn == true && this->_isModemOn == true && this->_isInitialized == true)
        {
            this->gpsPowerOff();
        }
    }
}

bool GSMTiny::readDatas()
{
    bool result = false;

    if (this->_isInitialized && this->_isGpsOn)
    {
        DEBUG_STRING_LN(DEBUG_TS_GPS, "Requesting current GPS/GNSS/GLONASS location");

        if (this->modem->getGPS(&this->_latitude, &this->_longitude, &this->_speed, &this->_altitude, &this->_visibleSatellites, &this->_usedSatellites,
                                &this->_accuracy, &this->_year, &this->_month, &this->_day, &this->_hour, &this->_minute, &this->_seconde))
        {
            result = true;

            this->_lastReadTimeMS = millis();
#if DEBUG_TS_GPS_HARDCORE
            Serial.println("Latitude: " + String(this->_latitude, 10) + "\tLongitude: " + String(this->_longitude, 10));
            Serial.println("Speed: " + String(this->_speed) + "\tAltitude: " + String(this->_altitude));
            Serial.println("Visible Satellites: " + String(this->_visibleSatellites) + "\tUsed Satellites: " + String(this->_usedSatellites));
            Serial.println("Accuracy: " + String(this->_accuracy));
            Serial.println("Year: " + String(this->_year) + "\tMonth: " + String(this->_month) + "\tDay: " + String(this->_day));
            Serial.println("Hour: " + String(this->_hour) + "\tMinute: " + String(this->_minute) + "\tSecond: " + String(this->_seconde));

            Serial.println();
            Serial.println("Retrieving GPS/GNSS/GLONASS location again as a string");
            String gps_raw = this->modem->getGPSraw();
            Serial.println("GPS/GNSS Based Location String: " + gps_raw);
#endif
        }
        else
        {
            DEBUG_STRING_LN(DEBUG_TS_GPS, "Couldn't get GPS/GNSS/GLONASS location.");
        }

        this->_TSProperties->PropertiesGPS.CounterTotal++;
    }

    return result;
}

bool GSMTiny::isFixValid()
{
    bool result = false;

    if (this->_latitude != 0 && this->_longitude != 0 && this->_usedSatellites >= 4 && this->_speed != -9999.00 && this->_accuracy < 2 && this->_altitude != 0)
    {
        result = true;
    }

    return result;
}

bool GSMTiny::isGPSFixed()
{
    bool result = false;

    if (this->_latitude != 0 && this->_longitude != 0)
    {
        result = true;
    }

    return result;
}

void GSMTiny::saveGPSDatasToTSProperties()
{
    this->_TSProperties->PropertiesGPS.Latitude = this->_latitude;
    this->_TSProperties->PropertiesGPS.Longitude = this->_longitude;
    this->_TSProperties->PropertiesGPS.Altitude = this->_altitude;
    this->_TSProperties->PropertiesGPS.Speed = this->_speed;
    this->_TSProperties->PropertiesGPS.VisibleSatellites = this->_visibleSatellites;
    this->_TSProperties->PropertiesGPS.UsedSatellites = this->_usedSatellites;
    this->_TSProperties->PropertiesGPS.Accuracy = this->_accuracy;
    this->_TSProperties->PropertiesGPS.Year = this->_year;
    this->_TSProperties->PropertiesGPS.Month = this->_month;
    this->_TSProperties->PropertiesGPS.Day = this->_day;
    this->_TSProperties->PropertiesGPS.Hour = this->_hour;
    this->_TSProperties->PropertiesGPS.Minute = this->_minute;
    this->_TSProperties->PropertiesGPS.Seconde = this->_seconde;
    this->_TSProperties->PropertiesGPS.IsFixValid = this->isFixValid();
    this->_TSProperties->PropertiesGPS.IsGPSFixed = this->isGPSFixed();

    DEBUG_STRING_LN(DEBUG_TS_GPS_HARDCORE, "--------------------------------------------Accuracy : " + String(this->_TSProperties->PropertiesGPS.Accuracy));
}

void GSMTiny::saveCurrentRideDatasToTSProperties()
{
    this->_TSProperties->PropertiesGPS.CounterGoodValue++;

    if (this->_TSProperties->PropertiesCurrentRide.DateBegin == "0000-00-00T00:00:00")
    {
        this->_TSProperties->PropertiesCurrentRide.DateBegin = this->getDatetime();
    }

    this->_TSProperties->PropertiesCurrentRide.PointID++;
    this->_TSProperties->PropertiesCurrentRide.NbPoints++;
    this->_TSProperties->PropertiesCurrentRide.DateEnd = this->getDatetime();
    this->_TSProperties->PropertiesCurrentRide.Temperature = this->_TSProperties->PropertiesTemperature.Temperature;
    this->_TSProperties->PropertiesCurrentRide.CurrentPoint = String(this->_TSProperties->PropertiesCurrentRide.PointID) + ";" +
                                                              String(this->_latitude, 10) + ";" +
                                                              String(this->_longitude, 10) + ";" +
                                                              String(this->_altitude) + ";" +
                                                              String(this->_TSProperties->PropertiesCurrentRide.Temperature) + ";" +
                                                              String(this->_speed) + ";" +
                                                              this->getDatetime() + ";" +
                                                              String(this->_TSProperties->PropertiesCurrentRide.DurationS);

    this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters += this->_distanceMetersBetweenLastPointAndCurrentPoint;
    this->_TSProperties->PropertiesCurrentRide.MaxSpeedKMPH = max(this->_TSProperties->PropertiesCurrentRide.MaxSpeedKMPH, this->_speed);

    this->_TSProperties->PropertiesCurrentRide.IsPointReadyToSave = true;
}

String GSMTiny::getDate()
{
    String month = String(this->_month);
    if (month.length() == 1)
    {
        month = "0" + month;
    }
    String day = String(this->_day);
    if (day.length() == 1)
    {
        day = "0" + day;
    }

    String result = String(this->_year) + "-" + month + "-" + day;
    return result;
}

String GSMTiny::getTime()
{
    String hour = String(this->_hour);
    if (hour.length() == 1)
    {
        hour = "0" + hour;
    }
    String minute = String(this->_minute);
    if (minute.length() == 1)
    {
        minute = "0" + minute;
    }
    String seconde = String(this->_seconde);
    if (seconde.length() == 1)
    {
        seconde = "0" + seconde;
    }

    String result = hour + ":" + minute + ":" + seconde;
    return result;
}

String GSMTiny::getDatetime()
{
    // AppMobile doit recevoir ceci : 2023-09-03T14:30:00
    String result = this->getDate() + "T" + this->getTime();
    return result;
}

void GSMTiny::gpsPowerOn()
{
    /*
        Parameters :
        <operation>
            0 Set the GPIO function including the GPIO output .
            1 Read the GPIO level. Please note that only when the gpio is set as input, user can use parameter 1 to read the GPIO level, otherwise the module will return "ERROR".

        <pin>
            The PIN index you want to be set. (It has relations with the hardware, please refer to the hardware manual)

        <function>
            Only when <operation> is set to 0, this option takes effect.
            0 Set the GPIO to input.
            1 Set the GPIO to output.

        <level>
            0 Set the GPIO low level
            1 Set the GPIO high level
    */
    DEBUG_STRING_LN(DEBUG_TS_GSM, "Enabling GPS");
    // Set SIM7000G GPIO4 HIGH ,turn on GPS power
    // CMD:AT+SGPIO=0,4,1,1
    // Only in version 20200415 is there a function to control GPS power
    // Version 20200415 (Version 1.1) = The version we have.
    // Version 20191227 (Version 1.0)
    this->modem->sendAT("+SGPIO=0,4,1,1");
    if (this->modem->waitResponse(10000L) != 1)
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, " SGPIO=0,4,1,1 false : Set GPS Power HIGH Failed");
    }

    this->modem->enableGPS();
    this->_isGpsOn = true;
    delay(1000);
}

void GSMTiny::gpsPowerOff()
{
    DEBUG_STRING_LN(DEBUG_TS_GSM, "Disabling GPS");
    this->modem->disableGPS();

    // Set SIM7000G GPIO4 LOW ,turn off GPS power
    // CMD:AT+SGPIO=0,4,1,0
    // Only in version 20200415 is there a function to control GPS power.
    // Version 20200415 (Version 1.1) = The version we have.
    // Version 20191227 (Version 1.0)
    this->modem->sendAT("+SGPIO=0,4,1,0"); // maybe it should be "+CGPIO=0,4,1,0" or "+SGPIO=0,4,1,0" ???
    if (this->modem->waitResponse(10000L) != 1)
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, " SGPIO=0,4,1,0 false : Set GPS Power LOW Failed");
    }

    delay(200);
    this->_isGpsOn = false;
}

void GSMTiny::gpsRestart()
{
    gpsPowerOff();
    delay(1000);
    gpsPowerOn();
}

void GSMTiny::modemPowerOn()
{
    digitalWrite(PWR_PIN, LOW);
    delay(300);
    digitalWrite(PWR_PIN, HIGH);
    this->_isModemOn = true;
    delay(1000);
}

void GSMTiny::modemPowerOff()
{
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);
    this->_isModemOn = false;
}

void GSMTiny::modemRestart()
{
    modemPowerOff();
    delay(1000);
    modemPowerOn();
}

/*
    Set work mode of SIM7000G GPS with AT Command "AT+CGNSMOD=1,1,1,1"

    Type of commands :
    Test Command : AT+CGNSMOD=?
    Read Command : AT+CGNSMOD?
    Set Command  : AT+CGNSMOD=<GPS mode>,<glonass mode>,<beidou mode>,<galieo mode>

    Parameters :
    <GPS mode>     :    1 - Enable GPS
    <glonass mode> :    0 - Disable GLONASS     1 - Enable GLONASS
    <beidou mode>  :    0 - Disable BeiDou      1 - Enable BeiDou
    <galieo mode>  :    0 - Disable Galileo     1 - Enable Galileo
*/
void GSMTiny::setWorkModeGPS()
{
    this->modem->sendAT("+CGNSMOD=1,1,1,1"); // Activate GPS, GLONASS, BeiDou, Galileo work mode.
    if (this->modem->waitResponse(10000L) != 1)
    {
        DEBUG_STRING_LN(DEBUG_TS_GPS, " CGNSMOD=1,1,1,1 : Set work mode of SIM7000G GPS failed");
    }
}

bool GSMTiny::isInitialized()
{
    return this->_isInitialized;
}

void GSMTiny::tick_test()
{
    ;
}

double GSMTiny::distanceBetweenInMeters(double lat1, double long1, double lat2, double long2)
{
    /* Méthode provient de TinyGPS++ */

    // returns distance in meters between two positions, both specified
    // as signed decimal-degrees latitude and longitude. Uses great-circle
    // distance computation for hypothetical sphere of radius 6372795 meters.
    // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
    // Courtesy of Maarten Lamers
    double delta = radians(long1 - long2);
    double sdlong = sin(delta);
    double cdlong = cos(delta);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = sq(delta);
    delta += sq(clat2 * sdlong);
    delta = sqrt(delta);
    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);
    return delta * 6372795;
}

void GSMTiny::envoyerLocation()
{
    // Ouvrir GPS
    if (this->_isGpsOn == false)
    {
        this->gpsPowerOn();
    }
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

    for (int8_t i = 15; i; i--)
    {
        SerialMon.println("Requesting current GPS/GNSS/GLONASS location");
        if (modem->getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                          &year, &month, &day, &hour, &min, &sec))
        {
            SerialMon.println("Latitude: " + String(lat, 8) + "\tLongitude: " + String(lon, 8));
            SerialMon.println("Speed: " + String(speed) + "\tAltitude: " + String(alt));
            SerialMon.println("Visible Satellites: " + String(vsat) + "\tUsed Satellites: " + String(usat));
            SerialMon.println("Accuracy: " + String(accuracy));
            SerialMon.println("Year: " + String(year) + "\tMonth: " + String(month) + "\tDay: " + String(day));
            SerialMon.println("Hour: " + String(hour) + "\tMinute: " + String(min) + "\tSecond: " + String(sec));
            break;
        }
        else
        {
            SerialMon.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
            delay(15000L);
        }
    }
    SerialMon.println("Retrieving GPS/GNSS/GLONASS location again as a string");
    String gps_raw = modem->getGPSraw();
    SerialMon.println("GPS/GNSS Based Location String: " + gps_raw);

    mylati = dtostrf(lat, 3, 6, buff);
    mylong = dtostrf(lon, 3, 6, buff);
    textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong;
    // mylati = dtostrf(46.78569, 3, 6, buff);
    // mylong = dtostrf(-71.28704, 3, 6, buff);
    // textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong;

    delay(5000);
    modem->sendSMS(SMS_TARGET, "Je suis ici: ");
    modem->sendSMS(SMS_TARGET, textForSMS);
    // fona.sendSMS(callerIDbuffer,textForSMS );
    Serial.println("GPS send");
    textForSMS = "";
}

void GSMTiny::envoyerSMS(float latitude, float longitude)
{
    mylati = dtostrf(latitude, 3, 6, buff);
    mylong = dtostrf(longitude, 3, 6, buff);
    textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong;
    delay(5000);
    modem->sendSMS(SMS_TARGET, "Je suis ici: ");
    modem->sendSMS(SMS_TARGET, textForSMS);
    // fona.sendSMS(callerIDbuffer,textForSMS );
    Serial.println("GPS send");
    textForSMS = "";
}
