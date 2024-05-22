#pragma once
#include <Arduino.h>
#include "stdlib.h"
#include <vector>
#include "Location.h"
#include "ArduinoJson.h"
#include <time.h>
#include <UUID.h>

class Trajet
{
private:
    /* data */
public:
    Trajet();
    Trajet(String, String, String, String, double, double, unsigned long, std::vector<Location> *, std::vector<Location> *, std::vector<Location> *);
    ~Trajet();

    String fromTrajet2Json() const;

    void calculerDuration(long p_duration);
    void calculerDIstance(double p_distance);
    void calculerVitesse(double p_vitesse);
    void rajouterPointInteret(Location p_point);
    void rajouterPointDanger(Location p_point);
    void creerNomTrajet();
    void setUUIDTrajet(String p_uuid);
    void setStart(String p_start);
    void setEnd(String p_end);

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
    static Trajet fromJson2Trajet(const String &json);
};
