
#define DEBUGING GSM

#include <Arduino.h>
#include "EEPROM_memory.h"
#include "Sim800L.h"

//------sim800------
#define RESET_PIN D1   //пин для програмной перезагрузки модуля GSM
#define RX_PIN D2      //подключение GSM
#define TX_PIN D3      //подключение GSM
Sim800L GSM(RX_PIN,TX_PIN,RESET_PIN);

//------переменные----
auto value = create(static_cast<int64_t>(0),"value from sms");  //переменная для сохранения значения из смс

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
  
}