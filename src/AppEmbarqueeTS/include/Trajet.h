#include <Arduino.h>
#include "stdlib.h"
#include <vector>
#include "Location.h"
#include "ArduinoJson.h"
class Trajet
{
private:
    /* data */
public:
    Trajet();
    Trajet(String, String, String, String, double, double, unsigned long, std::vector<Location> *, std::vector<Location> *, std::vector<Location> *);
    ~Trajet();

    String ride_id;
    String nom;
    double distance;
    double vitesse_moyenne;
    String dateBegin;
    String dateEnd;
    unsigned long duration;

    std::vector<Location> *points;
    std::vector<Location> *pointsdInteret;
    std::vector<Location> *pointsdDanger;

    bool estComplete;
    bool estReadyToSave;

    //
    static Trajet fromJson(const String &json);
};

inline Trajet::Trajet()
{
    this->ride_id = String();
    this->nom = String();
    this->dateBegin = String();
    this->dateEnd = String();
    this->distance = 0.0;
    this->vitesse_moyenne = 0.0;
    this->duration = 0.0;
    this->points = new std::vector<Location>();
    this->pointsdInteret = new std::vector<Location>();
    this->pointsdDanger = new std::vector<Location>();
}

Trajet::Trajet(String p_id, String p_nom, String p_dateBegin, String p_dateEnd, double p_distance, double p_vitesse, unsigned long p_duration, std::vector<Location> *p_points, std::vector<Location> *p_points_Interet, std::vector<Location> *p_points_Danger)
    : ride_id(p_id),
      nom(p_nom),
      dateBegin(p_dateBegin),
      dateEnd(p_dateEnd),
      distance(p_distance),
      vitesse_moyenne(p_vitesse),
      duration(p_duration),
      points(p_points),
      pointsdInteret(p_points_Interet),
      pointsdDanger(p_points_Danger),
      estComplete(false),
      estReadyToSave(false)
{
    ;
}

Trajet::~Trajet()
{
    delete this->points;
    delete this->pointsdInteret;
    delete this->pointsdDanger;

    this->points = nullptr;
    this->pointsdInteret = nullptr;
    this->pointsdDanger = nullptr;
}

inline Trajet Trajet::fromJson(const String &jsonString)
{
    Trajet t;
    DynamicJsonDocument doc(1024);    // creer JSON file
    deserializeJson(doc, jsonString); // json string - > json file
    String ride_id = doc["ride_id"];
    String nom = doc["nom"];
    double distance = doc["distance"];
    double vitesse_moyenne = doc["vitesse_moyenne"];
    String dateBegin = doc["dateBegin"];
    String dateEnd = doc["dateEnd"];
    unsigned long duration = doc["duration"];
    bool estComplete = doc["estComplete"];
    bool estReadyToSave = doc["estReadyToSave"];

    JsonArray points = doc["points"];
    JsonArray pointsdInteret = doc["pointsdInteret"];
    JsonArray pointsdDanger = doc["pointsdDanger"];

    for (JsonVariant pointValue : points)
    {
        double lat = pointValue["latitude"];
        double lon = pointValue["longitude"];
        t.points->push_back(Location(lat, lon));
    }

    for (JsonVariant pointValue : pointsdInteret)
    {
        double lat = pointValue["latitude"];
        double lon = pointValue["longitude"];
        t.pointsdInteret->push_back(Location(lat, lon));
    }

    for (JsonVariant pointValue : pointsdDanger)
    {
        double lat = pointValue["latitude"];
        double lon = pointValue["longitude"];
        t.pointsdDanger->push_back(Location(lat, lon));
    }
    return t;
}
