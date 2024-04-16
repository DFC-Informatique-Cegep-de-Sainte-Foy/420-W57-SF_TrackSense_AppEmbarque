#include "Program.h"
Program::Program() : _TSProperties(nullptr),
                     // _lte(nullptr),
                     //  _gsm(nullptr),
                     //  _sdCard(nullptr),
                     //  _gyroscope(nullptr),
                     //  _compass(nullptr),
                     //  _accelerometer(nullptr),
                     //  _ble(nullptr),
                     //  _buzzer(nullptr),
                     //  _controlerButtons(nullptr),
                     //  _controlerScreen(nullptr),
                     //  _MPU6050(nullptr),
                     _Compass(nullptr)
{
    this->_TSProperties = new TSProperties();
    // this->_controlerScreen = new ControlerScreen(this->_TSProperties);
    // this->_controlerButtons = new ControlerButtons(this->_TSProperties);
    // rajouter 6050
    // this->_MPU6050 = new MPU6050(this->_TSProperties);
    // rajouter QMC5883L
    this->_Compass = new GY87_Adafruit(this->_TSProperties);
    this->_Compass->calibrationHMC5883L();

    // this->_ble = new BLE(this->_TSProperties);
    // this->_sdCard = new SDCard(this->_TSProperties);
    // this->_gsm = new GSMTiny(this->_TSProperties);
    // this->_gyroscope = new GyroscopeMPU6050(this->_TSProperties);
    // this->_compass = new CompassHMC5883L(this->_TSProperties);
    // this->_accelerometer = new AccelerometerMPU6050(this->_TSProperties);
    // this->_buzzer = new Buzzer(this->_TSProperties);

    // this->_TSProperties->PropertiesTS.IsInitializingTS = false;
    // this->_TSProperties->PropertiesScreen.ActiveScreen = HOME_PAGE_ID;

    // this->_controlerButtons->resetLastDateChangementStateButtons();

    // pinMode(PIN_BATTERY, INPUT);

    // Serial.print("PIN_BATTERY voltage analogue : ");
    // Serial.print(analogRead(PIN_BATTERY));
    // Serial.println(" V");

    // Serial.print("PIN_BATTERY voltage digital : ");
    // Serial.print(digitalRead(PIN_BATTERY));
    // Serial.println(" V");
}

Program::~Program()
{
    ;
}

void Program::execute()
{
    // Serial.println("program in");
    // rajouter 6050
    // this->_MPU6050->tick();
    // rajouter QMC5883l
    this->_Compass->tick();
    // this->_controlerButtons->tick();
    // this->_buzzer->tick();
    // this->_controlerScreen->tick();
    // this->_ble->tick();
    // this->_gsm->tick();
    // this->_sdCard->tick();
    // this->_gyroscope->tick();
    // this->_compass->tick();
    // this->_accelerometer->tick();

    // float vref = 1100;
    // uint16_t analogueVoltage = analogRead(PIN_BATTERY);
    // float battery_voltage = ((float)analogueVoltage / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    // float battery_voltage = ((float)analogueVoltage / 4095.0) * 2.0 * 3.3 * (1100 / 1000.0);
    // long percent = map(battery_voltage, 0, 1023, 0, 100);

    // String voltage = "Voltage :" + String(battery_voltage) + "V\n";
    // Serial.println(voltage);

    // this->_TSProperties->PropertiesBattery.BatteryLevelPourcentage = (double)analogueVoltage;
    // Serial.println("program out");
}

void Program::executeCore0()
{
    DEBUG_STRING_LN(DEBUG_TS_CORE || DEBUG_TS_SCREEN, "                                             _controlerScreen->drawOnScreen()");
    // this->_controlerScreen->drawOnScreen();
}
