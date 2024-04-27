#include "ControlerButtons.h"
#define ECRAN_TACTICL

ControlerButtons::ControlerButtons(TSProperties *TSProperties) : _TSProperties(TSProperties),
                                                                 _button1(nullptr),
                                                                 _button2(nullptr),
                                                                 // _isPressedButton1(false),
                                                                 // _isPressedButton2(false),
                                                                 _finalStateButton1(0),
                                                                 _finalStateButton2(0),
                                                                 _guidGenerator(nullptr),
                                                                 _lastDateChangementStateButtons(millis()),
                                                                 _finalGesture("NONE")
{
    // this->_button1 = new ButtonTactile(PIN_BUTTON1, _TSProperties);

    // TEST boutton tactile
    this->_button1 = new ButtonTouch(_TSProperties);
    this->_button2 = new ButtonTactile(PIN_BUTTON2, _TSProperties);

    this->_guidGenerator = new UUID();
    this->_guidGenerator->setRandomMode();
}

ControlerButtons::~ControlerButtons()
{
    ;
}

void ControlerButtons::tick()
{
#ifdef DEUX_BOUTON

    // Serial.println("1---Button-Affecter ");
    this->_finalStateButton1 = this->_button1->getFinalState(); // 0 == not pressed    // 1 == short press    // 2 == long press    // 3 == double short press
    this->_finalStateButton2 = this->_button2->getFinalState();
    long dateActuelle = millis();

    /*--------------------------------------Tester Effacer Chute------------------------------------------*/
    // if (this->_finalStateButton1 == 1 && this->_finalStateButton2 == 1)
    // {
    //     // 还原状态
    //     this->_TSProperties->PropertiesGPS.estChute = false;
    //     this->_TSProperties->PropertiesGPS.estEnvoyerSMS = false;
    //     Serial.println("Deux Bouttons Appuye! ");
    //     // Serial.println("TR est reset");
    //     // Serial.println("TR est reset");
    //     // Serial.println("TR est reset");
    // }
    /*---------------------------------------Tester Effacer Chute-----------------------------------------*/

#if DEBUG_TS_BUTTONS_HARDCORE
    this->_TSProperties->PropertiesButtons.Button1State = this->_finalStateButton1;
    this->_TSProperties->PropertiesButtons.Button2State = this->_finalStateButton2;
#endif
    // 说明按键了
    // Serial.println("2---Button-Appuyer Detecte ");
    if (this->_finalStateButton1 != 0 || this->_finalStateButton2 != 0)
    {
        this->_lastDateChangementStateButtons = dateActuelle;
        this->_TSProperties->PropertiesTS.IsOnStanby = false;
        digitalWrite(TFT_BLK, HIGH); // Backlight on
    }

    // 计算按键对应的值
    // Serial.println("3---Button-ControlState ");

    int controlerState = this->_finalStateButton1 + 4 * this->_finalStateButton2;
    // Serial.print("Button Controle State -> ");
    // Serial.println(controlerState);
    switch (controlerState)
    {
    case 0:
        /* Nothing Happened... */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "No button pressed");
        // Serial.println("   3-1 --Button-State--0 ");

        if (!this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
        {
            if (dateActuelle - this->_lastDateChangementStateButtons > this->_TSProperties->PropertiesButtons.TimeBeforeInactivityMS)
            {
                this->_TSProperties->PropertiesTS.IsOnStanby = true;
                DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "++++++++++++++++++++++ IsOnStanby = true ++++++++++++++++++++++");
            }
        }
        break;

    case 1:
        /* Change Page Up ---- ici 添加指南针罗盘？？？*/
        // Serial.println("   3-2 --Button-State--1 ");
        if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted) // À retirer pour quand on ajoutera la boussole
        {
            DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Button 1 SHORT press");
            this->changePageUp();
        }
        break;

    case 2:
        /* Start/Stop Ride */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Button 1 LONG press");

        if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
        {
            this->finishRide();
        }
        else
        {
            // 更新当前行程的数据
            this->startRide();
        }
        break;

    case 3:
        /* Trigger The Buzzer */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Button 1 DOUBLE SHORT press"); // Impossible !!!
        this->makeNoiseBuzzer();
        break;

    case 4:
        /* Change Page Down */
        if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted) // À retirer pour quand on ajoutera la boussole
        {
            DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Button 2 SHORT press");
            this->changePageDown();
        }
        break;

    case 5:
        /* Activate GoHome Mode */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Buttons 1 and 2 SHORT press");
        this->goHome();
        break;

    case 8:
        /* Pause/Restart Ride */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Button 2 LONG press");

        if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
        {
            if (this->_TSProperties->PropertiesCurrentRide.IsRidePaused)
            {
                this->restartRide();
            }
            else
            {
                this->pauseRide();
            }
        }
        break;

    case 10:
        /* Trigger The Buzzer */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Buttons 1 and 2 LONG press");
        this->makeNoiseBuzzer();
        break;

    case 12:
        /* Trigger The Buzzer */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "Button 2 DOUBLE SHORT press"); // Impossible !!!
        this->makeNoiseBuzzer();
        break;

    default:
        /* Nothing Good Happened... */
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "BUTTONS ERROR !!!");
        break;
    }
#endif

#ifdef ECRAN_TACTICL

    this->_finalGesture = this->_button1->getTouchGesture();
    long dateActuelle = millis();
    // Reveiller
    if (this->_finalGesture != "NONE")
    {
        this->_lastDateChangementStateButtons = dateActuelle;
        this->_TSProperties->PropertiesTS.IsOnStanby = false;
        digitalWrite(TFT_BLK, HIGH); // Backlight on
    }
    if (this->_TSProperties->PropertiesScreen.etat_Actuel == "STAND_BY")
    {
        if (this->_finalGesture == "SWIPE LEFT") // entrer dans ecran Liste_Trajet
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "Liste_Trajet";
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0; // afficher premier sous-ecran
            Serial.println("2 Liste");
        }
        else if (this->_finalGesture == "SWIPE RIGHT") // entrer dans ecran Statistic_Trajet
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "Statistic_Trajet";
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0; // afficher premier sous-ecran
            Serial.println("2 Statistic");
        }
        else if (this->_finalGesture == "SWIPE UP")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen++; // afficher prochain sous-ecran
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--; // afficher dernier sous-ecran
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
    }
    else if (this->_TSProperties->PropertiesScreen.etat_Actuel == "Liste_Trajet")
    {
        if (this->_finalGesture == "SWIPE LEFT")
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "Statistic_Trajet";
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            Serial.println("2 Statistic");
        }
        else if (this->_finalGesture == "SWIPE RIGHT")
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "STAND_BY";
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            Serial.println("2 Stand By");
        }
        else if (this->_finalGesture == "SWIPE UP")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen++;
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--;
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SINGLE CLICK") // appuyer sur un Trajet
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "DEMARRER"; // entrer dans ecran de DEMARRER
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            Serial.println("2 Demarrer");
        }
    }
    else if (this->_TSProperties->PropertiesScreen.etat_Actuel == "Demarrer")
    {
        if (this->_finalGesture == "SWIPE UP")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen++;
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--;
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SINGLE CLICK") // appuyer sur un Bouton STOP ou Pause ou Re-Damarrer
        {
            if (this->_TSProperties->PropertiesScreen.ActiveScreen == 0) // appuyer sur ecran Demarrer
            {
                // STOP
                if (this->_button1->getTouchPosition().first >= 150 && this->_button1->getTouchPosition().second <= 119)
                {
                    this->_TSProperties->PropertiesScreen.etat_Actuel = "Statistic_Trajet"; // entrer dans ecran de DEMARRER
                    this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
                    Serial.println("STOP -> 2 Statistic");
                }
                // PAUSE
                else if (this->_button1->getTouchPosition().first >= 150 && this->_button1->getTouchPosition().second >= 121)
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = 1;
                    Serial.println("Pause -> 2 sous Ecran 1");
                }
            }
            else if (this->_TSProperties->PropertiesScreen.ActiveScreen == 1) // appuyer sur ecran Re-Demarrer
            {
                // Re-Demarrer
                if (this->_button1->getTouchPosition().first >= 150)
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
                    Serial.println("Re-Demarrer -> 2 sous Ecran 0");
                }
            }
        }
    }
    else if (this->_TSProperties->PropertiesScreen.etat_Actuel == "Statistic_Trajet")
    {
        if (this->_finalGesture == "SWIPE LEFT")
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "STAND_BY";
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            Serial.println("2 Stand By");
        }
        else if (this->_finalGesture == "SWIPE RIGHT")
        {
            this->_TSProperties->PropertiesScreen.etat_Actuel = "Liste_Trajet";
            this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            Serial.println("2 Liste Trajet");
        }
        else if (this->_finalGesture == "SWIPE UP")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen++;
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--;
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
    }

    // pseudocode
    /*
        1 - prendre gesture de touch
        2 - verifier l'etat actuel de TS
        3 - MAJ l'etat de TS selon l'etat actuel et gesture de touch
--------------------------------------------------------------------------
    1)  this->_finalStateButton1 = this->_button1->getFinalState();

    2)  Ecran 1
        if this-> _TS_etat_Actuel == StandBy
            if _finalStateButton1 == "SWIPE RIGHT"   →
                then this-> _TS_etat_Actuel = Liste_Trajets
                then this-> _TS_Ecran_Active = 0

            else if _finalStateButton1 == "SWIPE LEFT"   ←
                then this-> _TS_etat_Actuel = Statistic_Trajet
                then this-> _TS_Ecran_Active = 0

            else if _finalStateButton1 == "SWIPE UP"   ↑
                then this-> _TS_etat_Actuel = StandBy
                then this->  _TS_Ecran_Active ++

            else if _finalStateButton1 == "SWIPE DOWN"  ↓
                then this-> _TS_etat_Actuel = StandBy
                then this-> _TS_Ecran_Active --
        Ecran 2
        if this-> _TS_etat_Actuel == Liste_Trajet
            if _finalStateButton1 == "SWIPE RIGHT"   →
                then this-> _TS_etat_Actuel = Statistic_Trajet
                then this-> _TS_Ecran_Active = 0

            else if _finalStateButton1 == "SWIPE LEFT"   ←
                then this-> _TS_etat_Actuel = Stand_By
                then this-> _TS_Ecran_Active = 0

            else if _finalStateButton1 == "SWIPE UP"   ↑
                then this-> _TS_etat_Actuel = Liste_Trajet
                then this->  _TS_Ecran_Active ++

            else if _finalStateButton1 == "SWIPE DOWN"  ↓
                then this-> _TS_etat_Actuel = Liste_Trajet
                then this-> _TS_Ecran_Active --

            else if _finalStateButton1 == "SINGLE CLICK"
                then this-> _TS_etat_Actuel = Demarrer

        Ecran 3
        if this-> _TS_etat_Actuel == Demarrer
            else if _finalStateButton1 == "SWIPE UP"   ↑
                then this-> _TS_etat_Actuel = Demarrer
                then this->  _TS_Ecran_Active ++

            else if _finalStateButton1 == "SWIPE DOWN"  ↓
                then this-> _TS_etat_Actuel = Demarrer
                then this-> _TS_Ecran_Active --

            else if _finalStateButton1 == "SINGLE CLICK" sur "STOP"
                then this-> _TS_etat_Actuel = Statistic_Trajets

            else if _finalStateButton1 == "SINGLE CLICK" sur "Pause"
                then this-> _TS_etat_Actuel = Pause
                then this-> _TS_Ecran_Actuel = Pause  ??? rajouter un Bouton ou rajouter un ecran

            else if _finalStateButton1 == "SINGLE CLICK" sur "Re-Demarrer"
                then this-> _TS_etat_Actuel = Demarrer

        Ecran 4
        if this-> _TS_etat_Actuel == Statistic_Trajet
            if _finalStateButton1 == "SWIPE RIGHT"   →
                then this-> _TS_etat_Actuel = Stand By
                then this-> _TS_Ecran_Active = 0

            else if _finalStateButton1 == "SWIPE LEFT"   ←
                then this-> _TS_etat_Actuel = Liste_Trajets
                then this-> _TS_Ecran_Active = 0
    */
#endif
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
void ControlerButtons::changePageUp()
{
    this->_TSProperties->PropertiesScreen.ActiveScreen++;

    if (this->_TSProperties->PropertiesScreen.ActiveScreen >= NB_ACTIVE_PAGES || this->_TSProperties->PropertiesScreen.ActiveScreen < 0)
    {
        this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
    }
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
void ControlerButtons::changePageDown()
{
    this->_TSProperties->PropertiesScreen.ActiveScreen--;

    if (this->_TSProperties->PropertiesScreen.ActiveScreen < 0 || this->_TSProperties->PropertiesScreen.ActiveScreen > NB_ACTIVE_PAGES)
    {
        this->_TSProperties->PropertiesScreen.ActiveScreen = NB_ACTIVE_PAGES - 1;
    }
}

void ControlerButtons::startRide()
{
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted == false)
    {
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "===================== Start Ride =====================");
        this->_TSProperties->PropertiesCurrentRide.resetCurrentRide();
        this->_TSProperties->PropertiesGPS.resetGPSValues();

        this->_TSProperties->PropertiesCurrentRide.IsRideStarted = true;
        this->_TSProperties->PropertiesCurrentRide.IsRideFinished = false;

        this->_TSProperties->PropertiesCurrentRide.StartTimeMS = millis();

        this->_guidGenerator->seed(this->_TSProperties->PropertiesCurrentRide.StartTimeMS);
        this->_guidGenerator->generate();

        this->_TSProperties->PropertiesCurrentRide.CompletedRideId = this->_guidGenerator->toCharArray();
        this->_TSProperties->PropertiesScreen.ActiveScreen = RIDE_PAGE_ID;

        // 更新TS的终点坐标,坐标应是存储在数据库中的某段路程的终点坐标
        // HOME
        // this->_TSProperties->PropertiesCurrentRide.latitude_destination = 46.78772;   //
        // this->_TSProperties->PropertiesCurrentRide.longitude_destination = -71.26219; //

        // Chateau Frontenac 坐标
        // this->_TSProperties->PropertiesCurrentRide.latitude_destination = 46.81207;   //
        // this->_TSProperties->PropertiesCurrentRide.longitude_destination = -71.20501; //

        // Test Garneau 坐标
        // this->_TSProperties->PropertiesCurrentRide.latitude_destination = 46.79308;   //
        // this->_TSProperties->PropertiesCurrentRide.longitude_destination = -71.26474; //

        // Test 418 坐标
        this->_TSProperties->PropertiesCurrentRide.latitude_destination = 46.78570;   //
        this->_TSProperties->PropertiesCurrentRide.longitude_destination = -71.28714; //
    }
}

void ControlerButtons::finishRide()
{
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
    {

        this->_TSProperties->PropertiesCurrentRide.IsRideFinished = true;
        this->_TSProperties->PropertiesCurrentRide.IsRideStarted = false;

        this->_TSProperties->PropertiesCurrentRide.EndTimeMS = millis();
        this->_TSProperties->PropertiesCurrentRide.DurationS = (this->_TSProperties->PropertiesCurrentRide.EndTimeMS - this->_TSProperties->PropertiesCurrentRide.StartTimeMS) / 1000;

        this->_TSProperties->PropertiesCurrentRide.IsRideReadyToSave = true;

        this->_TSProperties->PropertiesScreen.ActiveScreen = ENDING_RIDE_PAGE_ID;

        this->_TSProperties->PropertiesGPS.CounterTotal = 0;
        this->_TSProperties->PropertiesGPS.CounterGoodValue = 0;

        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "===================== Finish Ride =====================");
    }
}

void ControlerButtons::pauseRide()
{
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
    {
        this->_TSProperties->PropertiesCurrentRide.IsRidePaused = true;
    }
}

void ControlerButtons::restartRide()
{
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted)
    {
        this->_TSProperties->PropertiesCurrentRide.IsRidePaused = false;
    }
}

void ControlerButtons::makeNoiseBuzzer()
{
    this->_TSProperties->PropertiesBuzzer.IsBuzzerOn = true;
}

void ControlerButtons::goHome()
{
    this->_TSProperties->PropertiesScreen.ActiveScreen = GO_HOME_PAGE_ID;
}

void ControlerButtons::resetLastDateChangementStateButtons()
{
    this->_lastDateChangementStateButtons = millis();
}
