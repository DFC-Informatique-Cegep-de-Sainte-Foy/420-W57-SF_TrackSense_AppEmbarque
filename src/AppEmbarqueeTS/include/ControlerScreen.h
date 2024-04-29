#pragma once
#include <Arduino.h>
#include "Interfaces/IControlerScreen.h"
#include "Configurations.h"
#include "TSProperties.h"

#include "Modules/ScreenGC9A01.h"
#include "StringQueue.h"
class ControlerScreen : public IControlerScreen
{
private:
    TSProperties *_TSProperties;
    ScreenGC9A01 *_screen;
    StringQueue *_trajetsSauvgardeSD;

    unsigned long _timeToDisplayEndingRidePageMS;
    bool entreCompass = false;
    float lastDegree = -1;

    SemaphoreHandle_t _xMutex; // Create a mutex object

    /* Pages */
    void drawInitTSPage() override;
    void drawHomePage() override;
    void drawCompassPage() override;
    void drawRideDirectionPage() override;
    void drawRidePage() override;
    void drawGlobalStatisticsPage() override;
    void drawGoHomePage() override;
    void drawRideStatisticsPage() override;
    void drawEndingRidePage() override;
    void drawErrorPage() override;
    void drawWatch() override;
    void drawTrajets(int) override;
    void drawDemarrerPrincipal() override;
    void drawPauseStop() override;
    void drawReDemarrer() override;
    void drawStatisticRide(int) override;

public:
    ControlerScreen(TSProperties *TSProperties, StringQueue *trajetsSD);
    ~ControlerScreen();

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

    void tick() override;
    void drawOnScreen() override;
    float calculerDirectionDegree();
};
