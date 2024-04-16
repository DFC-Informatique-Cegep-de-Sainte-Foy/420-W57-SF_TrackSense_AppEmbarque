#include <Arduino.h>

// #define official_Exemple
#define GPS_Tracking_System // works great,send “Location” to sim carte, traget telephone receive a lien with a location en Google Map
// #define Send_Receive_Messages // works great, send & receive sms

#ifdef official_Exemple
/**
 * @file      GPSDebug.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-06-13
 * @note      The diagram shows the result through external analysis analysis NMAE example,
 *            can be used for GPS failure judgment, can be used to see GPS correct or not operation normal.
 *            If the GPS output is NMEA phrase, it is possible to confirm that GPS is normal. https://github.com/Xinyuan-LilyGO/T-SIM7600X/issues/42#issuecomment-1507181275
 *            It's been a long time since I've been unlocated.
 * */
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define SerialAT Serial1

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

// Use TinyGPS NMEA math analysis library
// #define USING_TINYGPS_LIBRARY           https://github.com/mikalhart/TinyGPSPlus.git

// set GSM PIN, if any
#define GSM_PIN ""

#include <TinyGsmClient.h>
#include "utilities.h"

#ifdef USING_TINYGPS_LIBRARY
// Use TinyGPS NMEA math analysis library
#include <TinyGPS++.h>
TinyGPSPlus gps;
void displayInfo();
#endif

#ifdef DUMP_AT_COMMANDS // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

void setup()
{
  // Serial.begin(115200); // Set console baud rate
  Serial.begin(9600); // Set console baud rate
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  /*
  The indicator light of the board can be controlled
  */
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  /*
  MODEM_PWRKEY IO:4 The power-on signal of the modulator must be given to it,
  otherwise the modulator will not reply when the command is sent
  */
  pinMode(MODEM_PWRKEY, OUTPUT);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(300); // Need delay
  digitalWrite(MODEM_PWRKEY, LOW);

  /*
  MODEM_FLIGHT IO:25 Modulator flight mode control,
  need to enable modulator, this pin must be set to high
  */
  pinMode(MODEM_FLIGHT, OUTPUT);
  digitalWrite(MODEM_FLIGHT, HIGH);

  Serial.println("Start modem...");

  for (int i = 0; i < 3; ++i)
  {
    while (!modem.testAT(5000))
    {
      Serial.println("Try to start modem...");
      pinMode(MODEM_PWRKEY, OUTPUT);
      digitalWrite(MODEM_PWRKEY, HIGH);
      delay(300); // Need delay
      digitalWrite(MODEM_PWRKEY, LOW);
    }
  }

  Serial.println("Modem Response Started.");

  // Stop GPS Server
  modem.sendAT("+CGPS=0");
  modem.waitResponse(30000);

  // Configure GNSS support mode
  modem.sendAT("+CGNSSMODE=15,1");
  modem.waitResponse(30000);

  // Configure NMEA sentence type
  modem.sendAT("+CGPSNMEA=200191");
  modem.waitResponse(30000);

  // Set NMEA output rate to 1HZ
  modem.sendAT("+CGPSNMEARATE=1");
  modem.waitResponse(30000);

  // Enable GPS
  modem.sendAT("+CGPS=1");
  modem.waitResponse(30000);

  // Download Report GPS NMEA-0183 sentence , NMEA TO AT PORT
  modem.sendAT("+CGPSINFOCFG=1,31");
  modem.waitResponse(30000);

  // Disable NMEA OUTPUT
  //  modem.sendAT("+CGPSINFOCFG=0,31");
  //  modem.waitResponse(30000);
}

void loop()
{

#ifdef USING_TINYGPS_LIBRARY
  while (SerialAT.available())
  {
    if (gps.encode(SerialAT.read()))
    {
      displayInfo();
    }
  }
#else
  if (SerialAT.available())
  {
    Serial.write(SerialAT.read());
  }
  if (Serial.available())
  {
    SerialAT.write(Serial.read());
  }
#endif
  delay(1);
}

#ifdef USING_TINYGPS_LIBRARY
void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
#endif

#endif

#ifdef GPS_Tracking_System
#define TINY_GSM_MODEM_SIM7600

#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#include "Adafruit_FONA.h"
#include <HardwareSerial.h>
#include <TinyGsmClient.h>

// LilyGO T-SIM7000G Pinout
#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4

#define LED_PIN 12

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands
#define SerialAT Serial1

TinyGsm modem(SerialAT);
const int FONA_RST = 34;
const int RELAY_PIN = 13;
char replybuffer[255];
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
String smsString = "";
char fonaNotificationBuffer[64]; // for notifications from the FONA
char smsBuffer[250];

HardwareSerial *fonaSerial = &SerialAT;

Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

unsigned long timeout;
char charArray[20];
unsigned char data_buffer[4] = {0};
String mylong = ""; // for storing the longittude value
String mylati = ""; // for storing the latitude value
String textForSMS;
char buff[10];
#define SMS_TARGET "+14182558849"

void gpslocation();
void setup()
{
  SerialMon.begin(9600);
  SerialMon.println("Place your board outside to catch satelite signal");

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Turn on the modem
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(300);
  digitalWrite(PWR_PIN, LOW);

  delay(1000);

  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  fonaSerial->begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX, false);

  SerialMon.println("Initializing modem...");
  if (!modem.restart())
  {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
  }

  // Print modem info
  String modemName = modem.getModemName();
  delay(500);
  SerialMon.println("Modem Name: " + modemName);

  String modemInfo = modem.getModemInfo();
  delay(500);
  SerialMon.println("Modem Info: " + modemInfo);

  modem.sendAT("+SGPIO=0,4,1,1");
  if (modem.waitResponse(10000L) != 1)
  {
    SerialMon.println(" SGPIO=0,4,1,1 false ");
  }

  modem.enableGPS();

  delay(15000);

  if (!fona.begin(*fonaSerial))
  {
    Serial.println(F("Couldn't find FONA"));
    while (1)
      ;
  }
  Serial.println(F("FONA is OK"));

  fonaSerial->print("AT+CNMI=2,1\r\n"); // set up the FONA to send a +CMTI notification when an SMS is received
  Serial.println("FONA Ready");
}

void loop()
{

  char *bufPtr = fonaNotificationBuffer; // handy buffer pointer

  if (fona.available()) // any data available from the FONA?
  {
    int slot = 0; // this will be the slot number of the SMS
    int charCount = 0;
    // Read the notification into fonaInBuffer
    do
    {
      *bufPtr = fona.read();
      Serial.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer) - 1)));

    // Add a terminal NULL to the notification string
    *bufPtr = 0;

    // Scan the notification string for an SMS received notification.
    //   If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot))
    {
      Serial.print("slot: ");
      Serial.println(slot);

      char callerIDbuffer[32]; // we'll store the SMS sender number in here

      // Retrieve SMS sender address/phone number.
      if (!fona.getSMSSender(slot, callerIDbuffer, 31))
      {
        Serial.println("Didn't find SMS message in slot!");
      }
      Serial.print(F("FROM: "));
      Serial.println(callerIDbuffer);

      // Retrieve SMS value.
      uint16_t smslen;
      if (fona.readSMS(slot, smsBuffer, 250, &smslen)) // pass in buffer and max len!
      {
        smsString = String(smsBuffer);
        Serial.println(smsString);
      }

      if (smsString == "location")
      {
        gpslocation();
        //        modem.sendSMS(SMS_TARGET, textForSMS);
        //        //fona.sendSMS(callerIDbuffer,textForSMS );
        //        Serial.println("SMS send");
        //     textForSMS="";
      }
      if (fona.deleteSMS(slot))
      {
        Serial.println(F("OK!"));
      }
      else
      {
        Serial.print(F("Couldn't delete SMS in slot "));
        Serial.println(slot);
        fona.print(F("AT+CMGD=?\r\n"));
      }
    }
  }
}
void gpslocation()
{

  float lat = 0;
  float lon = 0;
  float speed = 0;
  float alt = 0;
  int vsat = 0;
  int usat = 0;
  float accuracy = 0;
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int min = 0;
  int sec = 0;

  for (int8_t i = 15; i; i--)
  {
    SerialMon.println("Requesting current GPS/GNSS/GLONASS location");
    if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                     &year, &month, &day, &hour, &min, &sec))
    {
      SerialMon.println("Latitude: " + String(lat, 8) + "\tLongitude: " + String(lon, 8));
      SerialMon.println("Speed: " + String(speed) + "\tAltitude: " + String(alt));
      SerialMon.println("Visible Satellites: " + String(vsat) + "\tUsed Satellites: " + String(usat));
      SerialMon.println("Accuracy: " + String(accuracy));
      SerialMon.println("Year: " + String(year) + "\tMonth: " + String(month) + "\tDay: " + String(day));
      SerialMon.println("Hour: " + String(hour) + "\tMinute: " + String(min) + "\tSecond: " + String(sec));
      break;
    }
    else
    {
      SerialMon.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
      delay(15000L);
    }
  }
  SerialMon.println("Retrieving GPS/GNSS/GLONASS location again as a string");
  String gps_raw = modem.getGPSraw();
  SerialMon.println("GPS/GNSS Based Location String: " + gps_raw);

  mylati = dtostrf(lat, 3, 6, buff);
  mylong = dtostrf(lon, 3, 6, buff);
  textForSMS = textForSMS + "http://www.google.com/maps/place/" + mylati + "," + mylong;
  delay(5000);
  modem.sendSMS(SMS_TARGET, textForSMS);
  // fona.sendSMS(callerIDbuffer,textForSMS );
  Serial.println("SMS send");
  textForSMS = "";
}

#endif

#ifdef Send_Receive_Messages
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#include "Adafruit_FONA.h"
#include <HardwareSerial.h>
#include <TinyGsmClient.h>

// LilyGO T-SIM7000G Pinout
#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands
#define SerialAT Serial1

TinyGsm modem(SerialAT);
const int FONA_RST = 34;
const int RELAY_PIN = 13;
char replybuffer[255];
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
String smsString = "";
char fonaNotificationBuffer[64]; // for notifications from the FONA
char smsBuffer[250];

HardwareSerial *fonaSerial = &SerialAT;

Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

unsigned long timeout;
char charArray[20];
unsigned char data_buffer[4] = {0};

String textForSMS;
char buff[10];
// #define SMS_TARGET "+923348912230"
// #define SMS_TARGET "+15819785858"
#define SMS_TARGET "+14182558849" // Trancksense发送短信的目的地手机号
int Relay1 = 12;
int Relay2 = 13;
int Relay3 = 14;
int Relay4 = 15;
int pot = 39;
void setup()
{
  // SerialMon.begin(115200);
  SerialMon.begin(9600);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(pot, INPUT);
  // Set LED OFF

  // Turn on the modem ?? 为什么先高后底？？

  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(300);
  digitalWrite(PWR_PIN, LOW);

  delay(1000);

  // Set module baud rate and UART pins
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  fonaSerial->begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX, false);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  if (!modem.restart())
  {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
  }

  // Print modem info
  String modemName = modem.getModemName();
  delay(500);
  SerialMon.println("Modem Name: " + modemName);

  String modemInfo = modem.getModemInfo();
  delay(500);
  SerialMon.println("Modem Info: " + modemInfo);

  if (!fona.begin(*fonaSerial))
  {
    Serial.println(F("Couldn't find FONA"));
    while (1)
      ;
  }
  Serial.println(F("FONA is OK"));

  fonaSerial->print("AT+CNMI=2,1\r\n"); // set up the FONA to send a +CMTI notification when an SMS is received
  Serial.println("FONA Ready");
}

void loop()
{

  int potvalue = analogRead(pot);

  char *bufPtr = fonaNotificationBuffer; // handy buffer pointer

  // 1 FONA收到消息
  if (fona.available()) // any data available from the FONA?
  {
    int slot = 0; // this will be the slot number of the SMS
    int charCount = 0;
    // Read the notification into fonaInBuffer
    do
    {
      *bufPtr = fona.read();
      Serial.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer) - 1)));

    // Add a terminal NULL to the notification string
    *bufPtr = 0;

    // Scan the notification string for an SMS received notification.
    //   If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot))
    {
      Serial.print("slot: ");
      Serial.println(slot);

      char callerIDbuffer[32]; // we'll store the SMS sender number in here

      // Retrieve SMS sender address/phone number.
      if (!fona.getSMSSender(slot, callerIDbuffer, 31))
      {
        Serial.println("Didn't find SMS message in slot!");
      }
      Serial.print(F("FROM: "));
      Serial.println(callerIDbuffer);

      // Retrieve SMS value.
      uint16_t smslen;
      if (fona.readSMS(slot, smsBuffer, 250, &smslen)) // pass in buffer and max len!
      {
        smsString = String(smsBuffer);
        Serial.println(smsString);
      }

      if (smsString == "load1 on")
      {
        digitalWrite(Relay1, HIGH);
        textForSMS = "Miao Xiong Gei Miao Zai Qing An La";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }
      if (smsString == "load1 off")
      {
        digitalWrite(Relay1, LOW);
        textForSMS = "Xiao Xiong Guan Deng La";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }

      if (smsString == "load2 on")
      {
        digitalWrite(Relay2, HIGH);
        textForSMS = "load2 is on";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }
      if (smsString == "load2 off")
      {
        digitalWrite(Relay1, LOW);
        textForSMS = "load2 is off";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }

      if (smsString == "load3 on")
      {
        digitalWrite(Relay3, HIGH);
        textForSMS = "load3 is on";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }
      if (smsString == "load3 off")
      {
        digitalWrite(Relay3, LOW);
        textForSMS = "load3 is off";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }

      if (smsString == "load4 on")
      {
        digitalWrite(Relay4, HIGH);
        textForSMS = "load4 is on";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }
      if (smsString == "load4 off")
      {
        digitalWrite(Relay4, LOW);
        textForSMS = "load4 is off";
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }
      if (smsString == "req")
      {
        textForSMS = potvalue;
        modem.sendSMS(SMS_TARGET, textForSMS);
        textForSMS = "";
      }

      if (fona.deleteSMS(slot))
      {
        Serial.println(F("OK!"));
      }
      else
      {
        Serial.print(F("Couldn't delete SMS in slot "));
        Serial.println(slot);
        fona.print(F("AT+CMGD=?\r\n"));
      }
    }
  }
}
#endif