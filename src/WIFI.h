/**
 * 
 * WiFi连接
 * 
 */

#ifndef _ESP_REMOTE_CONTROL_WIFI_
#define _ESP_REMOTE_CONTROL_WIFI_

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Config.h>


ESP8266WiFiMulti wifiMulti;

void smartConfig(){
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  WiFi.beginSmartConfig();
  while (!WiFi.smartConfigDone()) {
    Serial.print(".");
    delay(500);
    }
  Serial.println();
  Serial.println("SmartConfig Success");
  Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
  Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
  Config *config = Config::getConfig();
  config->deviceID = WiFi.macAddress();
  config->wifiSSID = WiFi.SSID();
  config->wifiPassword = WiFi.psk();
  config->saveConfig();
  wifiMulti.addAP(WiFi.SSID().c_str(),WiFi.psk().c_str());
}

void loadConfig(){
  Config *config = Config::getConfig();
  wifiMulti.addAP(config->wifiSSID.c_str(), config->wifiPassword.c_str());
}

void beginWiFi(){
  loadConfig();
  wifiMulti.run();
  uint8_t count = 0;
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(1000);
    if(count ++ >= 8){
        //WiFi连接失败, 进入配置模式
        Serial.println("WiFi not connected!");
        smartConfig();
        break;
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



#endif