/**
 * 
 * 处理红外信息
 * 
 * 
 * 
 */

#ifndef _ESP_REMOTE_CONTROL_IR_
#define _ESP_REMOTE_CONTROL_IR_

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

class IR
{
    private:
        IRrecv* iRrecv;       //recObj
        IRsend* iRsend;       //sendObj
        const uint16_t khz = 38;       //红外频率,一般为38kHz
        decode_results results;    //recDataObj
        bool _useLed = false;
        uint16_t _ledPin = 2;
        uint8_t recCount;

        // IR(uint16_t recvPin = 4, uint16_t buffSize = 200, uint16_t sendPin = 5, bool isLED = false, uint16_t ledPin = 2){
        //     this->iRrecv = new IRrecv(recvPin, buffSize);
        //     this->iRsend = new IRsend(sendPin);
        // }
    public:
        ~IR(){
            delete iRrecv;
            iRrecv = nullptr;
            delete iRsend;
            iRsend = nullptr;
        }
        void begin(uint16_t recvPin = 4, uint16_t buffSize = 200, uint16_t sendPin = 5, bool useLed = false, uint16_t ledPin = 2){
            iRrecv = new IRrecv(recvPin, buffSize);
            iRsend = new IRsend(sendPin);

            iRrecv->enableIRIn();   // 启用红外接收
            iRsend->begin();        //启用红外发送

            _useLed = useLed;
            _ledPin = ledPin;
        }

        void sendRaw(uint16_t buf[], uint16_t len){
            ledOn();
            iRsend->sendRaw(buf, len, khz);
            ledOff();
        }

        uint16_t* recRawArray(){
            if(recData()){
                iRrecv->resume();  //刷新
                return NULL;
            }
            iRrecv->resume();  //刷新
            return resultToRawArray(&results);
            
        }


        String recRawString(){
            if(!recData()){
                iRrecv->resume();  //刷新
                return "";
            }
            iRrecv->resume();  //刷新
            return resultToSourceCode(&results);
        }


        bool recData(){
            recCount = 0;
            ledOn();
            while (!iRrecv->decode(&results)){   //阻塞，直到获取到红外信息
                delay(1000);
                if(recCount ++ >=10){
                    ledOff();
                    return false;
                }
            }
            ledOff();
            return true;
        }

        void ledOn(){
            if (_useLed)
            {
                digitalWrite(_ledPin, LOW);   //默认低电平，打开led
            }
        }

        void ledOff(){
            if (_useLed)
            {
                digitalWrite(_ledPin, HIGH);   //默认高电平，关闭led
            }
        }


        IRrecv* getIRrecv(){
            return iRrecv;
        }

        IRsend* getIRsend(){
            return iRsend;
        }
};



#endif
