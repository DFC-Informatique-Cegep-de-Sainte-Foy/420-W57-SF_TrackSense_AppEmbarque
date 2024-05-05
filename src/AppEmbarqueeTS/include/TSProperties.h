#pragma once
#include <Arduino.h>
#include "Configurations.h"

class TSProperties
{
public:
    /* Datas TrackSense */
    struct TSPropertiesTS
    {
        bool IsInitializingTS;
        bool IsInitializedGSM;
        bool IsOnStanby;   // mode veille
        bool IsFrenchMode; // mode français. If false, mode english. // TODO : Take from Configuration.json file
    } PropertiesTS;

    /* datas Current ride */
    struct TSPropertiesCurrentRide
    {
        double latitude_destination;  // y
        double longitude_destination; // x
        double latitude_point_depart;
        double longitude_point_depart;

        bool IsRideStarted;
        bool IsRidePaused;
        bool IsRideFinished;
        bool estArrive;

        String CompletedRideId;
        String PlannedRideId;
        float MaxSpeedKMPH;
        float AverageSpeedKMPH;
        double DistanceTotalMeters;

        unsigned long DurationS;
        unsigned long StartTimeMS;
        unsigned long EndTimeMS;

        String DateBegin;
        String DateEnd;
        String CurrentPoint;
        int NbPoints;
        int PointID;
        int NbFalls; //??nombre de chute?
        bool IsRideReadyToSave;
        bool IsPointReadyToSave;
        float Temperature;

        String formatDurationHMS()
        {
            uint dureeHour = this->DurationS / 3600;
            uint dureeMinutes = (this->DurationS % 3600) / 60;
            uint dureeSecondes = this->DurationS % 60;
            char buffer[9]; // Un tampon pour stocker la chaîne formatée
            sprintf(buffer, "%02d:%02d:%02d", dureeHour, dureeMinutes, dureeSecondes);
            return String(buffer);
        }

        void formatDurationHMS(String &text)
        {
            uint dureeHour = this->DurationS / 3600;
            uint dureeMinutes = (this->DurationS % 3600) / 60;
            uint dureeSecondes = this->DurationS % 60;
            char buffer[9]; // Un tampon pour stocker la chaîne formatée
            sprintf(buffer, "%02d:%02d:%02d", dureeHour, dureeMinutes, dureeSecondes);
            text = String(buffer);
        }

        void resetCurrentRide()
        {
            latitude_destination = 0;
            longitude_destination = 0;

            this->IsRideStarted = false;
            this->IsRidePaused = false;
            this->IsRideFinished = false;

            this->CompletedRideId = "00000000-0000-0000-0000-000000000000";
            this->PlannedRideId = "00000000-0000-0000-0000-000000000000";
            this->MaxSpeedKMPH = 0;
            this->AverageSpeedKMPH = 0;
            this->DistanceTotalMeters = 0;

            this->DurationS = 0;
            this->DateBegin = "0000-00-00T00:00:00";
            this->DateEnd = "0000-00-00T00:00:00";
            this->CurrentPoint = "";
            this->NbPoints = 0;
            this->PointID = 0;
            this->NbFalls = 0;
            this->IsRideReadyToSave = false;
            this->IsPointReadyToSave = false;
            this->Temperature = 0;
            this->StartTimeMS = 0;
            this->EndTimeMS = 0;
        }
    } PropertiesCurrentRide;

    /* datas GPS */
    struct TSPropertiesGPS
    {
        // datas brute GPS
        float Home_Latitude = 46.78552;   // Cegep
        float Home_Longitude = -71.28686; // Cegep
        float Latitude;
        float Longitude;
        float Altitude;
        float Speed;
        int VisibleSatellites;
        int UsedSatellites;
        float Accuracy;
        int Year;
        int Month;
        int Day;
        int Hour;
        int Minute;
        int Seconde;
        bool IsFixValid;
        bool IsGPSFixed;
        bool estChute;
        bool estEnvoyerSMS;
        float Distance2Home;

        int CounterGoodValue;
        int CounterTotal;

        float mph() { return 0.621371 * Speed; }
        float miles() { return 0.00062137112 * Altitude; }

        void resetGPSValues()
        {
            this->Latitude = 0;
            this->Longitude = 0;
            this->Altitude = 0;
            this->Speed = 0;
            this->VisibleSatellites = 0;
            this->UsedSatellites = 0;
            this->Accuracy = 0;
            this->Year = 0;
            this->Month = 0;
            this->Day = 0;
            this->Hour = 0;
            this->Minute = 0;
            this->Seconde = 0;
            this->CounterGoodValue = 0;
            this->CounterTotal = 0;
            this->IsFixValid = false;
            this->estChute = false;
            this->estEnvoyerSMS = false;
        }
    } PropertiesGPS;

    /* datas Buttons */
    struct TSPropertiesButtons
    {
        // bool IsButton1Pressed;
        // bool IsButton2Pressed;
        int Button1State;
        int Button2State;
        unsigned long TimeBeforeInactivityMS; // BUTTON_INACTIVITY_TIME_MS

    } PropertiesButtons;

    /* datas Screen */
    struct TSPropertiesScreen
    {
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
        String etat_Actuel;
        int ActiveScreen; // Seul paramètre modifier par le controlerScreen
        bool IsDarkMode;

        /*
            0 : Pins vers le haut
            1 : Pins vers la gauche
            2 : Pins vers le bas
            3 : Pins vers la droite
        */
        uint8_t ScreenRotation;
        bool IsScreenRotationChanged;

    } PropertiesScreen;

    /* datas SD Card */
    struct TSPropertiesSDCard
    {
        bool IsSDCardConnected;
    } PropertiesSDCard;

    /* datas Bluetooth */
    struct TSPropertiesBluetooth
    {
        bool IsDeviceBluetoothConnected;

        // bool IsBluetoothConnected;
        // bool IsBluetoothActivated;
    } PropertiesBluetooth;

    /* datas LTE */
    struct TSPropertiesLTE
    {
        // bool IsDeviceLTEConnected;
    } PropertiesLTE;

    /* datas Wifi */
    struct TSPropertiesWifi
    {
        ;
    } PropertiesWifi;

    /* datas Compass */
    struct TSPropertiesCompass
    {
        // datas brute Compass
        float Acc_X;
        float Acc_Y;
        float Acc_Z;
        float Gyro_X;
        float Gyro_Y;
        float Gyro_Z;
        float Mag_X;
        float Mag_Y;
        float Mag_Z;

        float heading;
        float pitch;
        float roll;
        float yaw;
        void resetCompassValues()
        {
            // data raw
            this->Acc_X;
            this->Acc_Y;
            this->Acc_Z;
            this->Gyro_X;
            this->Gyro_Y;
            this->Gyro_Z;
            this->Mag_X;
            this->Mag_Y;
            this->Mag_Z;
            // data normalise
            this->heading;
            this->pitch;
            this->roll;
            this->yaw;
        }

    } PropertiesCompass;

    /* datas Gyroscope */
    struct TSPropertiesGyroscope
    {
        // double RollAngle;  // PositionRoulis
        // double PitchAngle; // PositionTangage
        // double YawAngle;   // PositionLacet
    } PropertiesGyroscope;

    /* datas Accelerometer */
    struct TSPropertiesAccelerometer
    {
        // double Acceleration_x;
        // double Acceleration_y;
        // double Acceleration_z;
    } PropertiesAccelerometer;

    /* datas Temperature */
    struct TSPropertiesTemperature
    {
        double Temperature;
    } PropertiesTemperature;

    /* datas Battery */
    struct TSPropertiesBattery
    {
        float BatteryLevelPourcentage;
    } PropertiesBattery;

    /* datas Buzzer */
    struct TSPropertiesBuzzer
    {
        bool IsBuzzerOn;
    } PropertiesBuzzer;

    /* datas CompletedRide to send */
    struct TSPropertiesCompletedRideToSend
    {
        String CompletedRideId;
        String Stats;
        String Point;
        int CurrentPointNumber;
        int NbPoints;

        bool IsStatsReceived;

        bool IsPointReady;
        bool IsPointReceived;

        bool IsReady;
        bool IsReceived;
    } PropertiesCompletedRideToSend;

    /* Methods */
    TSProperties();
    ~TSProperties();

    void initProperties();
};
