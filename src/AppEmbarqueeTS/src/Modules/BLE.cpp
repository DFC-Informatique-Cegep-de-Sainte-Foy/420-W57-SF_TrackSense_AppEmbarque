#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
// #include <BLESecurity.h>
// #include <BLE2902.h>

#include "Modules/BLE.h"
#include "Configurations.h"
#include "ControlerConfigurationFile.h"

/*----- Definition des membres statics -----*/
bool BLE::isDeviceConnected = false;
bool BLE::isCompletedRideStatsSending = false;
bool BLE::isCompletedRideStatsReceived = false;
bool BLE::isCompletedRidePointSending = false;
bool BLE::isCompletedRidePointReceived = false;
bool BLE::isAdvertiesingStarted = false;
int BLE::currentPointNumber = 0;

bool BLE::isNeedToUpdateTSProperties = false;
bool BLE::isRecivedTrajet = false;

/*----- CallBacks -----*/
class ServerBLECallbacks
    : public BLEServerCallbacks
{
    void onConnect(BLEServer *p_server)
    {
        BLE::isDeviceConnected = true;
        BLE::isAdvertiesingStarted = false;
        Serial.println("Connected!!!");
        DEBUG_STRING_LN(DEBUG_TS_BLE, "Connected");
    }

    void onDisconnect(BLEServer *p_server)
    {
        BLE::isDeviceConnected = false;
        Serial.println("Dis-Connected~~~~");
        DEBUG_STRING_LN(DEBUG_TS_BLE, "Disconnected");
    }
};

class CompletedRideReceiveDataCallbacks
    : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *p_characteristic)
    {
        std::string receivedData = p_characteristic->getValue();
        std::string acceptation = String(BLE::currentPointNumber).c_str();

        DEBUG_STRING_LN(DEBUG_TS_BLE, String("Callback Reception data: ") + String(receivedData.c_str()));
        DEBUG_STRING_LN(DEBUG_TS_BLE, String("Callback Acceptation data: ") + String(acceptation.c_str()));

        if (receivedData.compare(acceptation) == 0)
        {
            if (BLE::isCompletedRideStatsSending)
            {
                // DEBUG_STRING_LN(DEBUG_TS_BLE, "Callback reception stats");
                BLE::isCompletedRideStatsReceived = true;
                BLE::isCompletedRideStatsSending = false;
                BLE::currentPointNumber = 1;
            }
            else if (BLE::isCompletedRidePointSending)
            {
                // DEBUG_STRING_LN(DEBUG_TS_BLE, "Callback reception point");
                BLE::isCompletedRidePointReceived = true;
                BLE::isCompletedRidePointSending = false;
            }
            // p_characteristic->setValue("sending");
        }
    }
};

class ScreenRotateCallbacks
    : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *p_characteristic)
    {
        BLE::isNeedToUpdateTSProperties = true;
    }
};

//
class TrajetPlanifieCallbacks : public BLECharacteristicCallbacks
{
    // quand cette caractere est ecri
    void onWrite(BLECharacteristic *p_characteristic)
    {
        // TS recois un trajet
        BLE::isRecivedTrajet = true;
        String stringJSON = p_characteristic->getValue().c_str();
        Serial.println(stringJSON);
    }
    // quand cette caractere est lu
    void onRead(BLECharacteristic *p_characteristic)
    {
        // BLE::isNeedToUpdateTSProperties = true;
        Serial.println("on Read!");
    }
};

/*----- BLE -----*/
BLE::BLE(TSProperties *TSProperties, ISDCard *SD) : _TSProperties(TSProperties),
                                                    _sd(SD),
                                                    _serverBLE(nullptr),
                                                    _advertisingBLE(nullptr),
                                                    _completedRideService(nullptr),
                                                    _CRDataCharacteristic(nullptr),
                                                    _CRNotificationCharacteristic(nullptr),
                                                    _CRDataDescriptor(nullptr),
                                                    _CRNotificationDescriptor(nullptr),
                                                    _lastTimeStatsSent(0),
                                                    _lastTimePointSent(0),
                                                    _lastTimeAdvertiesingStarted(0),
                                                    _isBLELowPowerMode(false)
{
    this->initBLE();
    this->initServices();
    this->initCaracteristics();
    this->initDescriptors();
    this->startServices();
    this->initAdvertising();

    DEBUG_STRING_LN(DEBUG_TS_BLE, "Start Advertising");
    this->_serverBLE->startAdvertising();
    BLE::isAdvertiesingStarted = true;
};

BLE::~BLE()
{
    ;
};

void BLE::tick()
{
    // Serial.println("5---BlueTooth --> tick");
    this->_TSProperties->PropertiesBluetooth.IsDeviceBluetoothConnected = BLE::isDeviceConnected;

    if (BLE::isDeviceConnected)
    {
        // si un trajet est complete et pret a envoyer a App mobile
        if (this->_TSProperties->PropertiesCompletedRideToSend.IsReady)
        {
            this->_TSProperties->PropertiesCompletedRideToSend.IsStatsReceived = BLE::isCompletedRideStatsReceived;
            if (!BLE::isCompletedRideStatsReceived || BLE::isCompletedRideStatsSending) // Renvoie les stats tant qu'on a pas la confirmation de reception
            {                                                                           // Il faudrait peut-être vérifier si la Ride possède au moins 1 point...
                DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Send Stats");
                this->sendCompletedRideStats();
            }
            else if (BLE::isCompletedRidePointReceived)
            {
                DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Confirm Point Received");
                this->confirmPointReceived();
            }
            else if (BLE::isCompletedRideStatsReceived && (this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady))
            {
                DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Send Point");
                this->sendCompletedRideCurrentPoint();
            }
        }

        if (BLE::isRecivedTrajet)
        {
            BLE::isRecivedTrajet = false;
            // todo
            // Lire les donnes dans BLE et transferer a un obj de Trajet
            this->lancerTrajet();
        }
    }
    // Si non connecte avec App mobile, et TS est en mode StandBy et non en Broadcast  -> commencer a faire BroadCast
    else if (!this->_TSProperties->PropertiesTS.IsOnStanby && !BLE::isAdvertiesingStarted)
    {
        DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Restart Advertising");
        this->_serverBLE->startAdvertising();
        BLE::isAdvertiesingStarted = true;
    }
    // Si non connecte avec App mobile, et TS est en mode StandBy et en Broadcast  -> Arreter a faire BroadCast
    else if (this->_TSProperties->PropertiesTS.IsOnStanby && BLE::isAdvertiesingStarted)
    {
        DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Stop Advertising");
        BLEDevice::stopAdvertising();
        BLE::isAdvertiesingStarted = false;
    }

    // Si TS est en mode Demarrer et non en paused  -> commencer low BLE
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && !this->_TSProperties->PropertiesCurrentRide.IsRidePaused && !this->_isBLELowPowerMode)
    {
        DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Low Power Mode");
        BLEDevice::setPower(ESP_PWR_LVL_N6, ESP_BLE_PWR_TYPE_DEFAULT);
        this->_isBLELowPowerMode = true;
    }
    // Si TS est en LOW BLE et soit en non demarrer soit en en paused - > commencer NORMAL BLE
    else if (this->_isBLELowPowerMode && (!this->_TSProperties->PropertiesCurrentRide.IsRideStarted || this->_TSProperties->PropertiesCurrentRide.IsRidePaused))
    {
        DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Normal Mode");
        BLEDevice::setPower(ESP_PWR_LVL_P3, ESP_BLE_PWR_TYPE_DEFAULT);
        this->_isBLELowPowerMode = false;
    }

    // si besoin de changer TS_properties
    if (BLE::isNeedToUpdateTSProperties)
    {
        this->updateTSProperties();
    }
};

void BLE::initBLE()
{
    BLEDevice::init(BLE_DEVICE_NAME);

    this->_serverBLE = BLEDevice::createServer();
    this->_serverBLE->setCallbacks(new ServerBLECallbacks());
};

void BLE::initAdvertising()
{
    this->_advertisingBLE = this->_serverBLE->getAdvertising();
    this->_advertisingBLE->setScanResponse(true);
    this->_advertisingBLE->setMinPreferred(0x06);
    this->_advertisingBLE->setMinPreferred(0x12);
    this->_advertisingBLE->setAppearance(0x0000);

    this->_advertisingBLE->addServiceUUID(BLE_COMPLETED_RIDE_SERVICE_UUID);
    //
    this->_advertisingBLE->addServiceUUID(BLE_LED_SERVICE_UUID);
    this->_advertisingBLE->start();
};

void BLE::initServices()
{
    this->_completedRideService = this->_serverBLE->createService(BLE_COMPLETED_RIDE_SERVICE_UUID);
    this->_screenService = this->_serverBLE->createService(BLE_SCREEN_SERVICE_UUID);
    this->_receiveTrajetPlanifieService = this->_serverBLE->createService(BLE_LED_SERVICE_UUID);
};

void BLE::initCaracteristics()
{
    this->_CRDataCharacteristic = this->_completedRideService->createCharacteristic(BLE_COMPLETED_RIDE_CHARACTRISTIC_DATA, BLECharacteristic::PROPERTY_READ);
    this->_CRDataCharacteristic->setValue("id;plannedRideId;maxSpeed;avgSpeed;distance;duration;dateBegin;dateEnd;nbPoints;nbFalls");

    this->_CRNotificationCharacteristic = this->_completedRideService->createCharacteristic(BLE_COMPLETED_RIDE_NOTIFICATION_CHARACTRISTIC, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    this->_CRNotificationCharacteristic->setValue("sending");
    this->_CRNotificationCharacteristic->setCallbacks(new CompletedRideReceiveDataCallbacks());

    this->_screenRotateCharacteristic = this->_screenService->createCharacteristic(BLE_SCREEN_CHARACTRISTIC_ROTATE, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    this->_screenRotateCharacteristic->setValue(ControlerConfigurationFile::getValue(FIELD_SCREEN_ROTATION).c_str());
    this->_screenRotateCharacteristic->setCallbacks(new ScreenRotateCallbacks());

    //
    this->_trajetPlanifieCharacteristic = this->_receiveTrajetPlanifieService->createCharacteristic(BLE_LED_CHARACTRISTIC, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    this->_trajetPlanifieCharacteristic->setValue("LED allumer");
    this->_trajetPlanifieCharacteristic->setValue("id;nom;{}");

    this->_trajetPlanifieCharacteristic->setCallbacks(new TrajetPlanifieCallbacks());
};

void BLE::initDescriptors()
{
    this->_CRDataDescriptor = new BLEDescriptor(BLE_COMPLETED_RIDE_DESCRIPTOR_DATA_UUID); // 5a2b4a0f-8ddd-4c69-a825-dbab5822ba0e
    this->_CRDataDescriptor->setValue(BLE_COMPLETED_RIDE_DESCRIPTOR_DATA_NAME);
    this->_CRDataCharacteristic->addDescriptor(this->_CRDataDescriptor);

    this->_CRNotificationDescriptor = new BLEDescriptor(BLE_COMPLETED_RIDE_DESCRIPTOR_NOTIFICATION_UUID); // 6a2b4a0f-8ddd-4c69-a825-dbab5822ba0e
    this->_CRNotificationDescriptor->setValue(BLE_COMPLETED_RIDE_DESCRIPTOR_NOTIF_NAME);
    this->_CRNotificationCharacteristic->addDescriptor(this->_CRNotificationDescriptor);

    this->_screenRotateDescriptor = new BLEDescriptor(BLE_SCREEN_DESCRIPTOR_ROTATE_UUID); // 65000b05-c1a9-4dfb-a173-bdaa4a029bf7
    this->_screenRotateDescriptor->setValue(BLE_SCREEN_DESCRIPTOR_ROTATE_NAME);
    this->_screenRotateCharacteristic->addDescriptor(this->_screenRotateDescriptor);

    //
    this->_trajetPlanifieDescriptor = new BLEDescriptor(BLE_LED_CHARACTRISTIC); // 65000b05-c1a9-4dfb-a173-bdaa4a029cf7
    this->_trajetPlanifieDescriptor->setValue(BLE_LED_DESCRIPTOR);
    this->_trajetPlanifieCharacteristic->addDescriptor(this->_trajetPlanifieDescriptor);
};

void BLE::startServices()
{
    this->_completedRideService->start();
    this->_screenService->start();
    //
    this->_receiveTrajetPlanifieService->start();
};

void BLE::sendCompletedRideStats()
{
    unsigned long currentTime = millis();

    if ((currentTime - this->_lastTimeStatsSent) >= BLE_DELAY_SEND_STATS_MS)
    {
        this->_lastTimeStatsSent = currentTime;

        if (!BLE::isCompletedRideStatsSending)
        {
            this->_CRDataCharacteristic->setValue(this->_TSProperties->PropertiesCompletedRideToSend.Stats.c_str());
            BLE::isCompletedRideStatsSending = true;
            BLE::isCompletedRideStatsReceived = false;
            BLE::currentPointNumber = BLE_CONFIRME_STATS;
        }
        this->_CRNotificationCharacteristic->notify();

        DEBUG_STRING_LN(DEBUG_TS_BLE, "BLE Completed Ride stats sent");
    }
};

void BLE::sendCompletedRideCurrentPoint()
{
    unsigned long currentTime = millis();

    if ((currentTime - this->_lastTimePointSent) > BLE_DELAY_SEND_POINT_MS) // Envoie le point tant qu'on a pas la confirmation de reception
    {
        this->_lastTimePointSent = currentTime;

        if (!BLE::isCompletedRidePointSending)
        {
            this->_CRDataCharacteristic->setValue(this->_TSProperties->PropertiesCompletedRideToSend.Point.c_str());
            this->_CRNotificationCharacteristic->setValue("sending");
            BLE::currentPointNumber = this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber;
            BLE::isCompletedRidePointSending = true;
            DEBUG_STRING_LN(DEBUG_TS_BLE, String("Completed Ride Point ") + String(this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber) + String(" sent"));
        }
        // this->_CRNotificationCaracteristic->notify();
    }
};

void BLE::confirmPointReceived()
{
    BLE::isCompletedRidePointReceived = false;

    this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
    this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = true;

    if (this->_TSProperties->PropertiesCompletedRideToSend.NbPoints > this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber)
    {
        // this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
        // this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = true;
        DEBUG_STRING_LN(DEBUG_TS_BLE, String("Completed Ride Point ") + String(this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber) + String(" received"));
    }
    else
    {
        DEBUG_STRING_LN(DEBUG_TS_BLE, "End of points");
        // this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
        // this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = true;
        this->_TSProperties->PropertiesCompletedRideToSend.IsReady = false;
        this->_TSProperties->PropertiesCompletedRideToSend.IsReceived = true;
        BLE::isCompletedRideStatsReceived = false;
    }
};

void BLE::updateTSProperties()
{
    this->_TSProperties->PropertiesScreen.ScreenRotation = atoi(this->_screenRotateCharacteristic->getValue().c_str());
    this->_TSProperties->PropertiesScreen.IsScreenRotationChanged = true;
    ControlerConfigurationFile::setValue(FIELD_SCREEN_ROTATION, this->_screenRotateCharacteristic->getValue().c_str());

    BLE::isNeedToUpdateTSProperties = false;
};
void BLE::lancerTrajet()
{
    Serial.println("Trajet receving...");
    String stringJSON = this->_trajetPlanifieCharacteristic->getValue().c_str();
    Serial.println(stringJSON);
    // jsonString 2 Tranjet obj
    // Trajet t = Trajet::fromJsonStr2Trajet(stringJSON);
    // Serial.println(t.nom); // null
    // Sauvgarder un trajet dans SD
    _sd->SaveTrajet("/planifie", stringJSON);
    // Lancer un Trajet
}