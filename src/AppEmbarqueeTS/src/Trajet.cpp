#include <Arduino.h>
#include <Trajet.h>

Trajet::Trajet()
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

/// @brief convertir un jsonString a un objet Trajet
/// @param jsonString qui contient un objet Trajet en format String
/// @return un Trajet objet
Trajet Trajet::fromJson2Trajet(const String &jsonString)
{
    // Serial.print("In the method -> fromJsonStr2Trajet -> jsonString entering: ");

    // Serial.println(jsonString);

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

    // Serial.print("In the method -> fromJsonStr2Trajet -> nom from jsonStr: ");

    // Serial.println(nom);

    JsonArray points = doc["points"];
    JsonArray pointsdInteret = doc["pointsdInteret"];
    JsonArray pointsdDanger = doc["pointsdDanger"];

    Trajet t;
    t.ride_id = ride_id;
    t.nom = nom;
    t.distance = distance;
    t.vitesse_moyenne = vitesse_moyenne;
    t.dateBegin = dateBegin;
    t.dateEnd = dateEnd;
    t.duration = duration;
    t.estComplete = estComplete;
    t.estReadyToSave = estReadyToSave;

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
    // Serial.print("In the method -> fromJsonStr2Trajet -> ");
    // Serial.println(t.nom);

    return t;
}

/// @brief converir l'objet Trajet à String de Json
/// @return un String de Json
String Trajet::fromTrajet2Json() const
{
    // Creer DynamicJsonDocument
    DynamicJsonDocument doc(1024);
    // Creer JSON
    JsonObject jsonobj = doc.to<JsonObject>();
    // Affecter JSON obj
    jsonobj["ride_id"] = this->ride_id;
    jsonobj["nom"] = this->nom;
    jsonobj["distance"] = this->distance;
    jsonobj["vitesse_moyenne"] = this->vitesse_moyenne;
    jsonobj["dateBegin"] = this->dateBegin;
    jsonobj["dateEnd"] = this->dateEnd;
    jsonobj["duration"] = this->duration;
    jsonobj["estComplete"] = this->estComplete;
    jsonobj["estReadyToSave"] = this->estReadyToSave;

    JsonArray points = jsonobj.createNestedArray("points");
    for (const Location &loc : *this->points)
    {
        JsonObject locObj = points.createNestedObject();
        locObj["latitude"] = loc.latitude;
        locObj["longitude"] = loc.longitude;
    }

    JsonArray pointsdInteret = jsonobj.createNestedArray("pointsdInteret");
    for (const Location &loc : *this->pointsdInteret)
    {
        JsonObject locObj = pointsdInteret.createNestedObject();
        locObj["latitude"] = loc.latitude;
        locObj["longitude"] = loc.longitude;
    }

    JsonArray pointsdDanger = jsonobj.createNestedArray("pointsdDanger");
    for (const Location &loc : *this->pointsdDanger)
    {
        JsonObject locObj = pointsdDanger.createNestedObject();
        locObj["latitude"] = loc.latitude;
        locObj["longitude"] = loc.longitude;
    }

    // Creer String
    String jsonStr;
    // SerializeJSon
    serializeJson(doc, jsonStr);
    return jsonStr;
}