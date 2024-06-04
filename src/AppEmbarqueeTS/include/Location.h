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
        stream.print(loc.latitude, 6);
        stream.print(", Longitude: ");
        stream.print(loc.longitude, 6);
        return stream;
    }
};
