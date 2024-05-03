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
};

Location::Location(/* args */)
{
}

inline Location::Location(double p_latitude, double p_longitude)
    : latitude(p_latitude),
      longitude(p_longitude)
{
}

Location::~Location()
{
}
