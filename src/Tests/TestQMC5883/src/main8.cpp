// // QMC5883 compass
// // For an HMC5883L chip a popular library is :
// // James Sleeman's HMC5883L library from GitHub.
// // Here the QMC5883L is controlled by direct I2C read/write.
// //
// // Copyright John Main - Free for non commercial use.
// //

// #include "Wire.h" // For 5883L I2C interface
// #include <EEPROM.h>

// #define BUTTON_CAL 27
// #define LED 13
// #define EEADDR 66     // Start location to write EEPROM data.
// #define CALTIME 10000 // In ms.

// static byte stat, ovfl, skipped;
// static int minx, maxx, miny, maxy, offx = 0, offy = 0;

// /////////////////////////////////////////////////////////////
// void I2C_write_AddrDev_AddrReg_Byte(byte i2cAddr, byte regaddr, byte d)
// {
//     Wire.beginTransmission(i2cAddr);
//     Wire.write(regaddr);
//     Wire.write(d);
//     Wire.endTransmission();
// }

// /////////////////////////////////////////////////////////////
// void calc_offsets(void)
// {
//     offx = (maxx + minx) / 2;
//     offy = (maxy + miny) / 2;
// }

// /////////////////////////////////////////////////////////////
// byte magnetometerReady(void)
// {
//     // Data ready?
//     Wire.beginTransmission(0x0d); // Read from status reg
//     Wire.write(0x06);
//     int num = Wire.requestFrom((byte)0x0d, (byte)1);
//     stat = Wire.read(); // DOR Data out Ready (SKIPPED).
//     Wire.endTransmission();

//     ovfl = stat & 0x02;
//     skipped = stat & 0x04;

//     return (stat && 0x01); // 0x01 is the DRDY Data Ready flag
// }

// /////////////////////////////////////////////////////////////
// // If data is not ready x,y,z are not changed.
// byte getMagnetometerRaw(int16_t *x, int16_t *y, int16_t *z)
// {

//     if (!magnetometerReady())
//         return 0;

//     Wire.beginTransmission(0x0d);
//     Wire.write(0x00); // read from address zero = x,y,z registers.
//     int err = Wire.endTransmission();

//     if (!err)
//     {
//         Wire.requestFrom((byte)0x0d, (byte)6); // Blocking?
//         while (Wire.available() < 6)
//             ; // Wait if above blocking then this not needed.
//         *x = (int16_t)(Wire.read() | Wire.read() << 8);
//         *y = (int16_t)(Wire.read() | Wire.read() << 8);
//         *z = (int16_t)(Wire.read() | Wire.read() << 8);
//     }
//     return 1;
// }

// /////////////////////////////////////////////////////////////
// // Orient to board coordinates
// void getMagnetometer(int16_t *x, int16_t *y, int16_t *z)
// {

//     if (!getMagnetometerRaw(x, y, z))
//         return;
//     int tmp = *y;
//     *y = -*x; // x is down.
//     *x = tmp; // y is to the right.
// }

// /////////////////////////////////////////////////////////////
// // Blocking: Waits in this function for reading to be ready.
// void readMagnetometer(int16_t *x, int16_t *y, int16_t *z)
// {
//     while (!magnetometerReady())
//         ;
//     getMagnetometer(x, y, z); // Note: Addresses of pointers passed.
// }
// /////////////////////////////////////////////////////////////
// void calibrate()
// {
//     unsigned long calTimeWas = millis();
//     byte cal = 1, startCal = 1;
//     int16_t x, y, z;
//     float deg = 0, deg2 = 0;

//     readMagnetometer(&x, &y, &z);

//     maxx = minx = x; // Set initial values to current magnetometer readings.
//     maxy = miny = y;

//     while (cal)
//     {

//         if (magnetometerReady())
//             getMagnetometer(&x, &y, &z);
//         if (x > maxx)
//             maxx = x;
//         if (x < minx)
//             minx = x;
//         if (y > maxy)
//             maxy = y;
//         if (y < miny)
//             miny = y;

//         Serial.print("CALIBRATE ");

//         int secmillis = millis() - calTimeWas;
//         int secs = (int)((CALTIME - secmillis + 1000) / 1000);
//         Serial.print("--> ");
//         Serial.println((CALTIME - secmillis) / 1000);

//         if (secs == 0)
//         { // Cal has ended
//             calc_offsets();
//             cal = 0;

//             int EEAddr = EEADDR;
//             EEPROM.put(EEAddr, minx);
//             EEAddr += sizeof(minx);
//             EEPROM.put(EEAddr, maxx);
//             EEAddr += sizeof(maxx);
//             EEPROM.put(EEAddr, miny);
//             EEAddr += sizeof(miny);
//             EEPROM.put(EEAddr, maxy);
//             EEAddr += sizeof(maxy);
//             Serial.println("EEPROM Written"); // make sure this does not repeat on terminal!
//         }

//         delay(10);
//     } // while cal
// }
// /////////////////////////////////////////////////////////////
// void setup()
// {

//     pinMode(LED, OUTPUT);
//     pinMode(BUTTON_CAL, INPUT_PULLUP);

//     Wire.begin();          // Start I2C
//     Wire.setClock(100000); // Test at high speed

//     // Serial.begin(115200);
//     Serial.begin(9600);
//     Serial.println("QMC5883L Digital compass: 3 axis");
//     Serial.println("QMC5883L start initialise.");

//     // Datasheet suggests this for chip startup.
//     I2C_write_AddrDev_AddrReg_Byte(0x0d, 0x0b, 1);
//     I2C_write_AddrDev_AddrReg_Byte(0x0d, 0x09, B00000001);

//     Serial.println(F("DONE initialise"));

//     // Read EEPROM  offset data
//     // int EEAddr = EEADDR;
//     // EEPROM.get(EEAddr, minx);
//     // EEAddr += sizeof(minx);
//     // EEPROM.get(EEAddr, maxx);
//     // EEAddr += sizeof(maxx);
//     // EEPROM.get(EEAddr, miny);
//     // EEAddr += sizeof(miny);
//     // EEPROM.get(EEAddr, maxy);
//     // EEAddr += sizeof(maxy);
//     // calc_offsets();
//     calibrate();
//     Serial.println(F("DONE setup"));
// }

// /////////////////////////////////////////////////////////////
// void loop()
// {
//     Serial.print("loop in");
//     static unsigned long BLTimeWas = millis();
//     static int16_t x, y, z; // Raw compass output values.
//     static int bearing;

//     // if (digitalRead(BUTTON_CAL) == 0)
//     //     calibrate();

//     getMagnetometer(&x, &y, &z);

//     int atan2val = 180 / M_PI * atan2((float)(x - offx), (float)(y - offy));
//     bearing = (-atan2val + 360) % 360;

//     if (millis() - BLTimeWas > 400)
//     { // LED toggle
//         BLTimeWas = millis();
//         static byte togLED = 0;
//         togLED = !togLED;
//         if (togLED)
//             digitalWrite(LED_BUILTIN, HIGH);
//         else
//             digitalWrite(LED_BUILTIN, LOW);

//         Serial.print("Direction: ");
//         Serial.println(bearing);
//     }
// }
// // End of HMC5883 Serial output compass.
