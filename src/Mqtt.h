/**
 * 
 * MQTT
 * 
 */

#ifndef _ESP_REMOTE_CONTROL_MQTT_
#define _ESP_REMOTE_CONTROL_MQTT_

#define MQTT_SERVER_ADDRESS "worldyun.xyz"      //MQTT 服务器地址、域名
#define MQTT_SERVER_PORT 1883                   //MQTT 服务器端口
#define MSG_BUFFER_SIZE	3096                    //MQTT msg buffer size 同时还需要修改 PubSubClient.h 的 MQTT_MAX_PACKET_SIZE
#define MSG_CUT_SIZE 128                        //MQTT 上传报文分片大小
#define MQTT_UPLOAD_TOPIC "/upload"             //MQTT 上传topic
#define MQTT_DOWNLOAD_TOPIC "/device/"          //MQTT 订阅topic前缀

#include <PubSubClient.h>
WiFiClient wifiClient;
class Mqtt
{
private:
    static Mqtt* _mqtt;
    PubSubClient* mqttClient;
    String subscribeTopic;
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
        Serial.print("] ");
        for (int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();


        //发送长报文
        String json_str = "[4476, 4424, 568, 1626, 540, 536, 542, 1624, 542, 1624, 542, 536, 542, 534, 542, 1624, 544, 536, 544, 532, 544, 1624, 544, 534, 542, 534, 542, 1624, 544, 1624, 544, 532, 542, 1624, 546, 532, 544, 1624, 544, 1622, 546, 1622, 544, 1624, 544, 534, 544, 1620, 546, 1622, 546, 1622, 542, 534, 544, 532, 544, 532, 546, 532, 544, 1622, 544, 534, 546, 532, 544, 1620, 546, 1622, 568, 1598, 544, 534, 544, 532, 546, 530, 546, 532, 546, 532, 544, 532, 544, 532, 546, 532, 570, 1598, 546, 1620, 546, 1622, 544, 1622, 568, 1598, 546, 5248, 4484, 4398, 594, 1598, 566, 510, 570, 1598, 570, 1598, 568, 508, 568, 508, 570, 1598, 570, 508, 570, 506, 570, 1598, 570, 506, 570, 506, 572, 1596, 570, 1574, 592, 508, 570, 1572, 596, 506, 572, 1596, 572, 1596, 570, 1572, 596, 1596, 572, 504, 570, 1572, 596, 1572, 596, 1572, 596, 506, 570, 506, 570, 506, 570, 508, 570, 1574, 592, 508, 570, 508, 568, 1576, 590, 1574, 594, 1572, 594, 510, 568, 508, 566, 486, 592, 486, 590, 510, 566, 512, 566, 486, 590, 486, 590, 1574, 592, 1576, 592, 1576, 592, 1576, 590, 1578, 588]";
        //拆分字符串发送
        int json_str_len = json_str.length();  //总数据长度

        PubSubClient* client = getMqtt()->getPubSubClient();
        if (json_str_len > MSG_CUT_SIZE)
        {
            //开始发送长文件参数分别为  主题，长度，是否持续
            client->beginPublish(MQTT_UPLOAD_TOPIC, json_str_len, true);
            int count = json_str_len / MSG_CUT_SIZE;
            for (int i = 0; i < (count-1); i++)
            {
                client->print(json_str.substring(i * MSG_CUT_SIZE, (i * MSG_CUT_SIZE + MSG_CUT_SIZE)));
            }
            client->print(json_str.substring(MSG_CUT_SIZE * (count - 1)));
            //结束发送文本
            client->endPublish();
        }
        else
        {
            client->publish(MQTT_UPLOAD_TOPIC, json_str.c_str());
        }

        // getMqtt()->getPubSubClient()->publish(MQTT_UPLOAD_TOPIC, "111");
        //do 
    }

    bool connect(){
        Config* config = Config::getConfig();
        if(mqttClient->connect(config->deviceID.c_str(), config->mqttUserName.c_str(), config->mqttPassword.c_str())){       //MQTT连接
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
                // Once connected, publish an announcement...
                // mqttClient->publish("outTopic", "hello world");
                // ... and resubscribe
                // mqttClient->subscribe(subscribeTopic.c_str());
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