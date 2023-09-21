#include "Modules/SDCard.h"
#include "Configurations.h"
#include "StringQueue.h"

SDCard::SDCard(TSProperties* TSProperties) 
    : _TSProperties(TSProperties),
    _queueCompletedRideIds(),
    _nbRidesInSDCard(0),
    _isRideStarted(false),
    _currentPointsFile(),
    // _currentRideId(""),
    _currentPointsFileName(""),
    _currentStatsFileName(""),
    _currentFileSendPoints(),
    _positionCursorFileSendPoints(0),
    _isSendingPoints(false)
{
    this->_queueCompletedRideIds = StringQueue();
    this->init();
};

SDCard::~SDCard()
{
};

void SDCard::init()
{
    Serial.println("========SDCard Detect.======");
    SPI.begin(PIN_SDCARD_SCLK, PIN_SDCARD_MISO, PIN_SDCARD_MOSI);

    if (!SD.begin(PIN_SDCARD_CS))
    {
        Serial.println("SDCard MOUNT FAIL");
    }
    else
    {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        String str = "SDCard Size: " + String(cardSize) + "MB";
        Serial.println(str);

        if (!SD.exists(SDCARD_ROOT_PATH))
        {
            SD.mkdir(SDCARD_ROOT_PATH);
        }

        this->checkFiles();
    }
    Serial.println("===========================");
}

void SDCard::tick()
{
    this->processCurrentRide();
    this->processSendRide();
    
    // this->setPointsToSendFromFile();
    // this->setStatsToSendFromFile();
    // this->_TSProperties->PropertiesCompletedRideToSend.IsReady = true;
    // 
    // Serial.println("SDCard");
    // Serial.println("Writing to test.txt...");
    // File file = SD.open("/test.txt", FILE_WRITE);
    // if (file)
    // {
    //     file.println("Testing 1, 2, 3.");
    //     file.close();
    //     Serial.println("done.");
    // }
    // else
    // {
    //     Serial.println("error opening test.txt");
    // }
    // Serial.println("Reading from test.txt...");
    // file = SD.open("/test.txt");
    // if (file)
    // {
    //     while (file.available())
    //     {
    //         Serial.write(file.read());
    //     }
    //     file.close();
    // }
    // else
    // {
    //     Serial.println("error opening test.txt");
    // }
};

void SDCard::checkFiles()
{
    this->_nbRidesInSDCard = 0;
    File root = SD.open(SDCARD_ROOT_PATH);

    String id;
    String statsType = String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION);
    String pointsType = String(SDCARD_FILE_POINTS_NAME) + String(SDCARD_FILE_EXTENSION);

    while (File file = root.openNextFile())
    {
        String name = file.name();
        if (name.length() > 36)
        {
            String idTemp = name.substring(0, 36);
            
            if (!this->_queueCompletedRideIds.contains(idTemp))
            {
                Serial.println("SDCard Ride Id find: " + idTemp);
                this->_queueCompletedRideIds.enqueue(idTemp);
                this->_nbRidesInSDCard++;
            }

            Serial.println("SDCard nb of rides: " + String(this->_nbRidesInSDCard));
            file.close();
        }
    }
    root.close();
};

void SDCard::createRideFiles()
{
    this->_currentStatsFileName = 
        String(SDCARD_ROOT_PATH) 
        + "/" 
        + this->_TSProperties->PropertiesCurrentRide.CompletedRideId 
        + SDCARD_FILE_STATS_NAME 
        + SDCARD_FILE_EXTENSION;
    this->_currentPointsFileName = 
        String(SDCARD_ROOT_PATH) 
        + "/" 
        + this->_TSProperties->PropertiesCurrentRide.CompletedRideId 
        + SDCARD_FILE_POINTS_NAME 
        + SDCARD_FILE_EXTENSION;

    File f = SD.open(this->_currentStatsFileName, FILE_READ);
    f.close();
    f = SD.open(this->_currentPointsFileName, FILE_READ);
    f.close();
};

void SDCard::processCurrentRide()
{
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && !this->_isRideStarted)
    {
        this->_isRideStarted = true;

        this->createRideFiles();

        this->_currentPointsFile = SD.open(this->_currentPointsFileName, FILE_WRITE);        
    }
    else if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted 
                && this->_TSProperties->PropertiesCurrentRide.IsPointReadyToSave)
    {
        this->writePoint();
    }
    else if (this->_TSProperties->PropertiesCurrentRide.IsRideFinished && this->_isRideStarted)
    {
        this->_isRideStarted = false;

        if (this->_TSProperties->PropertiesCurrentRide.IsPointReadyToSave)
        {
            this->writePoint();
        }

        this->writeStatsFile();
        this->_currentPointsFile.close();
        Serial.println("SDCard Ride Id add into the queue: " + this->_TSProperties->PropertiesCurrentRide.CompletedRideId);
        this->_queueCompletedRideIds.enqueue(this->_TSProperties->PropertiesCurrentRide.CompletedRideId);
        this->_TSProperties->PropertiesCurrentRide.IsRideFinished = false;
    }
};

void SDCard::writeStatsFile()
{
    String content = 
        this->_TSProperties->PropertiesCurrentRide.CompletedRideId + ";" +
        this->_TSProperties->PropertiesCurrentRide.PlannedRideId + ";" +
        this->_TSProperties->PropertiesCurrentRide.MaxSpeed + ";" +
        this->_TSProperties->PropertiesCurrentRide.AvgSpeed + ";" +
        this->_TSProperties->PropertiesCurrentRide.DateBegin + ";" +
        this->_TSProperties->PropertiesCurrentRide.DateEnd + ";" +
        this->_TSProperties->PropertiesCurrentRide.DurationS + ";" +
        this->_TSProperties->PropertiesCurrentRide.Distance + ";" +
        this->_TSProperties->PropertiesCurrentRide.NbPoints + ";" +
        this->_TSProperties->PropertiesCurrentRide.NbFalls + ";";

    File file = SD.open(this->_currentStatsFileName, FILE_WRITE);

    file.println(content);
    file.close();
};

void SDCard::writePoint()
{
    this->_currentPointsFile.println(this->_TSProperties->PropertiesCurrentRide.CurrentPoint);
    this->_TSProperties->PropertiesCurrentRide.IsPointReadyToSave = false;
};

void SDCard::setStatsToSend()
{
    int compt = 0;
    File stats = SD.open(String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) 
                            + String(SDCARD_FILE_STATS_NAME) 
                            + String(SDCARD_FILE_EXTENSION)
                            , FILE_READ);
    String content = "";

    while (stats.available())
    {
        String line = stats.readStringUntil(';');
        content += line;
        content += ";";
        compt++;

        if (compt == 8)
        {
            this->_TSProperties->PropertiesCompletedRideToSend.NbPoints = line.toInt();
        }
    }

    stats.close();

    Serial.println("SDCard Stats find to send: " + content);

    // String content = 
    //     this->_TSProperties->PropertiesCurrentRide.CompletedRideId + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.PlannedRideId + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.MaxSpeed + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.AvgSpeed + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.DateBegin + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.DateEnd + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.DurationS + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.Distance + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.NbPoints + ";" +
    //     this->_TSProperties->PropertiesCurrentRide.NbFalls + ";";

    this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId 
        = this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId;
    this->_TSProperties->PropertiesCompletedRideToSend.Stats = content;
    // this->_TSProperties->PropertiesCompletedRideToSend.NbPoints 
    //     = this->_TSProperties->PropertiesCurrentRide.NbPoints;
};

void SDCard::setPointsToSendFromFile()
{
    if (this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber == 0)
    {
        String fileName = 
            String(SDCARD_ROOT_PATH) 
            + "/" 
            + this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId 
            + SDCARD_FILE_POINTS_NAME 
            + SDCARD_FILE_EXTENSION;

        this->_currentFileSendPoints = SD.open(this->_currentPointsFileName, FILE_READ);
        this->_positionCursorFileSendPoints = 0;
        Serial.println("SDCard Points file opened: " + fileName);
    }

    
    if (!this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady
                && this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber
                    < this->_TSProperties->PropertiesCompletedRideToSend.NbPoints)
    {
        ++this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber;

        this->_currentFileSendPoints.seek(this->_positionCursorFileSendPoints);
        String point = this->_currentFileSendPoints.readStringUntil('\n');
        this->_positionCursorFileSendPoints = this->_currentFileSendPoints.position();

        this->_TSProperties->PropertiesCompletedRideToSend.Point = point;
        this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = true;
        this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = false;
        Serial.println("SDCard Points ready to send: " + String(this->_TSProperties->PropertiesCompletedRideToSend.Point));
    }
    // else if (this->_TSProperties->PropertiesCompletedRideToSend.IsReceived)
    // {
    //     this->_currentFileSendPoints.close();
    //     this->_isSendingPoints = false;
    //     this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
    //     this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = false;
    // }
};

void SDCard::processSendRide()
{
    if (this->_queueCompletedRideIds.getSize() > 0)
    {
        if (!this->_TSProperties->PropertiesCompletedRideToSend.IsReady)
        {
            this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId = this->_queueCompletedRideIds.dequeue();
            Serial.println("SDCard Ride Id find to send: " + this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId);

            this->setStatsToSend();
            Serial.println("SDCard stats ready to send !");

            this->_isSendingPoints = true;
            this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId 
                = this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId;
            this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber = 0;
            this->_TSProperties->PropertiesCompletedRideToSend.IsReady = true;
            this->_TSProperties->PropertiesCompletedRideToSend.IsReceived  = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived  = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady  = false;
        }
        else if (this->_isSendingPoints)
        {
            this->setPointsToSendFromFile();
        }
        else if (this->_TSProperties->PropertiesCompletedRideToSend.IsReceived)
        {
            this->_currentFileSendPoints.close();
            this->deleteCurrentRideFiles();
            this->_isSendingPoints = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = false;
        }
    }
};

void SDCard::deleteCurrentRideFiles()
{
    SD.remove(String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) 
                            + String(SDCARD_FILE_STATS_NAME) 
                            + String(SDCARD_FILE_EXTENSION));
    Serial.println(String("Completed Ride Stats file deleted") + String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId));
    SD.remove(String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) 
                            + String(SDCARD_FILE_POINTS_NAME) 
                            + String(SDCARD_FILE_EXTENSION));
    Serial.println(String("Completed Ride Points file deleted") + String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId));
};

