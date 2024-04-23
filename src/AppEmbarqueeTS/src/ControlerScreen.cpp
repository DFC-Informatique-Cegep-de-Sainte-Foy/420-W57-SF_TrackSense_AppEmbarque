#include "ControlerScreen.h"

ControlerScreen::ControlerScreen(TSProperties *TSProperties) : _TSProperties(TSProperties),
                                                               _screen(nullptr),
                                                               _timeToDisplayEndingRidePageMS(10000),
                                                               _xMutex(nullptr)
{
    this->_screen = new ScreenGC9A01(this->_TSProperties);
    this->_xMutex = xSemaphoreCreateMutex(); // Create a mutex object

    this->tick();
    this->drawOnScreen();
}

ControlerScreen::~ControlerScreen()
{
    delete this->_screen;
}

/*
    0 : Home Page
    1 : Ride Page
    2 : Ride Statistics Page
    3 : Compass Page
    4 : Ride Direction Page
    5 : Global Statistics Page
    6 : Go Home Page
    -1 : Init TS Page
    -2 : Ending Ride Page
    -3 : No Page (error)

    0 : Page Accueil
    1 : Page Trajet
    2 : Page Statistiques Trajet
    3 : Page Boussole
    4 : Page Direction Trajet
    5 : Page Statistiques Globales
    6 : Page Retour Maison
    -1 : Page Init TS
    -2 : Page Fin Trajet
    -3 : Page Erreur
*/
void ControlerScreen::tick()
{
    // Serial.println("4---ScreenControl --> tick");

    // if (xSemaphoreTake(_xMutex, portMAX_DELAY))
    if (xSemaphoreTake(_xMutex, (150 * portTICK_PERIOD_MS)))
    {
        this->_screen->drawBackgroundColor(); // Reset Canvas

        DEBUG_STRING_LN(DEBUG_TS_SCREEN, "Screen Rotation : " + String(this->_TSProperties->PropertiesScreen.ScreenRotation));
        this->_screen->setRotation(this->_TSProperties->PropertiesScreen.ScreenRotation);

        if (this->_TSProperties->PropertiesTS.IsOnStanby)
        {
            DEBUG_STRING_LN(DEBUG_TS_SCREEN, "IsOnStanby");
            // this->_screen->drawBackgroundColor(GC9A01A_BLACK, GC9A01A_BLACK);
            // this->_screen->setTextSize(4);
            // this->_screen->printText("Veille", this->_screen->calculateXCoordTextToCenter("Veille"), 140);
            digitalWrite(TFT_BLK, LOW); // Backlight off
        }
        else
        {
            if (!this->_TSProperties->PropertiesTS.IsInitializedGSM)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = ERROR_PAGE_ID;
            }

            DEBUG_STRING_LN(DEBUG_TS_SCREEN, "Active Screen : " + String(this->_TSProperties->PropertiesScreen.ActiveScreen));
            // Serial.println("Active Screen" + String(this->_TSProperties->PropertiesScreen.ActiveScreen));

            switch (this->_TSProperties->PropertiesScreen.ActiveScreen)
            {
            case INIT_TS_PAGE_ID:       // -1
                this->drawInitTSPage(); // initialisation
                break;

            case HOME_PAGE_ID:        // 0
                this->drawHomePage(); //
                break;

            case RIDE_PAGE_ID: // 1  trajet
                if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
                {
                    this->drawRidePage();
                }
                else
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = HOME_PAGE_ID;
                }
                break;

            case RIDE_STATISTICS_PAGE_ID: // 2  trajet -> Static
                if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
                {
                    this->drawRideStatisticsPage();
                }
                else
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = HOME_PAGE_ID;
                }
                break;

            case COMPASS_PAGE_ID: // 3 compass
                this->drawCompassPage();
                break;

            case RIDE_DIRECTION_PAGE_ID: // 4 Direction
                this->drawRideDirectionPage();
                break;

            case GLOBAL_STATISTICS_PAGE_ID: // 5 Global
                this->drawGlobalStatisticsPage();
                break;

            case GO_HOME_PAGE_ID: // 6  backhome
                this->drawGoHomePage();
                break;

            case ENDING_RIDE_PAGE_ID: // -2
                if (this->_TSProperties->PropertiesCurrentRide.EndTimeMS + this->_timeToDisplayEndingRidePageMS >= millis())
                {
                    this->drawEndingRidePage();
                }
                else
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = HOME_PAGE_ID;
                }

                break;

            default: // -3
                // this->drawErrorPage();
                break;
            }
        }

        // this->_screen->drawOnScreen();   // We use void ControlerScreen::drawOnScreen() on Core 0 to draw on screen
        xSemaphoreGive(_xMutex); // release the mutex
    }
}

void ControlerScreen::drawOnScreen()
{
    // if (xSemaphoreTake(_xMutex, (200 * portTICK_PERIOD_MS)))
    if (xSemaphoreTake(_xMutex, portMAX_DELAY))
    {
        this->_screen->drawOnScreen();

        xSemaphoreGive(_xMutex); // release the mutex
    }
}

// Modification pour localiser le user story a corriger pour iteration 2
float ControlerScreen::calculerDirectionDegree()
{
    double newDirectionDestinationRAD = 0.0;
    float newDirectionDestinationDegree = 0.0;

    float newHeading = _TSProperties->PropertiesCompass.heading;
    float DX = _TSProperties->PropertiesCurrentRide.longitude_destination;
    float DY = _TSProperties->PropertiesCurrentRide.latitude_destination;
    float OX = _TSProperties->PropertiesGPS.Longitude;
    float OY = _TSProperties->PropertiesGPS.Latitude;

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

/*


    Draw Pages


*/
#pragma region DrawPages

void ControlerScreen::drawInitTSPage()
{
    this->_screen->drawLogoTS();
    this->_screen->setTextColor();
    this->_screen->setTextSize(1);
    this->_screen->setFont(3);

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        this->_screen->printText("Initialisation", this->_screen->calculateXCoordTextToCenter("Initialisation"), 210);
    }
    else // Anglais
    {
        this->_screen->printText("Initializing", this->_screen->calculateXCoordTextToCenter("Initializing"), 210);
    }

    this->_screen->setFont(1);
}

void ControlerScreen::drawHomePage()
{
#if DEBUG_TS_BUTTONS_HARDCORE
    this->_screen->testButtonsScreen();
#else
    this->_screen->drawLogoTS();

    int batteryLengthInPixels = 50;
    this->_screen->drawBattery(this->_screen->calculateXCoordItemToCenter(batteryLengthInPixels),
                               15,
                               batteryLengthInPixels,
                               this->_TSProperties->PropertiesBattery.BatteryLevelPourcentage);

    int rideStartedLengthInPixels = 40;
    this->_screen->drawIsRideStarted(this->_screen->calculateXCoordItemToCenter(rideStartedLengthInPixels), 185, rideStartedLengthInPixels);
#endif
}

void ControlerScreen::drawCompassPage()
{
    // TODO
    float newHeading = _TSProperties->PropertiesCompass.heading;
    // double newDirectionDestinationRAD = atan((_TSProperties->PropertiesCurrentRide.latitude_destination - _TSProperties->PropertiesGPS.Latitude) / (_TSProperties->PropertiesCurrentRide.longitude_destination - _TSProperties->PropertiesGPS.Longitude)); // 23.08
    // Serial.println("New Direction --> " + String(newDirectionDestinationRAD * RAD2DEG));                                                                                                                                                                   // 0.40弧度
    // // rad2deg
    // float newDirectionDestinationDegree = newDirectionDestinationRAD * RAD2DEG;
    // si premiere fois, draw gauge
    // si donnee change, re-affiche
    // if (!entreCompass)
    // {
    //     //draw gauge
    //     this->_screen->Draw_Cadran_Compass();
    //     entreCompass = true;
    //     delayMicroseconds(1000);
    // }

    this->_screen->Draw_Cadran_Compass();

    // Il y a de nouvelles données, effacez l'écran et mettez à jour
    if (lastDegree != newHeading)
    {
        // Effacer le dernier pointeur sur l'écran
        this->_screen->cleanNeedleCompass();
        // Mettre à jour la position du pointeur
        this->_screen->Draw_Compass(newHeading);
        // Mettre à jour l'emplacement de destination
        this->_screen->Draw_Destination(calculerDirectionDegree()); // 23.05  63.64
        lastDegree = newHeading;
    }
    // this->_screen->drawCompass(_TSProperties->PropertiesCompass.heading);

    // this->_screen->drawCompass(120);
}

void ControlerScreen::drawRideDirectionPage()
{
    // TODO
    this->_screen->setTextSize(1);
    this->_screen->setFont(1);
    this->_screen->printText("Direction", 100, 100);
}

void ControlerScreen::drawRidePage()
{
#if DEBUG_TS_GPS_HARDCORE
    this->_screen->testGPS();
#else
    String speed = "";

    this->_screen->setTextColor();
    this->_screen->setTextSize(2);
    this->_screen->setFont(2);

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        this->_screen->printText("Trajet", this->_screen->calculateXCoordTextToCenter("Trajet"), 65);
    }
    else // Anglais
    {
        this->_screen->printText("Ride Page", this->_screen->calculateXCoordTextToCenter("Ride Page"), 65);
    }

    this->_screen->setFont(1);
    this->_screen->setTextColor();
    this->_screen->setTextSize(2);
    this->_screen->printText("Km/h", this->_screen->calculateXCoordTextToCenter("Km/h"), 220);

    if (this->_TSProperties->PropertiesGPS.IsFixValid && this->_TSProperties->PropertiesGPS.IsGPSFixed)
    {
        this->_screen->setTextColor();
        // speed = String(this->_TSProperties->PropertiesGPS.Speed, 1);
        // tester pour GPS DATA
        speed = String(this->_TSProperties->PropertiesGPS.Altitude, 1);
    }
    else if (!this->_TSProperties->PropertiesGPS.IsFixValid && this->_TSProperties->PropertiesGPS.IsGPSFixed)
    {
        this->_screen->setTextColor(GC9A01A_YELLOW, GC9A01A_BLACK, GC9A01A_YELLOW, GC9A01A_WHITE);
        // speed = String(this->_TSProperties->PropertiesGPS.Speed, 1);
        // tester pour GPS DATA
        speed = String(this->_TSProperties->PropertiesGPS.Altitude, 1);
    }
    else
    {
        this->_screen->setTextColor();
        speed = String(this->_TSProperties->PropertiesGPS.Altitude, 1);

        // speed = "---";
    }

    this->_screen->setFont(2);
    this->_screen->setTextSize(4);
    this->_screen->printText(speed, this->_screen->calculateXCoordTextToCenter(speed), 155);

    this->_screen->setFont(1);
    int batteryLengthInPixels = 50;
    this->_screen->drawBattery(this->_screen->calculateXCoordItemToCenter(batteryLengthInPixels),
                               8,
                               batteryLengthInPixels,
                               this->_TSProperties->PropertiesBattery.BatteryLevelPourcentage);
#endif
}

void ControlerScreen::drawGlobalStatisticsPage()
{
    // TODO
    this->_screen->setTextSize(1);
    this->_screen->setFont(1);
    this->_screen->printText("Global", 100, 100);
}

void ControlerScreen::drawGoHomePage()
{
    // TODO
    this->_screen->setTextSize(1);
    this->_screen->setFont(1);
    this->_screen->printText("Back Home", 100, 100);
}

void ControlerScreen::drawRideStatisticsPage()
{
    this->_screen->setTextColor();
    this->_screen->setTextSize(2);
    this->_screen->setFont(2);

    this->_screen->printText("Stats", this->_screen->calculateXCoordTextToCenter("Stats"), 65); // Français & Anglais

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        this->_screen->drawStatistics("Dist.:", String(this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters / 1000, 3), "Km", 10, 85, 185, 95);
        this->_screen->drawStatistics("Duree:", this->_TSProperties->PropertiesCurrentRide.formatDurationHMS(), "h:m:s", 10, 85, 185, 120);
        this->_screen->drawStatistics("V.Moy:", String(this->_TSProperties->PropertiesCurrentRide.AverageSpeedKMPH, 2), "Km/h", 10, 85, 185, 145);
        this->_screen->drawStatistics("V.Max:", String(this->_TSProperties->PropertiesCurrentRide.MaxSpeedKMPH, 2), "Km/h", 10, 85, 185, 170);
    }
    else // Anglais
    {
        this->_screen->drawStatistics("Dist.:", String(this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters / 1000, 3), "Km", 10, 90, 185, 95);
        this->_screen->drawStatistics("Dur.:", this->_TSProperties->PropertiesCurrentRide.formatDurationHMS(), "h:m:s", 10, 90, 185, 120);
        this->_screen->drawStatistics("Avg. S.:", String(this->_TSProperties->PropertiesCurrentRide.AverageSpeedKMPH, 2), "Km/h", 10, 90, 185, 145);
        this->_screen->drawStatistics("Max S.:", String(this->_TSProperties->PropertiesCurrentRide.MaxSpeedKMPH, 2), "Km/h", 10, 90, 185, 170);
    }
}

void ControlerScreen::drawEndingRidePage()
{
    this->_screen->setTextColor();
    this->_screen->setTextSize(2);
    this->_screen->setFont(2);

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        this->_screen->printText("Fin Trajet", this->_screen->calculateXCoordTextToCenter("Fin Trajet"), 75);
    }
    else // Anglais
    {
        this->_screen->printText("Ending Ride", this->_screen->calculateXCoordTextToCenter("Ending Ride"), 75);
    }

    this->_screen->setTextSize(1);
    this->_screen->setFont(1);
    String formatDurationHMS = this->_TSProperties->PropertiesCurrentRide.formatDurationHMS();

    if (this->_TSProperties->PropertiesTS.IsFrenchMode) // Français
    {
        String textDuration1 = "Votre trajet a duree ";
        String textDuration2 = formatDurationHMS + " heures.";
        this->_screen->printText(textDuration1, this->_screen->calculateXCoordTextToCenter(textDuration1), 120);
        this->_screen->printText(textDuration2, this->_screen->calculateXCoordTextToCenter(textDuration2), 145);

        String textDistance1 = "Et vous avez roulez ";
        String textDistance2 = String(this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters / 1000.0, 2) + " Km.";
        this->_screen->printText(textDistance1, this->_screen->calculateXCoordTextToCenter(textDistance1), 185);
        this->_screen->printText(textDistance2, this->_screen->calculateXCoordTextToCenter(textDistance2), 210);
    }
    else // Anglais
    {
        String textDuration = "Your ride last " + formatDurationHMS + " hours.";
        this->_screen->printText(textDuration, this->_screen->calculateXCoordTextToCenter(textDuration), 135);

        String textDistance = "And you ride " + String(this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters / 1000.0, 2) + " Km.";
        this->_screen->printText(textDistance, this->_screen->calculateXCoordTextToCenter(textDistance), 175);
    }
}

void ControlerScreen::drawErrorPage()
{
    this->_screen->drawError();
}

#pragma endregion DrawPages
