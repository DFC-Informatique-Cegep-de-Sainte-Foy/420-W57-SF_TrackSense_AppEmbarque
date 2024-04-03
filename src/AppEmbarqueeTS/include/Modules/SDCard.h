#pragma once

#include <SPI.h>
#include <SD.h>
#include <Arduino.h>

#include "Interfaces/ISDCard.h"
#include "Configurations.h"
#include "TSProperties.h"
#include "StringQueue.h"

class SDCard : public ISDCard
{
private:
    TSProperties *_TSProperties;
    // SD* _sd;

    StringQueue _queueCompletedRideIds;
    int _nbRidesInSDCard;
    bool _isRideStarted;
    File _currentPointsFile;
    String _currentPointsFileName;
    String _currentStatsFileName;

    File _currentFileSendPoints;
    unsigned long _positionCursorFileSendPoints;
    bool _isSendingRide;
    bool _isSendingPoints;

    void checkFiles();
    void createRideFiles();
    void processCurrentRide();
    void writeStatsFile();
    void writePoint();
    void processSendRide();
    void setStatsToSend();
    void setPointsToSendFromFile();
    void deleteCurrentRideFiles();

public:
    SDCard(TSProperties *TSProperties);
    ~SDCard();

    void init() override;
    void tick() override;
};
