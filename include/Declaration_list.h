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
    if(str.indexOf("time zone"))          //если пришла смс с тестом value
    {
      time_zone = numberSearch(str);      //сохранаяем в EEPROM значение
    } 
  	Serial.println(GSM.readSms(index));
  } 
}


void sendSetupSms()
{
  static bool send_sms = false;
  static String message;
  message.reserve(110);
  char temp[5];

  //проверяем и ждём готовность модуля связи к приёму команд
  GSM.readiness(TIME_WAITING);      
  
  //делаем измерения, пока загружается модуль связи
  bme.oneMeasurement();          // Просим датчик проснуться и сделать одно преобразование
  while (bme.isMeasuring());     // Ждем окончания преобразования

  //заряд батареи
  message += "battery: ";
  itoa(GSM.getChargeLevelBattery(),temp,10);
  message += temp;
  message += "%";
  message += ", v: ";
  itoa(GSM.getVoltageBattery(),temp,10);
  message += temp;
  message += "mv";
  
  GSM.registrationInNetwork(TIME_WAITING);
  
  //подготовка смс
  if(GSM.prepareForSmsReceive())
  {
    PRINT("prepareForSmsReceive", "OK");
  }
  else
  {
    PRINT("prepareForSmsReceive", "ERROR");
  }

  //уровень сигнала
  message += "/ rssi: ";
  message += GSM.getSignalQuality();
  message += ", ber: ";
  message += GSM.getSignalBer();

  //температура
  message += "/ t: ";
  dtostrf(bme.readTemperature(), 3, 1, temp);
  message += temp;
  message += "*C";
  //влажность
  message += ", h: ";
  dtostrf(bme.readHumidity(), 3, 1, temp);
  message += temp;
  message += "%";
  //давление
  message += ", p: ";
  dtostrf(bme.readPressure(), 4, 1, temp);
  message += temp;
  message += "mm Hg/ ";
  //time
  message += GSM.updateRtc();

  #if( SEND_SMS != 0)
    GSM.sendSms(PHONE_NUMBER,message.c_str());
  #endif
  PRINT("text sms", message );
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

//когда нибудь дописать, выбрасывание исключений.
// template<typename Type> 
// class exception  
// {
//   protected:
//   public:
//   static Type _note;
//   static bool _trouble;
//   exception(Type note)
//   {
//     this->_note = note;
//     _trouble = true;
//   }
//   ~exception() 
//   {
//     _trouble = false;
//   }
// };
// template<typename Type>
// Type exception<Type>::_note;
// template<typename Type>
// bool exception<Type>::_trouble = false;

//    #define CATCH(note)\
//     if(exception::_note == note) continue;\
//     else 

// template<typename Type>
// class catch
// {
//   catch(Type note) 
//   {

//   }
// };


//  #define throw(note)\
//     exception(note);\
//     return 0;

// int try_catch() {  
//   throw(1);
//   return 90;
// }
#endif
