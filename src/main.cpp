//-----------头文件---------------------------
#include <Arduino.h>
#include <IR.h>
#include <WIFI.h>
#include <Config.h>
#include <ArduinoJson.h>

//-----------宏定义---------------------------
#define IR_RECV_PIN  4           //recPin
#define IR_RECV_BUFFER_SIZE 300  //bufSize
#define IR_SEND_PIN 5            //sendPin
#define LED_PIN 2                //ledPin
#define USE_LED true             //是否启用led



//-----------静态变量初始化--------------------
Config *Config::_config = nullptr;
IR *IR::_ir = nullptr;



//-----------全局变量--------------------------
IR *ir = IR::start(IR_RECV_PIN, IR_RECV_BUFFER_SIZE, IR_SEND_PIN, USE_LED, LED_PIN);   //红外Obj
Config *config = Config::getConfig();



const uint16_t rawCodes_open[199] = {4476, 4424,  568, 1626,  540, 536,  542, 1624,  542, 1624,  542, 536,  542, 534,  542, 1624,  544, 536,  544, 532,  544, 1624,  544, 534,  542, 534,  542, 1624,  544, 1624,  544, 532,  542, 1624,  546, 532,  544, 1624,  544, 1622,  546, 1622,  544, 1624,  544, 534,  544, 1620,  546, 1622,  546, 1622,  542, 534,  544, 532,  544, 532,  546, 532,  544, 1622,  544, 534,  546, 532,  544, 1620,  546, 1622,  568, 1598,  544, 534, 
 544, 532,  546, 530,  546, 532,  546, 532,  544, 532,  544, 532,  546, 532,  570, 1598,  546, 1620,  546, 1622,  544, 1622,  568, 1598,  546, 5248,  4484, 4398,  594, 1598,  566, 510,  570, 1598,  570, 1598,  568, 508,  568, 
508,  570, 1598,  570, 508,  570, 506,  570, 1598,  570, 506,  570, 506,  572, 1596,  570, 1574,  592, 508,  570, 1572,  596, 506,  572, 1596,  572, 1596,  570, 1572,  596, 1596,  572, 504,  570, 1572,  596, 1572,  596, 1572, 
 596, 506,  570, 506,  570, 506,  570, 508,  570, 1574,  592, 508,  570, 508,  568, 1576,  590, 1574,  594, 1572,  594, 510,  568, 508,  566, 486,  592, 486,  590, 510,  566, 512,  566, 486,  590, 486,  590, 1574,  592, 1576, 
 592, 1576,  592, 1576,  590, 1578,  588};


//------------初始化---------------------------
void setup() {
  Serial.begin(115200);            //设置串口波特率
  if(USE_LED){
    pinMode(LED_PIN,OUTPUT);       //led脚输出，低电平点亮
    digitalWrite(LED_PIN, HIGH);   //默认高电平，关闭led
  }
  Config::start();                 //读取config，必须在其他功能前启动
  WIFI::start();                   //启动WIFI
  IR::start(IR_RECV_PIN, IR_RECV_BUFFER_SIZE, IR_SEND_PIN, USE_LED, LED_PIN);   //启动红外
}


void loop() {
  Serial.println(config->wifiSSID);
  // Serial.println(config->wifiSSID);
  // ir.sendRaw((uint16_t *)rawCodes_open, 199);
  // String rawString = ir.recRawString();
  // Serial.println(rawString);
  delay(500);
  
}

