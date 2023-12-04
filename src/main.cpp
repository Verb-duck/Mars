
#define DEBUGING 1
#define KEY_EEPROM 1      //для сброса EEPROM памяти, поменять число, сохранятся значения присвоенные при инициализации переменных
#define PHONE_NUMBER "+79522220302"     //номер для смс  уведомлений
#define TIME_ZONE 3                      //часовой пояс
#define SEND_SMS 0          //отправка смс при запуске, да/нет
#define DELAY_POWER  5000   //задержка при первом включении, млс.   

#include <Arduino.h>
#include "EEPROM_memory.h"
#include "Sim800L.h"
#include "GyverBME280.h"

//------пины------
#define RX_PIN D6      //подключение GSM
#define TX_PIN D7      //подключение GSM
//#define RESET_PIN D5   //пин для програмной перезагрузки модуля GSM

Sim800L GSM(RX_PIN,TX_PIN);
GyverBME280 bme;      //на wemos подключен на SCL – D1, SDA – D2

//------переменные----
  auto value = create(static_cast<int64_t>(0),"value from sms");  //переменная для сохранения значения из смс
  
#include "Declaration_list.h"

//***************SETUP*************
void setup() {
  delay(DELAY_POWER);
  Serial.begin(115200);  
  Serial.println();     
  Serial.println("Start ESP!");
  GSM.begin(115200);
  GSM.checkList();
  bme.setMode(FORCED_MODE);         //датчик спит, измерения после вызова .oneMeasurement()
  bme.begin(); 
  //memory.update(KEY_EEPROM);      //чтение/сохранение данных из EEPRom памяти 
  sendSetupSms(SEND_SMS);           //отправка заряда, значений с датчика 
  
}


//***************LOOP**************
void loop() {
  
  waitingSMS();

}
