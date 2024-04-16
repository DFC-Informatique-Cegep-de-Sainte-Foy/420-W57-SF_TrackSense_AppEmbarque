#include <Arduino.h>
#include <LittleFS.h>

#include "ControlerConfigurationFile.h"
#include "Configurations.h"

#define FS LittleFS

String ControlerConfigurationFile::readFile()
{
    String content = String("");

    FS.begin(true);
    File file = FS.open(PATH_CONFIGURATION_FILE, "r");

    if (file)
    {
        while (file.available())
        {
            content += file.readString();
        }
    }

    file.close();

    return content;
}

void ControlerConfigurationFile::writeFile(String const &content)
{
    FS.begin(true);
    File file = FS.open(PATH_CONFIGURATION_FILE, "w");

    if (file)
    {
        file.println(content);
    }

    file.close();
}

String ControlerConfigurationFile::getValue(String const &field)
{
    String content = ControlerConfigurationFile::readFile();

    DynamicJsonDocument configFileJson(DYNAMIQUE_JSON_DOCUMENT_SIZE);
    deserializeJson(configFileJson, content);

    return configFileJson[field];
}

void ControlerConfigurationFile::setValue(String const &field, String const &value)
{
    String content = ControlerConfigurationFile::readFile();

    DynamicJsonDocument configFileJson(DYNAMIQUE_JSON_DOCUMENT_SIZE);
    deserializeJson(configFileJson, content);

    configFileJson[field] = value;

    String newContent = String("");
    serializeJson(configFileJson, newContent);

    ControlerConfigurationFile::writeFile(newContent);
}