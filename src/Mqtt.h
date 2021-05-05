/**
 * 
 * MQTT
 * 
 */

#ifndef _ESP_REMOTE_CONTROL_MQTT_
#define _ESP_REMOTE_CONTROL_MQTT_

#define MQTT_SERVER_ADDRESS "worldyun.xyz"      //MQTT 服务器地址、域名
#define MQTT_SERVER_PORT 1883                   //MQTT 服务器端口
#define MSG_BUFFER_SIZE	1560                    //MQTT msg buffer size
#define MSG_CUT_SIZE 128                        //MQTT 上传报文分片大小
// #define MQTT_UPLOAD_TOPIC "/upload"             //MQTT 上传topic
#define MQTT_LEARN_TOPIC "/learn"               //MQTT 上传学习码topic
#define MQTT_DOWNLOAD_TOPIC "/device/"          //MQTT 订阅topic前缀
#define MQTT_UAERNAME "esp-device"              //MQTT 用户名
#define MQTT_PASSWORD "123456"                  //MQTT 密码
#define JSON_BUFFER_SIZE 2048                   //Json buff size

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <IR.h>

WiFiClient wifiClient;

class Mqtt
{
private:
    static Mqtt* _mqtt;
    PubSubClient* mqttClient;
    String subscribeTopic;
    char msgBuf[MSG_BUFFER_SIZE];
    uint16_t msgSize;
    Mqtt(){
        mqttClient = new PubSubClient(wifiClient);
        mqttClient->setServer(MQTT_SERVER_ADDRESS, MQTT_SERVER_PORT);
        mqttClient->setCallback(callback);
        mqttClient->setBufferSize(MSG_BUFFER_SIZE);
        subscribeTopic = MQTT_DOWNLOAD_TOPIC + Config::getConfig()->deviceID;
        connect();
    }
public:
    
    ~Mqtt(){
        delete mqttClient;
        mqttClient = nullptr;
    }
    Mqtt(Mqtt &other) = delete;
    void operator=(const Mqtt &) = delete;
    static Mqtt* start(){
        if(_mqtt == nullptr){
            _mqtt = new Mqtt();
        }
        return _mqtt;
    }

    static Mqtt* getMqtt(){
        return _mqtt;
    }

    static void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("]  len: ");
        Serial.println(length);
        Mqtt* mqtt = getMqtt();
        mqtt->msgSize = length;
        for (int i = 0; i < length; i++) {
            mqtt->msgBuf[i] = (char)payload[i];
        }
        mqtt->msgBuf[length] = '\0';
        mqtt->work(topic);
    }

    void work(String topic){
        StaticJsonDocument<JSON_BUFFER_SIZE> json;
        // deserializeMsgPack(json, payload);   //byte[] to json
        deserializeJson(json, msgBuf);      //char[] to json
        uint16_t codeType = json["codeType"];
        uint32_t rawID = json["data"]["rawID"];
        switch (codeType)
        {
        case 1:         //send
            send(json["data"]["raw"].as<JsonArray>());
            break;
        
        case 2:         //learn
            json.clear();
            learn(rawID);
            break;

        } 
    }

    void send(JsonArray jsonArray){
        uint16_t size = jsonArray.size();
        uint16_t *raw = new uint16_t[size]; 
        uint16_t i = 0;
        for (JsonVariant value : jsonArray) {
            raw[i++] = value.as<uint16_t>();
        }
        Serial.println(size);
        IR::getIR()->sendRaw(raw, size);
        delete raw;
    }

    void learn(uint32_t rawID){
        Serial.print("rawID:");
        Serial.println(rawID);
        Serial.println("learn .... .... ");
        uint16_t* raw = IR::getIR()->recRawArray();
        uint16_t rawLen = IR::getIR()->getRawLen();

        /*  ESP8266内存太小， 用JSON会爆掉，只能手动封装， 这段代码留着以后上ESP32时使用
        DynamicJsonDocument doc(JSON_BUFFER_SIZE);
        JsonArray rawArray  = doc.createNestedArray("raw");
        if(raw != nullptr){
            for (size_t i = 0; i < rawLen; i++){
                rawArray.add(raw[i]);
            }
        }
        doc["rawID"] = rawID;
        String uploadString;
        serializeJson(doc, uploadString);
        doc.clear();
        publish(MQTT_LEARN_TOPIC, uploadString);
        Serial.println(uploadString);
        */

        String uploadString = "{\"rawID\":";
        uploadString += rawID;
        uploadString += ",\"raw\":[";
        if(raw != nullptr){
            for (size_t i = 0; i < rawLen; i++){
                uploadString += raw[i];
                if(i != rawLen - 1){
                    uploadString += ",";
                }
            }
        }
        uploadString += "]}";
        Serial.println(uploadString);
        publish(MQTT_LEARN_TOPIC, &uploadString);
    }

    void publish(char* topic, String* payload){

        int len = payload->length();  //总数据长度
        PubSubClient* client = getMqtt()->getPubSubClient();
        if (len > MSG_CUT_SIZE)        //超长报文，切片发送
        {
            //开始发送长文件参数分别为  主题，长度，是否持续
            client->beginPublish(topic, len, true);
            int count = len / MSG_CUT_SIZE;
            for (int i = 0; i < (count-1); i++)
            {
                client->print(payload->substring(i * MSG_CUT_SIZE, (i * MSG_CUT_SIZE + MSG_CUT_SIZE)));
            }
            client->print(payload->substring(MSG_CUT_SIZE * (count - 1)));
            //结束发送文本
            client->endPublish();
        }
        else
        {
            client->publish(topic, payload->c_str());
        }
    }

    bool connect(){
        Config* config = Config::getConfig();
        if(mqttClient->connect(config->deviceID.c_str(), MQTT_UAERNAME, MQTT_PASSWORD)){       //MQTT连接
            mqttClient->subscribe(subscribeTopic.c_str());          //MQTT 订阅
            return true;
        }
        return false;
    }

    void reconnect() {
        // Loop until we're reconnected
        while (!mqttClient->connected()) {
            Serial.print("Attempting MQTT connection...");
            // Attempt to connect
            if (connect()) {
                Serial.println("connected");
            } else {
                Serial.print("failed, rc=");
                Serial.print(mqttClient->state());
                Serial.println(" try again in 5 seconds");
                // Wait 5 seconds before retrying
                delay(5000);
            }
        }
    }
    
    static void loop() {
        Mqtt* mqtt = getMqtt();
        if (!mqtt->getPubSubClient()->connected()) {
            mqtt->reconnect();
        }
        mqtt->getPubSubClient()->loop();
    }

    PubSubClient* getPubSubClient(){
        return mqttClient;
    }
};



#endif