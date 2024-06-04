#include "Location.h"
#include <ostream>
Location::Location()
{
}

Location::Location(double p_latitude, double p_longitude)
    : latitude(p_latitude),
      longitude(p_longitude)
{
}

Location::~Location()
{
}
