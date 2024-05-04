#pragma once
#include <Arduino.h>
class Location
{
private:
public:
    Location(/* args */);
    Location(double, double);
    ~Location();
    double latitude;
    double longitude;
    friend HardwareSerial &operator<<(HardwareSerial &stream, const Location &loc)
    {
        stream.print("Latitude: ");
        stream.print(loc.latitude, 6); // 打印纬度，保留小数点后 6 位
        stream.print(", Longitude: ");
        stream.print(loc.longitude, 6); // 打印经度，保留小数点后 6 位
        return stream;
    }
};
