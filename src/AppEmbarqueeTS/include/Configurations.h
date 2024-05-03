#pragma once
#include <Arduino.h>

/*----- DEBUG -----*/
#ifndef DEBUG
#define DEBUG false
#define DEBUG_STRING(Debug_Type, Valeur)
#define DEBUG_STRING_LN(Debug_Type, Valeur)
#else
#define DEBUG_STRING(Debug_Type, Valeur) \
    if (Debug_Type)                      \
    {                                    \
        Serial.print(Valeur);            \
    };
#define DEBUG_STRING_LN(Debug_Type, Valeur) \
    if (Debug_Type)                         \
    {                                       \
        Serial.println(Valeur);             \
    };
#endif

// Debug_Type : true = affiche les messages de debug, false = n'affiche pas les messages de debug
#define DEBUG_TS_BUTTONS DEBUG && false
#define DEBUG_TS_BUTTONS_HARDCORE DEBUG && false
#define DEBUG_TS_SCREEN DEBUG && true
#define DEBUG_TS_GSM DEBUG && true
#define DEBUG_TS_GPS DEBUG && true
#define DEBUG_TS_GPS_HARDCORE DEBUG && true
#define DEBUG_TS_SDCARD DEBUG && true
#define DEBUG_TS_BLE DEBUG && false
#define DEBUG_TS_BATTERY DEBUG && false
#define DEBUG_TS_BUZZER DEBUG && false
#define DEBUG_TS_ACCELEROMETER DEBUG && false
#define DEBUG_TS_COMPASS DEBUG && false
#define DEBUG_TS_GYROSCOPE DEBUG && false
#define DEBUG_TS_CORE DEBUG && false

/*----- Screen -----*/
#define SPI_TFCard 0 // 1 = SPI, 0 = VSPI // ALWAYS USE VSPI, NEVER USE SPI. Our connections are bad on our V1.0 PCB and we can't use SPI_TFCard.

#if SPI_TFCard
// Ne pas considérer cette possibilité de connexion de l'écran. Aucune chance que ça fonctionne. Le PCB peut être modifié à l'avenir.
#define TFT_BLK -1  // LED back-light
#define TFT_CS 12   // 25       // Chip select control pin
#define TFT_DC 25   // 02          // Data Command control pin
#define TFT_RST -1  // Reset pin (could connect to Arduino RESET pin)
#define TFT_MOSI 15 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_CLK 14  // In some display driver board, it might be written as "SCL" and so on.
#define TFT_MISO 2  // Ne semble pas être utile, car non tactile

#else               // VSPI
// ATTENTION : La PIN 25 TFT_BLK semble utilisée par le GPS selon le site officiel Lilygo. Mais elle ne semble pas utilisée par le GPS dans notre cas... Sert pour le mode veille !
#define TFT_BLK -1  // LED back-light. "BLK" or "BLK"
#define TFT_CS 5    // Chip select control pin. "CS" or "SS"
#define TFT_DC 0    // Data Command control pin
#define TFT_RST -1  // Reset pin (could connect to Arduino RESET pin) "RST" or "RST"
#define TFT_MOSI 23 // In some display driver board, it might be written as "SDA" and so on. "DIN" or "MOSI
#define TFT_CLK 18  // In some display driver board, it might be written as "SCL" and so on. "CLK" or "SCK"
#define TFT_MISO -1 // Ne semble pas être utile, car non tactile

#endif

#define TFT_WIDTH 240
#define TFT_HEIGHT 240
#define TFT_LIGHT_MODE_BACKGROUND_COLOR GC9A01A_WHITE
#define TFT_LIGHT_MODE_TEXT_COLOR GC9A01A_BLACK
#define TFT_DARK_MODE_BACKGROUND_COLOR GC9A01A_BLACK
#define TFT_DARK_MODE_TEXT_COLOR GC9A01A_WHITE

/*----- Page ID -----*/
#define NB_ACTIVE_PAGES 7 // 3 == Home Page, Ride Page, Ride Statistics Page
/*
    0 : Home Page
    1 : Ride Page
    2 : Ride Statistics Page
    3 : Compass Page
    4 : Ride Direction Page
    5 : Global Statistics Page
    6 : Go Home Page
    -1 : Init TS Page
    -2 : Ending Ride Page
    -3 : No Page (error)

    0 : Page Accueil
    1 : Page Trajet
    2 : Page Statistiques Trajet
    3 : Page Boussole
    4 : Page Direction Trajet
    5 : Page Statistiques Globales
    6 : Page Retour Maison
    -1 : Page Init TS
    -2 : Page Fin Trajet
    -3 : Page Erreur
*/
#define HOME_PAGE_ID 0
#define RIDE_PAGE_ID 1
#define RIDE_STATISTICS_PAGE_ID 2
#define COMPASS_PAGE_ID 3
#define RIDE_DIRECTION_PAGE_ID 4
#define GLOBAL_STATISTICS_PAGE_ID 5
#define GO_HOME_PAGE_ID 6
#define INIT_TS_PAGE_ID -1
#define ENDING_RIDE_PAGE_ID -2
#define ERROR_PAGE_ID -3

/*----- ESP32 -----*/
#define PIN_LED 12
// setting PWM properties
#define LEDC_FREQ 5000
#define LEDC_CHANNEL_0 0
#define LEDC_RESOLUTION 8

/*----- SD Card -----*/
#define PIN_SDCARD_MOSI 15
#define PIN_SDCARD_SCLK 14
#define PIN_SDCARD_CS 13
#define PIN_SDCARD_MISO 2

#define SDCARD_ROOT_PATH "/cr"
#define SDCARD_FILE_EXTENSION ".csv"
#define SDCARD_FILE_STATS_NAME "_stats"
#define SDCARD_FILE_POINTS_NAME "_points"
#define SDCARD_POSITION_NUMBER_OF_POINTS 9
#define SDCARD_NUMBER_ELEMENTS_OF_POINT 8

/*----- GSM LilyGO T-SIM7000G (GPS, LTE) -----*/
// Exemple point
// 1;0.0000000000;0.0000000000;0.0000000000;00.00;00.00;0000/00/00-00:00:00;00:00:00

// Exemple Stats
// 00000000-0000-0000-0000-000000000000;00000000-0000-0000-0000-000000000000;00.00;00.00;0000/00/00-00:00:00;0000/00/00-00:00:00;00:00:00;000.0;5;0

#define SerialAT Serial1
#define GPS_UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4
#define TINY_GSM_MODEM_SIM7000 // Active le bon processeur
// #define TINY_GSM_MODEM_SIM7600  // Active le bon processeur
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define MINIMUM_DISTANCE_TO_ADD_POSITION 10
#define MINIMUM_DISTANCE 5

#define SMS_TARGET "+15819785858" // TrackSense send sms to this telephone
// #define SMS_TARGET "+15819785858" // TrackSense send sms to this telephone
// #define SMS_TARGET "+15813070531" // TrackSense send sms to this telephone

/*----- Serial -----*/
#define SerialMon Serial // HardwareSerial Serial(0);
// Set serial for AT commands
#define SerialAT Serial1 // HardwareSerial Serial1(1);

/*----- I2C : Accelerometer, Gyroscope, Compass, Temperature -----*/
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_CST816_RST 33
#define PIN_CST816_INT 34

/*----- Buzzer -----*/
#define PIN_BUZZER -1

/*----- Battery -----*/
// How to calculate Voltage : https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/issues/104#issuecomment-833887214
#define PIN_BATTERY 35
#define BATTERY_VOLTAGE_MIN 3.0
#define BATTERY_VOLTAGE_MAX 4.2
#define BATTERY_PERCENTAGE_MIN 0
#define BATTERY_PERCENTAGE_MAX 100
#define BATTERY_NB_READS 15 // number of voltage readings before choosing a median, keep this number odd
#define BATTERY_VREF 1100   // ADC reference voltage, change this to 1100 if using ESP32 ADC

/*----- Buttons -----*/
#define PIN_BUTTON1 -1
#define PIN_BUTTON2 -1
#define BUTTON_LONG_PRESS_DURATION_MS 2000
#define BUTTON_INACTIVITY_TIME_MS 300000 // 5 minutes
#define BUTTON_DURATIONDEBOUNCE 200

// #define BUTTON_INACTIVITY_TIME_MS 5000 // 10 secondes

/*----- BLE -----*/
#define BLE_DEVICE_NAME "TrackSense"
#define BLE_PIN_CODE "123456"
#define BLE_TRUE "true"
#define BLE_FALSE "false"
#define BLE_OK "ok"
#define BLE_CONFIRME_STATS 0

#define BLE_DELAY_SEND_STATS_MS 1000
#define BLE_DELAY_SEND_POINT_MS 200

// Service et caracterisiques pour CompletedRide
#define BLE_COMPLETED_RIDE_SERVICE_UUID "62ffab64-3646-4fb9-88d8-541deb961192"

#define BLE_COMPLETED_RIDE_CHARACTRISTIC_DATA "51656aa8-b795-427f-a96c-c4b6c57430dd"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_DATA_NAME "Completed Ride Stats"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_DATA_UUID "5a2b4a0f-8ddd-4c69-a825-dbab5822ba0e"

#define BLE_COMPLETED_RIDE_NOTIFICATION_CHARACTRISTIC "61656aa8-b795-427f-a96c-c4b6c57430dd"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_NOTIF_NAME "Notification"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_NOTIFICATION_UUID "6a2b4a0f-8ddd-4c69-a825-dbab5822ba0e"

#define BLE_SCREEN_SERVICE_UUID "68c50cff-e5ad-4cb8-9541-997d42925f27"
#define BLE_SCREEN_CHARACTRISTIC_ROTATE "65000b05-c1a9-4dfb-a173-bdaa4a029bf6"
#define BLE_SCREEN_DESCRIPTOR_ROTATE_NAME "Rotate screen"
#define BLE_SCREEN_DESCRIPTOR_ROTATE_UUID "65000b05-c1a9-4dfb-a173-bdaa4a029bf7"

#define BLE_LED_SERVICE_UUID "68c50cff-e5ad-4cb8-9541-997d42925g17"
#define BLE_LED_CHARACTRISTIC "65000b05-c1a9-4dfb-a173-bdaa4a029cf6"
#define BLE_LED_DESCRIPTOR "LED SWITCH"
#define BLE_LED_DESCRIPTOR_UUID "65000b05-c1a9-4dfb-a173-bdaa4a029cf7"

/*----- Configuration file -----*/
#define PATH_CONFIGURATION_FILE "/Configuration.json"
#define DYNAMIQUE_JSON_DOCUMENT_SIZE 4096

#define FIELD_SCREEN_ROTATION "Screen_Rotation"
