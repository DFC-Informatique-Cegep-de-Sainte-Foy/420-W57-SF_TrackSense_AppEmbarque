#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLESecurity.h>
#include <BLE2902.h>

#include "Interfaces/IBLE.h"
#include "Configurations.h"
#include "TSProperties.h"
#include "Trajet.h"

class BLE
    : public IBLE
{
private:
    TSProperties *_TSProperties;

    BLEServer *_serverBLE;
    BLEAdvertising *_advertisingBLE;

    BLEService *_completedRideService;
    BLECharacteristic *_CRDataCharacteristic;
    BLECharacteristic *_CRNotificationCharacteristic;
    BLEDescriptor *_CRDataDescriptor;
    BLEDescriptor *_CRNotificationDescriptor;

    BLEService *_screenService;
    BLECharacteristic *_screenRotateCharacteristic;
    BLEDescriptor *_screenRotateDescriptor;

    BLEService *_allumerLEDService;
    BLECharacteristic *_ledCharacteristic;
    BLEDescriptor *_ledAllumerDescriptor;

    unsigned long _lastTimeStatsSent;
    unsigned long _lastTimePointSent;
    unsigned long _lastTimeAdvertiesingStarted;

    bool _isBLELowPowerMode;

    void initBLE();
    void initAdvertising();
    void initServices();
    void initCaracteristics();
    void initDescriptors();
    void startServices();
    void sendCompletedRideStats();
    void sendCompletedRideCurrentPoint();
    void confirmPointReceived();

    void updateTSProperties();

    //
    void lancerTrajet();

public:
    static bool isDeviceConnected;
    static bool isCompletedRideStatsSending;
    static bool isCompletedRideStatsReceived;
    static bool isCompletedRidePointSending;
    static bool isCompletedRidePointReceived;
    static bool isAdvertiesingStarted;
    static int currentPointNumber;

    static bool isNeedToUpdateTSProperties;
    static bool isRecivedTrajet;

    BLE(TSProperties *TSProperties);
    ~BLE();

    void tick() override;
};
