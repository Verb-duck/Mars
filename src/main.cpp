
#define DEBUGING GSM

#include <Arduino.h>
#include "EEPROM_memory.h"
#include "Sim800L.h"

#define RX_PIN D2      //подключение GSM
#define TX_PIN D3      //подключение GSM
#define RESET_PIN D1   //пин для програмной перезагрузки модуля GSM
#define LED_PIN D4     //пин индикации работы модуля GSM

Sim800L GSM(RX_PIN,TX_PIN,RESET_PIN,LED_PIN);
int64_t temp;

//***************SETUP*************
void setup() {
  Serial.begin(115200);  
  Serial.println();     
  Serial.println("Start!");
  GSM.begin(115200);
  GSM.checkList();
}
//***************SETUP*************

void waitingSMS();


//***************LOOP**************
void loop() {
  // if (GSM.received()) {
  //   Serial.println(GSM._readSerial());
  // }
  //  if (Serial.available()){
  //   GSM.sendMessage(Serial.readString());
  // }
  waitingSMS();
 
}
//***************LOOP**************
void waitingSMS() {
  byte index = GSM.checkForSMS();
  if(index != 0)
  {
  	Serial.println(GSM.readSms(index));
  } 
}