
#define DEBUGING sim800


#include <Arduino.h>
#include "EEPROM_memory.h"
#include "Sim800L.h"

#define RX_PIN D2      //подключение sim800
#define TX_PIN D3      //подключение sim800
#define RESET_PIN D1   //пин для програмной перезагрузки модуля sim800
#define LED_PIN D4     //пин индикации работы модуля sim800

Sim800L sim800(RX_PIN,TX_PIN,RESET_PIN,LED_PIN);


//***************SETUP*************
  String temp;
void setup() {
  Serial.begin(115200);  
  Serial.println();     
  Serial.println("Start!");
  sim800.begin(115200);
  sim800.checkList();
}
//***************SETUP*************


//***************LOOP**************
void loop() {
  // if (sim800.received())
  //   Serial.println(sim800._readSerial());

  //  if (Serial.available())
  //    sim800.sendMessage(Serial.readString());

 
}
//***************LOOP**************

   