#include "ControlerButtons.h"
#define ECRAN_TACTILE
// #define DEUX_BOUTON

ControlerButtons::ControlerButtons(TSProperties *TSProperties, ISDCard *sdCard) : _TSProperties(TSProperties),
                                                                                  _button1(nullptr),
                                                                                  _button2(nullptr),
                                                                                  // _isPressedButton1(false),
                                                                                  // _isPressedButton2(false),
                                                                                  _finalStateButton1(0),
                                                                                  _finalStateButton2(0),
                                                                                  _guidGenerator(nullptr),
                                                                                  _lastDateChangementStateButtons(millis()),
                                                                                  _finalGesture("NONE"),
                                                                                  _sd(sdCard)
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
/// @brief Changer le contenu affiché en effectuant différents gestes sur l'écran tactile :
/// dans la version actuelle, le TS dispose de quatre menus principaux d'écran différents :
///     l'écran Stand By,
///     l'écran Liste Trajet,
///     l'écran de démarrage et
///     l'écran Statistic Trajet ;
/// faites glisser vers la gauche ou la droite pour changer de menu principal d'écran.
/// Sous chaque menu principal d'écran, faites glisser vers le haut ou le bas
/// pour modifier l'affichage du sous-menu correspondant :
///     sous Stand By, il y a trois sous-écrans Compass-Direction-Watch ;
///     sous Liste Trajet, il y a cinq sous-écrans affichant différents trajets ;
///     sous Demarrer, il y a trois sous-écrans Direction-Statistic-Pause/STOP-Re-Demarrer ;
///     sous Statistic, il y a des écrans statistiques pour chaque trajet effectué et un écran statistique principal.
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

    // Serial.println("2---Button-Appuyer Detecte ");
    if (this->_finalStateButton1 != 0 || this->_finalStateButton2 != 0)
    {
        this->_lastDateChangementStateButtons = dateActuelle;
        this->_TSProperties->PropertiesTS.IsOnStanby = false;
        digitalWrite(TFT_BLK, HIGH); // Backlight on
    }

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

#ifdef ECRAN_TACTILE

    this->_finalGesture = this->_button1->getTouchGesture();
    long dateActuelle = millis();
    // Reveiller
    if (this->_finalGesture != "NONE")
    {
        this->_lastDateChangementStateButtons = dateActuelle;
        this->_TSProperties->PropertiesTS.IsOnStanby = false;
        digitalWrite(TFT_BLK, HIGH); // Backlight on
        this->_TSProperties->PropertiesScreen.estChange = true;
    }
    // Ecran Stand By : Stand By -> Compass -> Direction Home -> Watch
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
            if (this->_TSProperties->PropertiesScreen.ActiveScreen == 4)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--; // afficher dernier sous-ecran
            if (this->_TSProperties->PropertiesScreen.ActiveScreen == -1)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 3;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
    }
    // Ecran Liste_Trajet : Trajet 1 -> Trajet 2 -> Trajet 3 ...
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
            // 0 - 1 - 2 - 3 - (nombreRidePlanifie-1)
            this->_TSProperties->PropertiesScreen.ActiveScreen++;
            if (this->_TSProperties->PropertiesScreen.ActiveScreen >= this->_TSProperties->PropertiesSDCard.NombreRidePlanifie)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--;
            if (this->_TSProperties->PropertiesScreen.ActiveScreen < 0)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = this->_TSProperties->PropertiesSDCard.NombreRidePlanifie - 1;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen); //-1??
        }
        else if (this->_finalGesture == "SINGLE CLICK") // appuyer sur un Trajet
        {
            // Si appuyer sur un bouton, (190,90) (230,120) (190,150)
            if (this->_button1->getTouchPosition().first >= 190 && this->_button1->getTouchPosition().first <= 230 && this->_button1->getTouchPosition().second >= 90 && this->_button1->getTouchPosition().second <= 150)
            {
                int indexRide = this->_TSProperties->PropertiesScreen.ActiveScreen;
                // diriger l'ecran vers Demarrer - 0
                this->_TSProperties->PropertiesScreen.etat_Actuel = "DEMARRER"; // entrer dans ecran de DEMARRER
                this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
                // Trajet Start - >
                // devrait appler une methode dans la classe de TSProperty pour modifier les properties du Trajet
                // mais pas une methode de controlerButton; tell don't ask,mais ici on fait comme ca temporairement
                // il faut donner aussi un index pour faire savoir quel trajet dans le vecteur est demarre
                // le index est le numero d'active screen actuel avant de remettre a 0;
                this->startRide(indexRide);
                // this->_TSProperties->startTrajet();
                Serial.println("2 Demarrer");
            }
        }
    }
    // Ecran Demarrer : Demarrer Principal -> Compass -> Direction -> Statistic -> Pause/Stop -> Re-Demarrer
    else if (this->_TSProperties->PropertiesScreen.etat_Actuel == "DEMARRER")
    {
        if (this->_finalGesture == "SWIPE UP")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen++;

            // PAS DE PAUSE, PAS ECRAN RE-DEMARRER

            if (!this->_TSProperties->PropertiesCurrentRide.IsRidePaused && this->_TSProperties->PropertiesScreen.ActiveScreen == 5)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            }

            if (this->_TSProperties->PropertiesScreen.ActiveScreen == 5)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--;
            // PAS DE PAUSE, PAS ECRAN RE-DEMARRER
            if (!this->_TSProperties->PropertiesCurrentRide.IsRidePaused && this->_TSProperties->PropertiesScreen.ActiveScreen == 5)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 4;
            }

            if (this->_TSProperties->PropertiesScreen.ActiveScreen == -1)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 4;
            }

            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SINGLE CLICK") // appuyer sur un Bouton STOP ou Pause ou Re-Damarrer
        {
            if (this->_TSProperties->PropertiesScreen.ActiveScreen == 4) // appuyer sur ecran Pause/Stop
            {
                // STOP
                if (this->_button1->getTouchPosition().first >= 120 && this->_button1->getTouchPosition().second >= 150) // Bouton en bas a gauche pour STOP
                {
                    this->_TSProperties->PropertiesScreen.etat_Actuel = "Statistic_Trajet"; // entrer dans ecran de DEMARRER
                    this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
                    this->finishRide();
                    Serial.println("STOP -> 2 Statistic");
                }
                // PAUSE
                else if (this->_button1->getTouchPosition().first <= 120 && this->_button1->getTouchPosition().second >= 150) // Bouton en bas a droite pour Pause
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = 5;
                    this->pauseRide();
                    Serial.println("Pause -> 2 sous Ecran Re-Demarrer");
                }
            }
            else if (this->_TSProperties->PropertiesScreen.ActiveScreen == 5) // appuyer sur ecran Re-Demarrer
            {
                // Re-Demarrer
                if (this->_button1->getTouchPosition().first >= 100 && this->_button1->getTouchPosition().first <= 160 && this->_button1->getTouchPosition().second >= 160 && this->_button1->getTouchPosition().second <= 220) // bouton en bas pour Re-Demarrer
                {
                    this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
                    this->restartRide();
                    Serial.println("Re-Demarrer -> 2 sous Ecran Principal Demarrer");
                }
            }
        }
    }
    // Ecran Statistic
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
            if (this->_TSProperties->PropertiesScreen.ActiveScreen == 6)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
        else if (this->_finalGesture == "SWIPE DOWN")
        {
            this->_TSProperties->PropertiesScreen.ActiveScreen--;
            if (this->_TSProperties->PropertiesScreen.ActiveScreen == -1)
            {
                this->_TSProperties->PropertiesScreen.ActiveScreen = 5;
            }
            Serial.println(this->_TSProperties->PropertiesScreen.ActiveScreen);
        }
    }

#endif
}

void ControlerButtons::changePageUp()
{
    this->_TSProperties->PropertiesScreen.ActiveScreen++;

    if (this->_TSProperties->PropertiesScreen.ActiveScreen >= NB_ACTIVE_PAGES || this->_TSProperties->PropertiesScreen.ActiveScreen < 0)
    {
        this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
    }
}

void ControlerButtons::changePageDown()
{
    this->_TSProperties->PropertiesScreen.ActiveScreen--;

    if (this->_TSProperties->PropertiesScreen.ActiveScreen < 0 || this->_TSProperties->PropertiesScreen.ActiveScreen > NB_ACTIVE_PAGES)
    {
        this->_TSProperties->PropertiesScreen.ActiveScreen = NB_ACTIVE_PAGES - 1;
    }
}

void ControlerButtons::startRide(int p_index)
{

    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted == false)
    {
        DEBUG_STRING_LN(DEBUG_TS_BUTTONS, "===================== Start Ride =====================");
        // reset TS_Property et GPS
        this->_TSProperties->PropertiesCurrentRide.resetCurrentRide();
        this->_TSProperties->PropertiesGPS.resetGPSValues();
        // creer Trajet par index
        // this->_TSProperties->trajet = this->_sd->ReadTrajet(PATH_RIDE_PLANIFIE, _sd->GetJsonFileNamesAvecSuffixe(PATH_RIDE_PLANIFIE)[p_index]);
        this->_TSProperties->trajet = new Trajet(this->_sd->ReadTrajet(PATH_RIDE_PLANIFIE, _sd->GetJsonFileNamesAvecSuffixe(PATH_RIDE_PLANIFIE)[p_index]));
        // initialiser TSProperties
        this->_TSProperties->PropertiesCurrentRide.IsRideStarted = true;
        this->_TSProperties->PropertiesCurrentRide.IsRideFinished = false;
        this->_TSProperties->PropertiesCurrentRide.StartTimeMS = millis();
        this->_guidGenerator->seed(this->_TSProperties->PropertiesCurrentRide.StartTimeMS);
        this->_guidGenerator->generate();
        this->_TSProperties->PropertiesCurrentRide.CompletedRideId = this->_guidGenerator->toCharArray();
        this->_TSProperties->PropertiesCurrentRide.latitude_destination = this->_TSProperties->trajet->points->back().latitude;   //
        this->_TSProperties->PropertiesCurrentRide.longitude_destination = this->_TSProperties->trajet->points->back().longitude; //

        // initialiser Trajet
        this->_TSProperties->trajet->ride_id = this->_TSProperties->PropertiesCurrentRide.CompletedRideId;
        this->_TSProperties->trajet->dateBegin = this->_TSProperties->PropertiesCurrentRide.StartTimeMS;
        this->_TSProperties->trajet->estComplete = false;
        this->_TSProperties->trajet->estReadyToSave = false;
    }
    // Serial.println("Trajet Nom->" + this->_TSProperties->trajet->nom + "  Begin->  " + this->_TSProperties->trajet->dateBegin); // checked！
    // Serial.println("Destination latitude-> " + String(this->_TSProperties->PropertiesCurrentRide.latitude_destination));        // checked！
    // Serial.println("Destination longitude-> " + String(this->_TSProperties->PropertiesCurrentRide.longitude_destination));      // checked！
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

        // Trajet
        this->_TSProperties->trajet->estComplete = true;
        this->_TSProperties->trajet->dateEnd = this->_TSProperties->PropertiesCurrentRide.EndTimeMS;
        this->_TSProperties->trajet->duration = this->_TSProperties->PropertiesCurrentRide.DurationS;
        this->_TSProperties->trajet->vitesse_moyenne = this->_TSProperties->PropertiesCurrentRide.AverageSpeedKMPH;
        this->_TSProperties->trajet->estReadyToSave = true;
        String filename = this->_TSProperties->trajet->nom + "-" + String(this->_TSProperties->PropertiesGPS.Year) + "-" + String(this->_TSProperties->PropertiesGPS.Month) + "-" + String(this->_TSProperties->PropertiesGPS.Day) + "-" + String(this->_TSProperties->PropertiesGPS.Hour) + "-" + String(this->_TSProperties->PropertiesGPS.Minute) + "-" + String(this->_TSProperties->PropertiesGPS.Seconde) + "-" + String(this->_TSProperties->trajet->dateEnd);
        String json = this->_TSProperties->trajet->fromTrajet2Json();
        this->_sd->SaveTrajetComplete(PATH_RIDE_COMPLETE, String(PATH_RIDE_COMPLETE) + "/" + filename + ".txt", json);
        //
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
