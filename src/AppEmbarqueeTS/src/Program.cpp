#include "Program.h"

Program::Program() : _TSProperties(nullptr),
                     _gsm(nullptr),
                     _sdCard(nullptr),
                     _gyroscope(nullptr),
                     _compass(nullptr),
                     _accelerometer(nullptr),
                     _ble(nullptr),
                     _buzzer(nullptr),
                     _battery(nullptr),
                     _controlerButtons(nullptr),
                     _controlerScreen(nullptr),
                     _gy87(nullptr)
{
    this->_TSProperties = new TSProperties();
    this->_trajetsSD = new StringQueue();
    this->_sdCard = new SDCard(this->_TSProperties, this->_trajetsSD);
    this->_controlerButtons = new ControlerButtons(this->_TSProperties, this->_sdCard);

    this->_controlerScreen = new ControlerScreen(this->_TSProperties, this->_trajetsSD, this->_sdCard);
    this->_ble = new BLE(this->_TSProperties, this->_sdCard);
    this->_gsm = new GSMTiny(this->_TSProperties);
    // this->_gsm = new MyTinyGsm(this->_TSProperties);
    this->_gyroscope = new GyroscopeMPU6050(this->_TSProperties);
    this->_compass = new CompassHMC5883L(this->_TSProperties);
    this->_accelerometer = new AccelerometerMPU6050(this->_TSProperties);
    this->_buzzer = new Buzzer(this->_TSProperties);
    this->_battery = new Battery(this->_TSProperties);

    this->_TSProperties->PropertiesTS.IsInitializingTS = false;

    this->_gy87 = new GY87_Adafruit(this->_TSProperties);
    this->_controlerButtons->resetLastDateChangementStateButtons();
    this->_TSProperties->PropertiesScreen.etat_Actuel = "STAND_BY"; // Ecran est pret
    // this->_TSProperties->PropertiesScreen.ActiveScreen = HOME_PAGE_ID;
    this->_TSProperties->PropertiesScreen.ActiveScreen = 0;
    this->_TSProperties->PropertiesSDCard.NombreRidePlanifie = this->_sdCard->NumTrajet(PATH_RIDE_PLANIFIE);
    Serial.println("Num Trajets -> " + String(this->_TSProperties->PropertiesSDCard.NombreRidePlanifie));
}

Program::~Program()
{
    ;
}

void Program::executeCore1()
{
    // if (this->_TSProperties->PropertiesGPS.targetTime < millis())
    // {
    //     // Set next update for 1 second later
    //     this->_TSProperties->PropertiesGPS.targetTime = millis() + 1000;
    //     this->_TSProperties->PropertiesGPS.ss++; // Advance second

    //     if (this->_TSProperties->PropertiesGPS.ss == 60)
    //     {                                                                                   // Check for roll-over
    //         this->_TSProperties->PropertiesGPS.ss = 0;                                      // Reset seconds to zero
    //         this->_TSProperties->PropertiesGPS.omm = this->_TSProperties->PropertiesGPS.mm; // Save last minute time for display update
    //         this->_TSProperties->PropertiesGPS.mm++;                                        // Advance minute
    //         if (this->_TSProperties->PropertiesGPS.mm > 59)
    //         { // Check for roll-over
    //             this->_TSProperties->PropertiesGPS.mm = 0;
    //             this->_TSProperties->PropertiesGPS.hh++; // Advance hour
    //             if (this->_TSProperties->PropertiesGPS.hh > 23)
    //             {                                              // Check for 24hr roll-over (could roll-over on 13)
    //                 this->_TSProperties->PropertiesGPS.hh = 0; // 0 for 24 hour clock, set to 1 for 12 hour clock
    //             }
    //         }
    //     }
    // }

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_controlerButtons");
    this->_controlerButtons->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_buzzer");
    this->_buzzer->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_battery");
    this->_battery->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_controlerScreen->tick()");
    this->_controlerScreen->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_ble");
    this->_ble->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_gsm");
    this->_gsm->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_sdCard");
    this->_sdCard->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_gyroscope");
    // this->_gyroscope->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_compass");
    // this->_compass->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_accelerometer");
    // this->_accelerometer->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_accelerometer");
    this->_gy87->tick();
}

void Program::executeCore0()
{
    DEBUG_STRING_LN(DEBUG_TS_CORE || DEBUG_TS_SCREEN, " _controlerScreen->drawOnScreen()");
    this->_controlerScreen->drawOnScreen();
}