#pragma once
#include <Arduino.h>

/*----- DEBUG -----*/
#define DEBUG_BUTTONS false
#define DEBUG_GSM false

/*----- Screen -----*/
#define SPI_TFCard 0 // 1 = SPI, 0 = VSPI

#if SPI_TFCard
// Ne pas considérer cette possibilité de connexion de l'écran. Aucune chance que ça fonctionne. Le PCB peut être modifié à l'avenir.
#define TFT_BLK -1  // LED back-light
#define TFT_CS 12   // 25       // Chip select control pin
#define TFT_DC 25   // 02          // Data Command control pin
#define TFT_RST -1  // Reset pin (could connect to Arduino RESET pin)
#define TFT_MOSI 15 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_CLK 14  // In some display driver board, it might be written as "SCL" and so on.
#define TFT_MISO 2  // Ne semble pas être utile, car non tactile

#else // VSPI

#define TFT_BLK -1  // 12 // LED back-light. "BLK" or "BLK"
#define TFT_CS 5    // 25     // Chip select control pin. "CS" or "SS"
#define TFT_DC 0    // 19        // Data Command control pin
#define TFT_RST -1  // Reset pin (could connect to Arduino RESET pin) "RST" or "RST"
#define TFT_MOSI 23 // In some display driver board, it might be written as "SDA" and so on. "DIN" or "MOSI
#define TFT_CLK 18  // In some display driver board, it might be written as "SCL" and so on. "CLK" or "SCK"
#define TFT_MISO 19 // Ne semble pas être utile, car non tactile

#endif

#define TFT_WIDTH 240
#define TFT_HEIGHT 240
// #define TFT_INIT_ROTATION 0
#define TFT_LIGHT_MODE_BACKGROUND_COLOR GC9A01A_WHITE
#define TFT_LIGHT_MODE_TEXT_COLOR GC9A01A_BLACK
#define TFT_DARK_MODE_BACKGROUND_COLOR GC9A01A_BLACK
#define TFT_DARK_MODE_TEXT_COLOR GC9A01A_WHITE

/*----- Page ID -----*/
#define NB_ACTIVE_PAGES 3 // 3 == Home Page, Ride Page, Ride Statistics Page
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

/*----- SD Card -----*/
#define PIN_SDCARD_MOSI 15
#define PIN_SDCARD_SCLK 14
#define PIN_SDCARD_CS 13
#define PIN_SDCARD_MISO 2
#define PIN_LED 12

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
#define PIN_GSM_DTR 25
#define PIN_GSM_TX 27
#define PIN_GSM_RX 26
#define PIN_GSM_PWR 4
#define TINY_GSM_MODEM_SIM7000  // Active le bon processeur
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define MINIMUM_DISTANCE_TO_ADD_POSITION 10

/*----- I2C : Accelerometer, Gyroscope, Compass, Temperature -----*/
#define PIN_GY87_SDA 21
#define PIN_GY87_SCL 22

/*----- Buzzer -----*/
#define PIN_BUZZER 32
// #define PIN_BUZZER 39

/*----- Battery -----*/
#define PIN_BATTERY 35

/*----- Buttons -----*/
#define PIN_BUTTON1 33
#define PIN_BUTTON2 34
#define BUTTON_LONG_PRESS_DURATION_MS 2000
#define BUTTON_INACTIVITY_TIME_MS 300000 // 5 minutes

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

#define BLE_COMPLETED_RIDE_CARACTRISTIC_DATA "51656aa8-b795-427f-a96c-c4b6c57430dd"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_DATA_NAME "Completed Ride Stats"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_DATA_UUID "5a2b4a0f-8ddd-4c69-a825-dbab5822ba0e"

#define BLE_COMPLETED_RIDE_NOTIFICATION_CARACTRISTIC "61656aa8-b795-427f-a96c-c4b6c57430dd"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_NOTIF_NAME "Notification"
#define BLE_COMPLETED_RIDE_DESCRIPTOR_NOTIFICATION_UUID "6a2b4a0f-8ddd-4c69-a825-dbab5822ba0e"

// seuil pour detecter la chute
#define pitch_threshold 30
#define roll_threshold 30