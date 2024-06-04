#include "Modules/MyTinyGsm.h"

MyTinyGsm::MyTinyGsm(TSProperties *ts)
    : _estEnvoye(false),
      _modem(nullptr),
      _fona(nullptr),
      _isInitialized(false),
      _TSProperties(ts),
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
      _durationS(0),
      _lastReadTimeMS(0),
      _lastValidLatitude(0),
      _lastValidLongitude(0),
      _maxDistanceTresholdInMeters(0)
{
    this->_TSProperties->PropertiesTS.IsInitializedGSM = false;
    _modem = new TinyGsm(SerialAT);
    _fona = new Adafruit_FONA_3G(FONA_RST);
    // Turn on the modem
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);
    this->_isModemOn = true;
    SerialMon.println("GPS on...");
    delay(1000);

    // Set module baud rate and UART pins
    SerialAT.begin(GPS_UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
    fonaSerial->begin(GPS_UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX, false);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    //
    if (this->_modem->testAT())
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Modem is finally functional");
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Modem initialized.");
    }
    else
    {
        if (!_modem->restart())
        {
            SerialMon.println("Failed to restart modem, attempting to continue without restarting");
            if (!this->_modem->init())
            {
                SerialMon.println("Failed to initialize modem");
            }
        }
    }

    // Print modem info
    String modemName = _modem->getModemName();
    delay(500);
    SerialMon.println("Modem Name: " + modemName);

    String modemInfo = _modem->getModemInfo();
    delay(500);
    SerialMon.println("Modem Info: " + modemInfo);

    // Set SIM7000G GPIO4 HIGH ,turn on GPS power
    // CMD:AT+SGPIO=0,4,1,1
    // Only in version 20200415 is there a function to control GPS power
    _modem->sendAT("+SGPIO=0,4,1,1");
    if (_modem->waitResponse(10000L) != 1)
    {
        SerialMon.println(" SGPIO=0,4,1,1 false ");
    }

    if (_modem->enableGPS())
    {
        SerialMon.println("GPS stand by");
    }
    else
    {
        SerialMon.println("GPS echc");
    }

    delay(15000);

    if (!_fona->begin(*fonaSerial))
    {
        Serial.println(F("Couldn't find FONA"));
        while (1)
            ;
    }
    Serial.println(F("FONA is OK"));

    fonaSerial->print("AT+CNMI=2,1\r\n"); // set up the FONA to send a +CMTI notification when an SMS is received
    Serial.println("FONA Ready");
}

bool MyTinyGsm::EnvoyerSMS(const String &telephone, const String &msg)
{
    if (telephone != NULL && msg != NULL)
    {
        if (_modem->sendSMS(telephone, msg))
        {
            Serial.println("SMS est envoye!");
            return true;
        }
        else
        {
            Serial.println("Error! Échec de l'envoi!");
        }
    }
    return false;
}

/// @brief 判断是否接受到通知
/// @return true = sms  false !=sms
bool MyTinyGsm::detecteSMS()
{
    char *bufPtr = fonaNotificationBuffer; // handy buffer pointer

    if (_fona->available()) // any data available from the FONA?
    {
        int charCount = 0;
        // Read the notification into fonaInBuffer
        do
        {
            *bufPtr = _fona->read();
            Serial.write(*bufPtr);
            delay(1);
        } while ((*bufPtr++ != '\n') && (_fona->available()) && (++charCount < (sizeof(fonaNotificationBuffer) - 1)));

        // Add a terminal NULL to the notification string
        *bufPtr = 0;
        if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot))
        {
            Serial.print("slot: ");
            Serial.println(slot);
            return true;
        }
    }
    return false;
}

/// @brief 读取短信内容
/// @return
String MyTinyGsm::getSMS()
{
    // Retrieve SMS value.
    uint16_t smslen;
    if (_fona->readSMS(slot, smsBuffer, 250, &smslen)) // pass in buffer and max len!
    {
        this->msgRecu = String(smsBuffer);
        Serial.println(this->msgRecu);
    }
    return String(this->msgRecu);
}

void MyTinyGsm::getGPS()
{
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
        if (_modem->getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                           &year, &month, &day, &hour, &min, &sec))
        {
            SerialMon.println("Latitude: " + String(lat, 8) + "\tLongitude: " + String(lon, 8));
            SerialMon.println("Speed: " + String(speed) + "\tAltitude: " + String(alt));
            SerialMon.println("Visible Satellites: " + String(vsat) + "\tUsed Satellites: " + String(usat));
            SerialMon.println("Accuracy: " + String(accuracy));
            SerialMon.println("Year: " + String(year) + "\tMonth: " + String(month) + "\tDay: " + String(day));
            SerialMon.println("Hour: " + String(hour) + "\tMinute: " + String(min) + "\tSecond: " + String(sec));

            //
            this->_latitude = lat;
            this->_longitude = lon;
            this->_speed = speed;
            this->_altitude = alt;
            this->_accuracy = accuracy;
            this->_visibleSatellites = vsat;
            this->_usedSatellites = usat;
            this->_year = year;
            this->_month = month;
            this->_day = day;
            this->_hour = hour;
            this->_minute = min;
            this->_seconde = sec;

            // MAJ info dans struct GPS
            this->_TSProperties->PropertiesGPS.Latitude = lat;
            this->_TSProperties->PropertiesGPS.Longitude = lon;
            this->_TSProperties->PropertiesGPS.Speed = speed;
            this->_TSProperties->PropertiesGPS.Altitude = alt;
            this->_TSProperties->PropertiesGPS.Accuracy = accuracy;
            this->_TSProperties->PropertiesGPS.VisibleSatellites = vsat;
            this->_TSProperties->PropertiesGPS.UsedSatellites = usat;
            this->_TSProperties->PropertiesGPS.Year = year;
            this->_TSProperties->PropertiesGPS.Month = month;
            this->_TSProperties->PropertiesGPS.Day = day;
            this->_TSProperties->PropertiesGPS.Hour = hour;
            this->_TSProperties->PropertiesGPS.Minute = min;
            this->_TSProperties->PropertiesGPS.Seconde = sec;
            this->_TSProperties->PropertiesGPS.Distance2Home = this->distanceBetweenInMeters(lat, lon, this->_TSProperties->PropertiesGPS.Home_Latitude, this->_TSProperties->PropertiesGPS.Home_Longitude);
            break;
        }
        else
        {
            SerialMon.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
            delay(15000L);
        }
    }

    // 获取GPS原始信息
    // SerialMon.println("Retrieving GPS/GNSS/GLONASS location again as a string");
    // String gps_raw = _modem->getGPSraw();
    // SerialMon.println("GPS/GNSS Based Location String: " + gps_raw);
    // 发送GPS信息
    // mylati = dtostrf(lat, 3, 6, buff);
    // mylong = dtostrf(lon, 3, 6, buff);
    // textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong;
    // delay(5000);
    // _modem->sendSMS(SMS_TARGET, textForSMS);
    // // fona.sendSMS(callerIDbuffer,textForSMS );
    // Serial.println("SMS send");
    // textForSMS = "";
}

void MyTinyGsm::init()
{
    ;
}

void MyTinyGsm::tick()
{
    // 无论何种情况，都要获取GPS信息
    getGPS();

    if (detecteSMS())
    {
        getSender();
        getSMS();
        if (msgRecu == "location")
        {
            EnvoyerGPS(SMS_TARGET);
        }

        if (msgRecu == "Bonjour" || msgRecu == "hello" || msgRecu == "hi")
        {
            EnvoyerSMS(SMS_TARGET, "Salutation de TrackSense~");
        }
    }

    // 无论在哪种状态下，只要有跌倒，并且消息还没有发送，就发送消息
    if (this->_TSProperties->PropertiesGPS.estChute && !this->_TSProperties->PropertiesGPS.estEnvoyerSMS)
    {
        // 发送消息
        EnvoyerSMS(SMS_TARGET, "Je suis ici:");
        EnvoyerGPS(SMS_TARGET);
        // 更新TS状态为：已发送
        // TODO:
        // 更新是否发送了短信的状态：已经发送(true)
        this->_TSProperties->PropertiesGPS.estEnvoyerSMS = true;
        Serial.println("SMS est envoye!");
        // 等待按钮事件，如果点击了按钮，则重新更新状态：还未发送(false)
        // TODO
    }

    /*_________________________________________________________________________*/
    // long actualTime = millis();
    // // 如果在骑行过程中
    // if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && this->_TSProperties->PropertiesCurrentRide.IsRideFinished == false)
    // {
    //     // // 如果GPS没开启，则启动GPS
    //     // if (this->_isGpsOn == false && this->_isModemOn == true && this->_isInitialized == true)
    //     // {
    //     //     this->gpsPowerOn();
    //     // }
    //     // 如果满足时间间隔(大于1秒钟)，则GPS工作
    //     // if (actualTime - this->_lastReadTimeMS > 1000)
    //     // {
    //     //     this->_durationS = (actualTime - this->_TSProperties->PropertiesCurrentRide.StartTimeMS) / 1000;
    //     //     this->_TSProperties->PropertiesCurrentRide.DurationS = this->_durationS;
    //     //     DEBUG_STRING_LN(DEBUG_TS_GPS, "DurationS : " + String(this->_durationS));

    //     //     this->_lastReadTimeMS = actualTime;
    //     // }

    //     this->getGPS();
    //     this->getSender();
    //     this->getSMS();
    // }
    // this->getGPS();
    // this->getSender();
    // this->getSMS();
}

bool MyTinyGsm::isInitialized()
{
    return true;
}

void MyTinyGsm::tick_test()
{
    long actualTime = millis();

#if DEBUG_GPS
    Serial.println("=======================================");
    Serial.println("Tick GSM");
    Serial.println("IsRideStarted : " + String(this->_TSProperties->PropertiesCurrentRide.IsRideStarted));
    Serial.println("IsRideFinished : " + String(this->_TSProperties->PropertiesCurrentRide.IsRideFinished));
#endif

    // 如果在骑行过程中
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && this->_TSProperties->PropertiesCurrentRide.IsRideFinished == false)
    {
        // 如果GPS没开启，则启动GPS
        if (this->_isGpsOn == false && this->_isModemOn == true && this->_isInitialized == true)
        {
            this->gpsPowerOn();
        }
        // 如果满足时间间隔(大于1秒钟)，则GPS工作，否则不读取数据
        if (actualTime - this->_lastReadTimeMS > 1000)
        {
            // 更新TR运行时间时间
            this->_durationS = (actualTime - this->_TSProperties->PropertiesCurrentRide.StartTimeMS) / 1000;
            this->_TSProperties->PropertiesCurrentRide.DurationS = this->_durationS;
            DEBUG_STRING_LN(DEBUG_TS_GPS, "DurationS : " + String(this->_durationS));
            // 更新时间戳
            this->_lastReadTimeMS = actualTime;
            // 如果读取到GPS数据
            if (this->readDatas())
            {
                // 保存数据到TS
                this->saveGPSDatasToTSProperties();
                // 如果 信号稳定？？？

                if (this->_TSProperties->PropertiesGPS.IsFixValid)
                {
                    // 1 - 如果是第一次获取到数据,则更新数据
                    if (this->_lastValidLatitude == 0 && this->_lastValidLongitude == 0)
                    {
                        this->_lastValidLatitude = this->_latitude;
                        this->_lastValidLongitude = this->_longitude;
                    }
                    // 2 - 计算当前位置和上一次位置的相对距离
                    this->_distanceMetersBetweenLastPointAndCurrentPoint = this->distanceBetweenInMeters(this->_lastValidLatitude, this->_lastValidLongitude, this->_latitude, this->_longitude);
                    // 3 - 如果距离差距大于最小间隔
                    if (this->_distanceMetersBetweenLastPointAndCurrentPoint > this->_maxDistanceTresholdInMeters)
                    {
                        // 4 -  保存当前骑行信息
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
                    }
                    // 5 - 更新当前经纬度
                    this->_lastValidLatitude = this->_latitude;
                    this->_lastValidLongitude = this->_longitude;
                }
            }
            else // 如果没有获取到GPS数据，则在debug模式下，输出提示
            {
                DEBUG_STRING_LN(DEBUG_TS_GPS, "Write GPS : Location is not Valid");
            }
        }
        this->_TSProperties->PropertiesCurrentRide.AverageSpeedKMPH = (this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters / this->_durationS) * 3.6;
    }
    else // 如果不在骑行中
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, "Write GPS : Ride is not Started");

        if (this->_isGpsOn == true && this->_isModemOn == true && this->_isInitialized == true)
        {
            this->gpsPowerOff();
        }
    }

    // 无论在哪种状态下，只要有跌倒，并且消息还没有发送，就发送消息
    if (this->_TSProperties->PropertiesGPS.estChute && !this->_TSProperties->PropertiesGPS.estEnvoyerSMS)
    {
        // 发送消息
        EnvoyerSMS(SMS_TARGET, "Je suis ici:");
        EnvoyerGPS(SMS_TARGET);
        // 更新TS状态为：已发送
        // TODO:
        // 更新是否发送了短信的状态：已经发送(true)
        this->_TSProperties->PropertiesGPS.estEnvoyerSMS = true;
        Serial.println("SMS est envoye!");
        // 等待按钮事件，如果点击了按钮，则重新更新状态：还未发送(false)
        // TODO
    }
}
bool MyTinyGsm::EnvoyerGPS(const String &telephone)
{
    String mylati = dtostrf(this->_TSProperties->PropertiesGPS.Latitude, 3, 6, buff);
    String mylong = dtostrf(this->_TSProperties->PropertiesGPS.Longitude, 3, 6, buff);
    String textForSMS = "http://www.google.com/maps/place/" + mylati + "," + mylong;
    if (textForSMS != NULL && telephone != NULL)
    {
        if (_modem->sendSMS(telephone, textForSMS))
        {
            Serial.println("GPS est envoye!");
            return true;
        }
        else
        {
            Serial.println("Error! Échec de l'envoi!");
        }
    }
    return false;
}

double MyTinyGsm::distanceBetweenInMeters(double lat1, double long1, double lat2, double long2)
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

void MyTinyGsm::saveGPSDatasToTSProperties()
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

void MyTinyGsm::saveCurrentRideDatasToTSProperties()
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

/// @brief latitude!=0&&longitude!=&&usedSatellites!=0&&speed!=-9999 &&accuracy<2
/// @return
bool MyTinyGsm::isFixValid()
{
    bool result = false;

    if (this->_latitude != 0 && this->_longitude != 0 && this->_usedSatellites >= 4 && this->_speed != -9999.00 && this->_accuracy < 2 && this->_altitude != 0)
    {
        result = true;
    }

    return result;
}

/// @brief latitude !=0 && longitude != 0
/// @return
bool MyTinyGsm::isGPSFixed()
{
    bool result = false;

    if (this->_latitude != 0 && this->_longitude != 0)
    {
        result = true;
    }

    return result;
}

String MyTinyGsm::getDate()
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

String MyTinyGsm::getTime()
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

String MyTinyGsm::getDatetime()
{
    // AppMobile doit recevoir ceci : 2023-09-03T14:30:00
    String result = this->getDate() + "T" + this->getTime();
    return result;
}

void MyTinyGsm::gpsPowerOn()
{
    DEBUG_STRING_LN(DEBUG_TS_GSM, "Enabling GPS");
    // Set SIM7000G GPIO4 HIGH ,turn on GPS power
    // CMD:AT+SGPIO=0,4,1,1
    // Only in version 20200415 is there a function to control GPS power
    // Version 20200415 (Version 1.1) = The version we have.
    // Version 20191227 (Version 1.0)
    this->_modem->sendAT("+SGPIO=0,4,1,1");
    if (this->_modem->waitResponse(10000L) != 1)
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, " SGPIO=0,4,1,1 false : Set GPS Power HIGH Failed");
    }

    this->_modem->enableGPS();
    this->_isGpsOn = true;
    delay(1000);
}

void MyTinyGsm::gpsPowerOff()
{
    DEBUG_STRING_LN(DEBUG_TS_GSM, "Disabling GPS");
    this->_modem->disableGPS();

    // Set SIM7000G GPIO4 LOW ,turn off GPS power
    // CMD:AT+SGPIO=0,4,1,0
    // Only in version 20200415 is there a function to control GPS power.
    // Version 20200415 (Version 1.1) = The version we have.
    // Version 20191227 (Version 1.0)
    this->_modem->sendAT("+SGPIO=0,4,1,0"); // maybe it should be "+CGPIO=0,4,1,0" or "+SGPIO=0,4,1,0" ???
    if (this->_modem->waitResponse(10000L) != 1)
    {
        DEBUG_STRING_LN(DEBUG_TS_GSM, " SGPIO=0,4,1,0 false : Set GPS Power LOW Failed");
    }

    delay(200);
    this->_isGpsOn = false;
}

void MyTinyGsm::gpsRestart()
{
    gpsPowerOff();
    delay(1000);
    gpsPowerOn();
}

void MyTinyGsm::setWorkModeGPS()
{
    this->_modem->sendAT("+CGNSMOD=1,1,1,1"); // Activate GPS, GLONASS, BeiDou, Galileo work mode.
    if (this->_modem->waitResponse(10000L) != 1)
    {
        DEBUG_STRING_LN(DEBUG_TS_GPS, " CGNSMOD=1,1,1,1 : Set work mode of SIM7000G GPS failed");
    }
}

bool MyTinyGsm::readDatas()
{
    bool result = false;

    if (this->_isInitialized && this->_isGpsOn)
    {
        DEBUG_STRING_LN(DEBUG_TS_GPS, "Requesting current GPS/GNSS/GLONASS location");
        // 7600
        // if (this->modem->getGPS(&this->_status, &this->_latitude, &this->_longitude, &this->_speed, &this->_altitude, &this->_visibleSatellites, &this->_usedSatellites,
        //                         &this->_accuracy, &this->_year, &this->_month, &this->_day, &this->_hour, &this->_minute, &this->_seconde))
        if (this->_modem->getGPS(&this->_latitude, &this->_longitude, &this->_speed, &this->_altitude, &this->_visibleSatellites, &this->_usedSatellites,
                                 &this->_accuracy, &this->_year, &this->_month, &this->_day, &this->_hour, &this->_minute, &this->_seconde))
        {
            result = true;

#if DEBUG_TS_GPS_HARDCORE
            Serial.println("Latitude: " + String(this->_latitude, 10) + "\tLongitude: " + String(this->_longitude, 10));
            Serial.println("Speed: " + String(this->_speed) + "\tAltitude: " + String(this->_altitude));
            Serial.println("Visible Satellites: " + String(this->_visibleSatellites) + "\tUsed Satellites: " + String(this->_usedSatellites));
            Serial.println("Accuracy: " + String(this->_accuracy));
            Serial.println("Year: " + String(this->_year) + "\tMonth: " + String(this->_month) + "\tDay: " + String(this->_day));
            Serial.println("Hour: " + String(this->_hour) + "\tMinute: " + String(this->_minute) + "\tSecond: " + String(this->_seconde));

            Serial.println();
            Serial.println("Retrieving GPS/GNSS/GLONASS location again as a string");
            String gps_raw = this->_modem->getGPSraw();
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

void MyTinyGsm::modemPowerOn()
{
    digitalWrite(PWR_PIN, LOW);
    delay(300);
    digitalWrite(PWR_PIN, HIGH);
    this->_isModemOn = true;
    delay(1000);
}

void MyTinyGsm::modemPowerOff()
{
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);
    this->_isModemOn = false;
}

void MyTinyGsm::modemRestart()
{
    modemPowerOff();
    delay(1000);
    modemPowerOn();
}

/// @brief 获取发送者信息
/// @return
String MyTinyGsm::getSender()
{
    char callerIDbuffer[32]; // we'll store the SMS sender number in here

    // Retrieve SMS sender address/phone number.
    if (!_fona->getSMSSender(slot, callerIDbuffer, 31))
    {
        Serial.println("Didn't find SMS message in slot!");
    }
    Serial.print(F("FROM: "));
    Serial.println(callerIDbuffer);
    this->senderNum = callerIDbuffer;
    return String(callerIDbuffer);
}
