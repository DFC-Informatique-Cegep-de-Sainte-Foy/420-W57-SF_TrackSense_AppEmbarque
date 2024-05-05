#include "Modules/SDCard.h"
#include "Configurations.h"
#include "StringQueue.h"

SDCard::SDCard(TSProperties *TSProperties, StringQueue *trajetsSD)
    : _TSProperties(TSProperties),
      _queueCompletedRideIds(trajetsSD),
      _nbRidesInSDCard(0),
      _isRideStarted(false),
      _currentPointsFile(),
      _currentPointsFileName(""),
      _currentStatsFileName(""),
      _currentFileSendPoints(),
      _positionCursorFileSendPoints(0),
      _isSendingRide(false),
      _isSendingPoints(false)

{
    this->init();
};

SDCard::~SDCard(){};

void SDCard::init()
{
    DEBUG_STRING_LN(DEBUG_TS_SDCARD, "========SDCard Detect.======");
    SPI.begin(PIN_SDCARD_SCLK, PIN_SDCARD_MISO, PIN_SDCARD_MOSI);

    this->_TSProperties->PropertiesSDCard.IsSDCardConnected = SD.begin(PIN_SDCARD_CS);

    if (!this->_TSProperties->PropertiesSDCard.IsSDCardConnected)
    {
        DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard MOUNT FAIL");
        Serial.println("SDCard MOUNT FAIL");
    }
    else
    {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.println("SDCard Size: " + String(cardSize) + "MB");
        // String str = "SDCard Size: " + String(cardSize) + "MB";
        DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Size: " + String(cardSize) + "MB");

        if (!SD.exists(SDCARD_ROOT_PATH))
        {
            SD.mkdir(SDCARD_ROOT_PATH);
        }

        this->checkFiles();
    }
    DEBUG_STRING_LN(DEBUG_TS_SDCARD, "===========================");
}

void SDCard::tick()
{
    // Serial.println("7---SDCard --> tick");

    if (this->_TSProperties->PropertiesSDCard.IsSDCardConnected)
    {
        this->processCurrentRide();
        this->processSendRide();
    }
};

void SDCard::SaveTrajet(String p_path, Trajet *p_trajet)
{
    // Creer path si n'existe pas
    if (!SD.exists(p_path))
    {
        SDCard::creerDir(p_path);
    }

    String FileName = p_path + "/" + String(p_trajet->ride_id) + ".txt";
    File file = SD.open(FileName, FILE_WRITE);
    // write in file.txt
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    String strJson = p_trajet->fromTrajet2JsonStr();

    if (file.print(strJson))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}

Trajet SDCard::ReadTrajet(String p_path, String p_fileName)
{
    String FileName = p_path + "/" + p_fileName + ".txt";
    File file = SD.open(FileName, FILE_READ);

    if (!file)
    {
        Serial.println("Impossible d'ouvrir le fichier");
    }
    String jsonStr;
    while (file.available())
    {
        jsonStr += (char)file.read();
    }
    file.close();

    Trajet trajet = Trajet::fromJsonStr2Trajet(jsonStr);
    return trajet;
}

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

            if (!this->_queueCompletedRideIds->contains(idTemp))
            {
                DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Ride Id find: " + idTemp);

                this->_queueCompletedRideIds->enqueue(idTemp);
                this->_nbRidesInSDCard++;
            }

            DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard nb of rides: " + String(this->_nbRidesInSDCard));
            file.close();
        }
    }
    root.close();
};

void SDCard::createRideFiles()
{
    this->_currentStatsFileName =
        String(SDCARD_ROOT_PATH) + "/" + this->_TSProperties->PropertiesCurrentRide.CompletedRideId + SDCARD_FILE_STATS_NAME + SDCARD_FILE_EXTENSION;
    this->_currentPointsFileName =
        String(SDCARD_ROOT_PATH) + "/" + this->_TSProperties->PropertiesCurrentRide.CompletedRideId + SDCARD_FILE_POINTS_NAME + SDCARD_FILE_EXTENSION;

    File f = SD.open(this->_currentStatsFileName, FILE_READ);
    f.close();
    f = SD.open(this->_currentPointsFileName, FILE_READ);
    f.close();
    Serial.println("Ride File cree");
};

void SDCard::writeFile()
{
    String path = "/hello.txt";

    Serial.printf("Writing file: %s\n", path);

    File file = SD.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print("Hello"))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}
void SDCard::creerDir(String p_dir)
{
    Serial.printf("Creating Dir: %s\n", p_dir);
    if (SD.mkdir(p_dir))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}
void SDCard::processCurrentRide()
{
    if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && !this->_isRideStarted)
    {
        this->_isRideStarted = true;

        this->createRideFiles();

        this->_currentPointsFile = SD.open(this->_currentPointsFileName, FILE_WRITE);
    }
    else if (this->_TSProperties->PropertiesCurrentRide.IsRideStarted && this->_TSProperties->PropertiesCurrentRide.IsPointReadyToSave)
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

        if (this->_TSProperties->PropertiesCurrentRide.NbPoints > 0)
        {
            this->writeStatsFile();
            this->_currentPointsFile.close();
            DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Ride Id add into the queue: " + this->_TSProperties->PropertiesCurrentRide.CompletedRideId);
            this->_queueCompletedRideIds->enqueue(this->_TSProperties->PropertiesCurrentRide.CompletedRideId);
        }
        else
        {
            this->_currentPointsFile.close();
            this->deleteCurrentRideFiles();
        }

        this->_TSProperties->PropertiesCurrentRide.IsRideFinished = false;
    }
};

void SDCard::writeStatsFile()
{
    String content =
        this->_TSProperties->PropertiesCurrentRide.CompletedRideId + ";" +
        this->_TSProperties->PropertiesCurrentRide.PlannedRideId + ";" +
        this->_TSProperties->PropertiesCurrentRide.MaxSpeedKMPH + ";" +
        this->_TSProperties->PropertiesCurrentRide.AverageSpeedKMPH + ";" +
        this->_TSProperties->PropertiesCurrentRide.DateBegin + ";" +
        this->_TSProperties->PropertiesCurrentRide.DateEnd + ";" +
        this->_TSProperties->PropertiesCurrentRide.DurationS + ";" +
        this->_TSProperties->PropertiesCurrentRide.DistanceTotalMeters + ";" +
        this->_TSProperties->PropertiesCurrentRide.NbPoints + ";" +
        this->_TSProperties->PropertiesCurrentRide.NbFalls + ";";

    File file = SD.open(this->_currentStatsFileName, FILE_WRITE);

    file.print(content);
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

    File root = SD.open(SDCARD_ROOT_PATH);

    this->_currentPointsFileName =
        this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId + SDCARD_FILE_STATS_NAME + SDCARD_FILE_EXTENSION;

    File stats;

    while (File file = root.openNextFile())
    {
        String name = file.name();
        if (name == this->_currentPointsFileName)
        {
            stats = file;
            break;
        }
    }

    root.close();

    String content = "";

    while (stats.available())
    {
        String line = stats.readStringUntil(';');
        content += line;
        content += ";";
        compt++;

        if (compt == SDCARD_POSITION_NUMBER_OF_POINTS)
        {
            DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Number of points : " + line);
            this->_TSProperties->PropertiesCompletedRideToSend.NbPoints = line.toInt();
        }
    }

    stats.close();

    DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Stats find to send: " + content);

    this->_TSProperties->PropertiesCompletedRideToSend.Stats = content;
};

void SDCard::setPointsToSendFromFile()
{
    if (this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber == 0 && !this->_currentFileSendPoints)
    {
        File root = SD.open(SDCARD_ROOT_PATH);

        this->_currentPointsFileName =
            this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId + SDCARD_FILE_POINTS_NAME + SDCARD_FILE_EXTENSION;

        while (File file = root.openNextFile())
        {
            String name = file.name();
            if (name == this->_currentPointsFileName)
            {
                this->_currentFileSendPoints = file;
                break;
            }
        }

        root.close();

        this->_positionCursorFileSendPoints = 0;
    }

    if (!this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady && this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber < this->_TSProperties->PropertiesCompletedRideToSend.NbPoints)
    {
        String point = "";

        this->_currentFileSendPoints.seek(this->_positionCursorFileSendPoints);

        String element;

        element = this->_currentFileSendPoints.readStringUntil(';');
        this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber = element.toInt();

        point += element;
        point += ";";
        element = this->_currentFileSendPoints.readStringUntil('\n');
        point += element;

        this->_positionCursorFileSendPoints = this->_currentFileSendPoints.position();

        this->_TSProperties->PropertiesCompletedRideToSend.Point = point;
        this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = true;
        this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = false;
        DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Points ready to send: " + String(this->_TSProperties->PropertiesCompletedRideToSend.Point));
    }
};

void SDCard::processSendRide()
{
    if (this->_TSProperties->PropertiesBluetooth.IsDeviceBluetoothConnected && (this->_queueCompletedRideIds->getSize() > 0 || this->_isSendingRide))
    {
        if (this->_TSProperties->PropertiesCompletedRideToSend.IsReceived)
        {
            DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard begin to delete files");
            this->_currentFileSendPoints.close();
            this->deleteCurrentRideSentFiles();
            this->_isSendingPoints = false;
            this->_isSendingRide = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsStatsReceived = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsReceived = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsReady = false;
        }
        else if (!this->_TSProperties->PropertiesCompletedRideToSend.IsReady)
        {
            this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId = this->_queueCompletedRideIds->dequeue();
            DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard Ride Id find to send: " + this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId);
            this->setStatsToSend();
            DEBUG_STRING_LN(DEBUG_TS_SDCARD, "SDCard stats ready to send !");

            this->_isSendingPoints = true;
            this->_isSendingRide = true;
            this->_TSProperties->PropertiesCompletedRideToSend.CurrentPointNumber = 0;
            this->_TSProperties->PropertiesCompletedRideToSend.IsReady = true;
            this->_TSProperties->PropertiesCompletedRideToSend.IsReceived = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReceived = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsPointReady = false;
            this->_TSProperties->PropertiesCompletedRideToSend.IsStatsReceived = false;
        }
        else if (this->_TSProperties->PropertiesCompletedRideToSend.IsReady && this->_TSProperties->PropertiesCompletedRideToSend.IsStatsReceived && this->_isSendingPoints)
        {
            this->setPointsToSendFromFile();
        }
    }
};

void SDCard::deleteCurrentRideFiles()
{
    SD.remove((
                  SDCARD_ROOT_PATH + String("/") + this->_TSProperties->PropertiesCurrentRide.CompletedRideId) +
              String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION));
    DEBUG_STRING_LN(DEBUG_TS_SDCARD, String("Completed Ride Stats file deleted: ") + String(this->_TSProperties->PropertiesCurrentRide.CompletedRideId) + String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION));

    SD.remove(
        SDCARD_ROOT_PATH + String("/") + String(this->_TSProperties->PropertiesCurrentRide.CompletedRideId) + String(SDCARD_FILE_POINTS_NAME) + String(SDCARD_FILE_EXTENSION));
    DEBUG_STRING_LN(DEBUG_TS_SDCARD, String("Completed Ride Points file deleted: ") + String(this->_TSProperties->PropertiesCurrentRide.CompletedRideId) + String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION));
};

void SDCard::deleteCurrentRideSentFiles()
{
    SD.remove((
                  SDCARD_ROOT_PATH + String("/") + this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) +
              String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION));
    DEBUG_STRING_LN(DEBUG_TS_SDCARD, String("Completed Ride Stats file deleted: ") + String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) + String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION));

    SD.remove(
        SDCARD_ROOT_PATH + String("/") + String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) + String(SDCARD_FILE_POINTS_NAME) + String(SDCARD_FILE_EXTENSION));
    DEBUG_STRING_LN(DEBUG_TS_SDCARD, String("Completed Ride Points file deleted: ") + String(this->_TSProperties->PropertiesCompletedRideToSend.CompletedRideId) + String(SDCARD_FILE_STATS_NAME) + String(SDCARD_FILE_EXTENSION));
};
