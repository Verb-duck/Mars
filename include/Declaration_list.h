#ifndef Declaration_list_h
#define Declaration_list.h
#include <Arduino.h>

//------предварительное объявление------
  void waitingSMS();
  int64_t numberSearch(const String &strSearch);
  void sendSetupSms();
  
  //макрос
  #if (DEBUGING)
  #define PRINT(title, y) \
    Serial.print(title); \
    Serial.print(": "); \
    Serial.println(y);
  #else 
  #define PRINT(title, y) 
  #endif


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

int chargeMeasurement() {     //измеряем заряд
  return 90;
}

void sendSetupSms()
{
  String message;
 //заряд батареи
  message += "battery: ";
  char temp[5];
  itoa(chargeMeasurement(),temp,10);
  message += temp;
  message += "% ";

 //уровень сигнала
  message += "signal: ";
  itoa(GSM.getSignalQuality(),temp,10);
  message += temp;
  message += " ";

  bme.oneMeasurement();          // Просим датчик проснуться и сделать одно преобразование
  while (bme.isMeasuring());     // Ждем окончания преобразования
 //температура
  message += "temper ";
  dtostrf(bme.readTemperature(), DEC, 1, temp);
  message += temp;
  message += "*C ";
 //влажность
  message += "hum: ";
  dtostrf(bme.readHumidity(), DEC, 1, temp);
  message += temp;
  message += "% ";
 //давление
  message += "pre: ";
  dtostrf(bme.readPressure(), DEC, 1, temp);
  message += temp;
  message += "% ";

  GSM.sendSms(PHONE_NUMBER,message.c_str());
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
#endif