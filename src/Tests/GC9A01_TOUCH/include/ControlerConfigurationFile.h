#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class ControlerConfigurationFile
{
private:
    static String readFile();
    static void writeFile(String const &content);

public:
    static String getValue(String const &field);
    static void setValue(String const &field, String const &value);
};