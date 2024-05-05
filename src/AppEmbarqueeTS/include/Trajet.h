#pragma once
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

    String fromTrajet2JsonStr() const;

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
    static Trajet fromJsonStr2Trajet(const String &json);
};
