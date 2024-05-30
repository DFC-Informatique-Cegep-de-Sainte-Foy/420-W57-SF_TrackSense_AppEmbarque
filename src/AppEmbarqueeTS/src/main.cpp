// #define Address_detect
#define TrackSense
// #define Test_Trajet
// #define SD_Card
// #define WATCH

#ifdef Address_detect
#include <Adafruit_I2CDevice.h>

Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(0x15);
// CST816S 0x15
// mpu6050 0X65

void setup()
{
    // while (!Serial)
    // {
    //     delay(10);
    // }
    Serial.begin(115200);
    Serial.println("I2C address detection test");

    if (!i2c_dev.begin())
    {
        Serial.print("Did not find device at 0x");
        Serial.println(i2c_dev.address(), HEX);
        while (1)
            ;
    }
    Serial.print("Device found on address 0x");
    Serial.println(i2c_dev.address(), HEX);
}

void loop()
{
}
#endif

#ifdef TrackSense

#include <Arduino.h>
#include "Program.h"
#include <Modules/CST816S.h>

void loopCore0(void *pvParameters); // forward declaration of the loopCore0 function
Program *program = nullptr;

// CST816S touch(21, 22, 33, 34); // sda(21), scl(22), rst(33), irq(34)  works

void setup()
{
    Serial.end();
    Serial.begin(9600);
    // Test touch
    // touch.begin();
    program = new Program();

    //???
    ledcSetup(LEDC_CHANNEL_0, LEDC_FREQ, LEDC_RESOLUTION); // configure LED PWM functionalitites
    ledcAttachPin(PIN_LED, LEDC_CHANNEL_0);                // attach the channel to the GPIO to be controlled

    xTaskCreatePinnedToCore(
        loopCore0,
        "loopCore0", // Name of the task
        5000,        // Stack size in words // À vérifier !!!
        NULL,        // Task input parameter
        1,           // Priority of the task
        NULL,        // Task handle.
        0            // Core where the task should run
    );

#if DEBUG_TS_CORE
    Serial.println("Debug mode");

    Serial.printf("Flash Speed = %d Flash mode = %d", ESP.getFlashChipSpeed(), (int)ESP.getFlashChipMode());
    Serial.println();

    Serial.printf("Chip Cores = %d", ESP.getChipCores());
    Serial.println();

    Serial.printf("Chip Model = %s", ESP.getChipModel());
    Serial.println();

    Serial.printf("Chip Revision = %d", ESP.getChipRevision());
    Serial.println();

    Serial.printf("CPU Frequency = %d", ESP.getCpuFreqMHz());
    Serial.println();

    Serial.printf("Heap Size = %d", ESP.getHeapSize());
    Serial.println();

    Serial.printf("Free Heap = %d", ESP.getFreeHeap());
    Serial.println();

    Serial.printf("Sketch Size = %d", ESP.getSketchSize());
    Serial.println();

    Serial.printf("Free Sketch Space = %d", ESP.getFreeSketchSpace());
    Serial.println();

    Serial.printf("Flash Chip Size = %d", ESP.getFlashChipSize());
    Serial.println();

    Serial.printf("Cycle Count = %d", ESP.getCycleCount());
    Serial.println();

    Serial.printf("PSRAM Size = %d", ESP.getPsramSize());
    Serial.println();

    Serial.printf("Max Alloc Heap = %d", ESP.getMaxAllocHeap());
    Serial.println();

    Serial.printf("Max Alloc PSRAM = %d", ESP.getMaxAllocPsram());
    Serial.println();

    Serial.printf("SDK Version = %s", ESP.getSdkVersion());
    Serial.println();

    Serial.printf("Sketch Size = %d", ESP.getSketchSize());
    Serial.println();

    ESP.magicFlashChipMode(0);
    delay(1000);
    Serial.printf("Flash Speed = %d Flash mode = %d", ESP.getFlashChipSpeed(), ESP.getFlashChipMode());
    Serial.println();

    // int optimumStackSize = (5000 - uxTaskGetStackHighWaterMark(NULL)) + 2000;
    // Serial.printf("Optimum Stack Size = %d", optimumStackSize);
    // Serial.println();
#endif
}

void loop()
{
    program->executeCore1();
    // test touch
    // if (touch.available())
    // {
    //     Serial.print(touch.gesture());
    //     Serial.print("\t");
    //     Serial.print(touch.data.points);
    //     Serial.print("\t");
    //     Serial.print(touch.data.event);
    //     Serial.print("\t");
    //     Serial.print(touch.data.x);
    //     Serial.print("\t");
    //     Serial.println(touch.data.y);
    // }
}

void loopCore0(void *pvParameters)
{
    while (true)
    {
#if DEBUG_TS_CORE == 1
        Serial.printf("Stack High Water Mark = %d.......", uxTaskGetStackHighWaterMark(NULL));
        Serial.println();
        int optimumStackSize = (5000 - uxTaskGetStackHighWaterMark(NULL)) + 2000;
        Serial.printf("Optimum Stack Size = %d.......", optimumStackSize);
        Serial.println();
#endif
        program->executeCore0();

        /*
            REALLY IMPORTANT :
            Some Arduino low code is running on core 0 by default. If a Task is running on core 0 without pause,
            it take 100% of calculation time and let no time for Arduino low code to run. The Application will crash at random moment...
            So, we need to pause the task to let some time for Arduino low code to run.
            vTaskDelay(10 / portTICK_PERIOD_MS) is the minimum value to let Arduino low code to run.
        */
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

#endif

#ifdef Test_Trajet

#include <Arduino.h>
#include <Location.h>
#include <Trajet.h>
#include <modules/SDCard.h>
#include "TSProperties.h"
#include "StringQueue.h"
Trajet *t1;
std::vector<Location> *points = new std::vector<Location>();
std::vector<Location> *pointsInteret = new std::vector<Location>();
std::vector<Location> *pointsDanger = new std::vector<Location>();
SDCard *mySD;
TSProperties *ts;
StringQueue *queue;
void setup()
{
    Serial.begin(115200);
    ts = new TSProperties();
    queue = new StringQueue();
    mySD = new SDCard(ts, queue);

    t1 = new Trajet("5515", "to the North", "2024-05-04", "", 4888.38, 70.49, 54678.123, points, pointsInteret, pointsDanger);
    t1->points->push_back(Location(1.23, 2.34));
    t1->points->push_back(Location(9.87, 8.76));
    t1->pointsdInteret->push_back(Location(6.66, 8.88));
    t1->pointsdDanger->push_back(Location(3.33, 3.14));
    Serial.println("Hello");
    Serial.println(t1->nom);            // checked!
    String str = t1->fromTrajet2Json(); // checked!
    Serial.println(str);                // JSON obj
    /*
        {"ride_id":"5515","nom":"to the North","distance":4888.38,"vitesse_moyenne":70.49,
        "dateBegin":"2024-05-04","dateEnd":"","duration":54678,"estComplete":false,
        "estReadyToSave":false,"points":[{"latitude":1.23,"longitude":2.34},{"latitude":9.87,"longitude":8.76}],
        "pointsdInteret":[{"latitude":6.66,"longitude":8.88}],
        "pointsdDanger":[{"latitude":3.33,"longitude":3.14}]}
    */

    String jsonStr = "{\"ride_id\":\"5515\",\"nom\":\"to the North\",\"distance\":4888.38,\"vitesse_moyenne\":70.49,\"dateBegin\":\"2024-05-04\",\"dateEnd\":\"\",\"duration\":54678,\"estComplete\":false,\"estReadyToSave\":false,\"points\":[{\"latitude\":1.23,\"longitude\":2.34},{\"latitude\":9.87,\"longitude\":8.76}],\"pointsdInteret\":[{\"latitude\":6.66,\"longitude\":8.88}],\"pointsdDanger\":[{\"latitude\":3.33,\"longitude\":3.14}]}";

    Trajet t2 = Trajet::fromJson2Trajet(jsonStr);
    Serial.println(String(t2.ride_id));                          // checked!
    Serial.println(String(t2.nom));                              // checked!
    Serial.println(String(t2.points->front().latitude));         // checked!
    Serial.println(String(t2.pointsdInteret->front().latitude)); // checked!
    Serial.println(String(t2.pointsdDanger->front().latitude));  // checked!

    mySD->SaveTrajet("/complete", jsonStr);                                          // checked!
    Serial.println("Trajet saved with id ->" + String(t2.ride_id));                  // checked!
    Trajet trajet = mySD->ReadTrajet("/complete", String(5515));                     // checked!
    Serial.println("Read Trajet from SD with id 5515 ->" + String(trajet.duration)); // checked!
}
void loop()
{
}

#endif

#ifdef SD_Card
#define LED_YELLOW 2
#define LED_BUILTIN 1
#define RXD0 12
#define TXD0 14

#include "FS.h"
#include "SD.h"
#include "SPI.h"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);
void setup()
{
    Serial.begin(115200, SERIAL_8N1, RXD0, TXD0);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW); // active LOW
    digitalWrite(LED_YELLOW, HIGH); // active LOW

    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    digitalWrite(LED_YELLOW, LOW);   // active LOW
    digitalWrite(LED_BUILTIN, HIGH); // active LOW
}

void loop()
{
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path)
{
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path))
    {
        Serial.println("Dir removed");
    }
    else
    {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        Serial.println("File renamed");
    }
    else
    {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path)
{
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else
    {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char *path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
#endif

#ifdef WATCH
// #include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "Configurations.h"

#define TFT_GREY 0x5AEB

Adafruit_GC9A01A *tft = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
GFXcanvas16 *canvas = new GFXcanvas16(TFT_WIDTH, TFT_HEIGHT);

uint32_t targetTime = 0; // for next 1 second timeout

static uint8_t conv2d(const char *p); // Forward declaration needed for IDE 1.6.x

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

void setup(void)
{
    Serial.begin(115200);
    tft->begin(80000000);
    canvas->setTextWrap(false);
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);
    canvas->setRotation(1);
    targetTime = millis() + 1000;
    tft->fillScreen(GC9A01A_BLACK);
}

void loop()
{
    if (targetTime < millis())
    {
        // Set next update for 1 second later
        targetTime = millis() + 1000;

        // Adjust the time values by adding 1 second
        ss++; // Advance second
        if (ss == 60)
        {             // Check for roll-over
            ss = 0;   // Reset seconds to zero
            omm = mm; // Save last minute time for display update
            mm++;     // Advance minute
            if (mm > 59)
            { // Check for roll-over
                mm = 0;
                hh++; // Advance hour
                if (hh > 23)
                {           // Check for 24hr roll-over (could roll-over on 13)
                    hh = 0; // 0 for 24 hour clock, set to 1 for 12 hour clock
                }
            }
        }

        // Update digital time
        tft->setTextSize(8);
        if (omm != mm)
        {
            tft->fillScreen(GC9A01A_BLACK);
            // Redraw hours and minutes time every minute
            omm = mm;
            // Draw hours and minutes
            if (hh < 10)
            {
                tft->setCursor(10, 90);
                tft->print("0"); // Add hours leading zero for 24 hr clock
                tft->setCursor(58, 90);
                tft->print(hh); // Draw hours
            }
            else
            {
                tft->setCursor(10, 90);
                tft->print(hh); // Draw hours
            }
            tft->setCursor(100, 90);
            tft->setTextColor(GC9A01A_WHITE);
            tft->print(":");
            if (mm < 10)
            {
                tft->setCursor(140, 90);
                tft->print("0"); // Add minutes leading zero
                tft->setCursor(188, 90);
                tft->print(mm); // Draw minutes
            }
            else
            {
                tft->setCursor(140, 90);
                tft->print(mm); // Draw minutes
            }
        }
    }
}
// Function to extract numbers from compile time string
static uint8_t conv2d(const char *p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}
#endif