
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


//------предварительное объявление------
void waitingSMS();
int64_t numberSearch(const String &strSearch);



//***************SETUP*************
void setup() {
  Serial.begin(115200);  
  Serial.println();     
  Serial.println("Start ESP!");
  GSM.begin(115200);
  GSM.checkList();
}



//***************LOOP**************
void loop() {
  
  waitingSMS();
 
}

void waitingSMS() {
  byte index = GSM.checkForSMS();
  if(index != 0)
  {
    String str = GSM.readSms(index);
    if(str.indexOf("value"))          //если пришла смс с тестом value
    {
      value = numberSearch(str);      //сохранаяем в EEPROM значение
    }
  	Serial.println(GSM.readSms(index));
  } 
}

int64_t numberSearch(const String &strSearch)
{
    //возвращает число из переданной строки
    //-1 нет числа
    char* searchInt = new char[strSearch.length() + 1];
    int ii = 0;
    int incoming = 0;
    while (1)
    {
        switch (strSearch[incoming])
        {
        case '0':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '1':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '2':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '3':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '4':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '5':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '6':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '7':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '8':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case '9':
            searchInt[ii++] = strSearch[incoming++];
            break;
        case ',':
            searchInt[ii++] = strSearch[incoming++];
            break;
        default:
            if (ii != 0)
            {
                searchInt[ii] = '\0';
                return atoll(searchInt);
            }
            incoming++;
            break;
        }
        if (incoming == strSearch.length())
            return -1;
    }
}