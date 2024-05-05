#pragma once

#include <SPI.h>
#include <SD.h>
#include <Arduino.h>

#include "Interfaces/ISDCard.h"
#include "Configurations.h"
#include "TSProperties.h"
#include "StringQueue.h"
#include "Trajet.h"
class SDCard : public ISDCard
{
private:
    TSProperties *_TSProperties;

    // SD* _sd;
    StringQueue *_queueCompletedRideIds;
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
    void processCurrentRide();
    void writeStatsFile();
    void writePoint();
    void processSendRide();
    void setStatsToSend();
    void setPointsToSendFromFile();
    void deleteCurrentRideFiles();
    void deleteCurrentRideSentFiles();

public:
    SDCard(TSProperties *TSProperties, StringQueue *trajetSD);
    ~SDCard();
    // Trajet
    Trajet *_trajet;
    void init() override;
    void tick() override;
    void SaveTrajet(String p_path, Trajet *p_trajet);
    Trajet ReadTrajet(String p_path, String p_fileName);
    // test
    void createRideFiles();
    void writeFile();
    static void creerDir(String p_dir);
};
