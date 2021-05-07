/**
 * 
 * 配置
 * 
 */

#ifndef _ESP_REMOTE_CONTROL_CONFIG_
#define _ESP_REMOTE_CONTROL_CONFIG_

#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>


class Config
{
private:
    static Config* _config;
    Config(){
        this->loadConfig();
    };
public:

    String deviceID;
    String wifiSSID;
    String wifiPassword;


    ~Config(){
        this->saveConfig();
    };
    Config(Config &other) = delete;
    void operator=(const Config &) = delete;

    static Config* start(){
        if (_config == nullptr)
        {
            _config = new Config();
        }
        return _config;
    }

    static Config* getConfig(){
        return _config;
    }

    bool saveConfig(){
        Serial.println("Save config ... ...");
        if(!LittleFS.begin()){
            Serial.println("Unable to begin FS");
            return false;
        }
        File f = LittleFS.open("/config", "w");   //create flie or open file
        if (!f)
        {
            Serial.println("Unable to open file for writing");
        }

        StaticJsonDocument<200> doc;
        doc["deviceID"] = deviceID;
        doc["wifiSSID"] = wifiSSID;
        doc["wifiPassword"] = wifiPassword;
        String json;
        serializeJson(doc, json);
        f.write(json.c_str());
        f.close();
        Serial.println(json);
        Serial.println("Save config success");
        return true;
    }

    bool loadConfig(){
        Serial.println("Load config ... ...");
        if(!LittleFS.begin()){
            Serial.println("Unable to begin FS");
            return false;
        }
        File f = LittleFS.open("/config", "r");   //create flie or open file
        if (!f)
        {
            Serial.println("Unable to open file for writing");
            return false;
        }
        String json = f.readString();
        f.close();
        if(json.length() <= 5){
            return false;
        }
        StaticJsonDocument<200> doc;
        deserializeJson(doc, json);
        this->setDeviceID(doc["deviceID"]);
        this->setWifiSSID(doc["wifiSSID"]);
        this->setWifiPassword(doc["wifiPassword"]);
        Serial.println(json);
        Serial.println("Load config success");
        return true;
    }

    
    void setDeviceID(String deviceID){
        this->deviceID = deviceID;
    }
    void setWifiSSID(String wifiSSID){
        this->wifiSSID = wifiSSID;
    }
    void setWifiPassword(String wifiPassword){
        this->wifiPassword = wifiPassword;
    }

};


#endif