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
    this->_controlerScreen = new ControlerScreen(this->_TSProperties, this->_trajetsSD);
    this->_controlerButtons = new ControlerButtons(this->_TSProperties);
    this->_ble = new BLE(this->_TSProperties);
    this->_sdCard = new SDCard(this->_TSProperties, this->_trajetsSD);
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
}

Program::~Program()
{
    ;
}

void Program::executeCore1()
{
    DEBUG_STRING_LN(DEBUG_TS_CORE, "_controlerButtons");
    this->_controlerButtons->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_buzzer");
    this->_buzzer->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_battery");
    this->_battery->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_controlerScreen->tick()");
    this->_controlerScreen->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_ble");
    // this->_ble->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_gsm");
    this->_gsm->tick();

    DEBUG_STRING_LN(DEBUG_TS_CORE, "_sdCard");
    // this->_sdCard->tick();

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