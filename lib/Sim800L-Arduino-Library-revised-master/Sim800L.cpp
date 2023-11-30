/*
 *  This library was written by Vittorio Esposito
 *    https://github.com/VittorioEsposito
 *
 *  Designed to work with the GSM Sim800L.
 *
 *  ENG
 *  	This library uses SoftwareSerial, you can define RX and TX pins
 *  	in the header "Sim800L.h", by default pins are RX=10 and TX=11.
 *  	Be sure that GND is connected to arduino too.
 *  	You can also change the reset_pin as you prefer.
 *
 *	ESP
 *  	Esta libreria usa SoftwareSerial, se pueden cambiar los pines de RX y TX
 *  	en el archivo header, "Sim800L.h", por defecto los pines vienen configurado en
 *  	RX=10 TX=11.
 *  	Tambien se puede cambiar el reset_pin por otro que prefiera
 *
 *	ITA
 *		Questa libreria utilizza la SoftwareSerial, si possono cambiare i pin di RX e TX
 *  	dall' intestazione "Sim800L.h", di default essi sono impostati come RX=10 RX=11
 *		Assicurarsi di aver collegato il dispositivo al pin GND di Arduino.
 *		E' anche possibile cambiare il reset_pin.
 *
 *
 *   DEFAULT PINOUT:
 *        _____________________________
 *       |  ARDUINO UNO >>>   Sim800L  |
 *        -----------------------------
 *            GND      >>>   GND
 *        RX  10       >>>   TX
 *        TX  11       >>>   RX
 *       RESET 2       >>>   RST
 *
 *   POWER SOURCE 4.2V >>> VCC
 *
 *
 *	SOFTWARE SERIAL NOTES:
 *
 *		PINOUT
 *		The library has the following known limitations:
 *		1. If using multiple software serial ports, only one can receive data at a time.
 *		2. Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).
 *		3. Not all pins on the Leonardo and Micro support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 *		4. On Arduino or Genuino 101 the current maximum RX speed is 57600bps
 *		5. On Arduino or Genuino 101 RX doesn't work on Pin 13
 *
 *		BAUD RATE
 *		Supported baud rates are 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, and 115200.
 *
 *
 *	Edited on:  December 24, 2016
 *    Editor:   Vittorio Esposito
 *
 *  Original version by:   Cristian Steib
 *
 *
*/

#include "Arduino.h"
#include "Sim800L.h"
#include <SoftwareSerial.h>

//SoftwareSerial SIM(rx_pin,tx_pin);
//String _buffer;


Sim800L::Sim800L(void) : SoftwareSerial(DEFAULT_RX_PIN, DEFAULT_TX_PIN)
{
    rx_pin 		= DEFAULT_RX_PIN;
    tx_pin 		= DEFAULT_TX_PIN;
    reset_pin 	= DEFAULT_RESET_PIN;
    led_pin 	= DEFAULT_LED_PIN;
    LED_FLAG 	= DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
    rx_pin 		= rx;
    tx_pin 		= tx;
    reset_pin 	= DEFAULT_RESET_PIN;
    led_pin 	= DEFAULT_LED_PIN;
    LED_FLAG 	= DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst) : SoftwareSerial(rx, tx)
{
    rx_pin 		= rx;
    tx_pin 		= tx;
    reset_pin 	= rst;
    led_pin 	= DEFAULT_LED_PIN;
    LED_FLAG 	= DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led) : SoftwareSerial(rx, tx)
{
    rx_pin 		= rx;
    tx_pin 		= tx;
    reset_pin 	= rst;
    led_pin 	= led;
    LED_FLAG 	= true;
}

void Sim800L::begin()
{
  this->begin(DEFAULT_BAUD_RATE);    
}

void Sim800L::begin(uint32_t baud)
{

    pinMode(reset_pin, OUTPUT);
    digitalWrite(reset_pin,HIGH);       //перезагрузка при подаче сигнала низкого уровня
    _baud = baud;
    this->SoftwareSerial::begin(_baud);

    _sleepMode = 0;
    _functionalityMode = 1;

    if (LED_FLAG) pinMode(led_pin, OUTPUT);

    _buffer.reserve(BUFFER_RESERVE_MEMORY); // Reserve memory to prevent intern fragmention
}

void Sim800L::checkList()
{
  String str;
  byte attempt = 0;
  
 //проверяем готовность модуля к приему команд.
  this->SoftwareSerial::print(F("AT\r\n "));    //костыль, без него
  _readSerial();                                //сразу перезагружает
  this->SoftwareSerial::print(F("AT\r\n "));
  if(_readSerial().indexOf("OK")==-1)    //перезагружаем модуль, если не алё
  {
    if(!reset())      
    {
      return ;
    }
  } 
  Serial.println("sim800 answered: OK" ) ;

 //Отключить эхо AT-команд.
  this->SoftwareSerial::print(F("ATE0\r\n "));
  if(_readSerial().indexOf("OK")!=-1)
    Serial.println("echo off OK");
  else 
    Serial.println("echo off NO");
  
  //уровень заряда акб
  Serial.print("battery level: ");
  Serial.println(getChargeLevelBattery());
  
  // //проверка пинкода, ввод.
  // if(statusPin()){  
  //   enterPin();
  // }

  //проверяем готовность и текущее состояние модуля.
  this->SoftwareSerial::print(F("AT+CPAS\r\n "));
    /*Информация о состояние модуля
      +CPAS: 0 OK
      0 – готов к работе
      2 – неизвестно
      3 – входящий звонок
      4 – голосовое соединение*/
  if(_readSerial().indexOf("OK")==-1)  
  { 
    Serial.println("status communication module  ERROR");
    return;
  }
  else      //если всё ок, продолжаем проверку модуля 
  { 
    Serial.println("status communication module  OK");
    
    //проверка регистрации в сети
    Serial.println(registrationInNetwork());

    //Запрос уровня сигнала:
    PRINT("signal quality",getSignalQuality()); 

    //установка времени в модуль
      Serial.print("time update: ");
    if(updateRtcGSM(3))
    {
      Serial.println("OK");
      //RTCtime();
    }
    else Serial.println("ERROR");
      // Serial.print(time.hour + ":");
      // Serial.print( time.minute + ":");
      // Serial.print(time.second) ;//+" "+time.day+"."+time.month+"."+time.year );
 
    //подготовка смс
    while(!prepareForSmsReceive())
    {
      attempt++;
      if (attempt == 5)
      {
        PRINT("prepareForSmsReceive", "ERROR");
        break;;
      }
      delay(1000);
    }
    attempt = 0;
    PRINT("prepareForSmsReceive", "OK");
  }

}


/*
 * AT+CSCLK=0	Disable slow clock, module will not enter sleep mode.
 * AT+CSCLK=1	Enable slow clock, it is controlled by DTR. When DTR is high, module can enter sleep mode. When DTR changes to low level, module can quit sleep mode
 */
bool Sim800L::setSleepMode(bool state)
{

    _sleepMode = state;

    if (_sleepMode) this->SoftwareSerial::print(F("AT+CSCLK=1\r\n "));
    else 			this->SoftwareSerial::print(F("AT+CSCLK=0\r\n "));

    if ( (_readSerial().indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

bool Sim800L::getSleepMode()
{
    return _sleepMode;
}

/*
 * AT+CFUN=0	Minimum functionality
 * AT+CFUN=1	Full functionality (defualt)
 * AT+CFUN=4	Flight mode (disable RF function)
*/
bool Sim800L::setFunctionalityMode(uint8_t fun)
{

    if (fun==0 || fun==1 || fun==4)
    {

        _functionalityMode = fun;

        switch(_functionalityMode)
        {
        case 0:
            this->SoftwareSerial::print(F("AT+CFUN=0\r\n "));
            break;
        case 1:
            this->SoftwareSerial::print(F("AT+CFUN=1\r\n "));
            break;
        case 4:
            this->SoftwareSerial::print(F("AT+CFUN=4\r\n "));
            break;
        }

        if ( (_readSerial().indexOf("ER")) == -1)
        {
            return false;
        }
        else return true;
        // Error found, return 1
        // Error NOT found, return 0
    }
    return false;
}

uint8_t Sim800L::getFunctionalityMode()
{
    return _functionalityMode;
}

bool Sim800L::statusPin()
{
  this->SoftwareSerial::print(F("AT+CPIN?\r\n "));
  String str = _readSerial();
  if ( (_readSerial().indexOf("READY")) == -1)
  {
    PRINT ("enter", str);
    return true;
  }
  else 
  {
    PRINT ("sim pin", "not required");
    return false;  
  }
}

void Sim800L::setPIN(String pin)
{
  PINKode = pin;
}

bool Sim800L::enterPin()
{
    String command;
    command  = "AT+CPIN=";
    command += PINKode;
    command += "\r";

    // Can take up to 5 seconds

    this->SoftwareSerial::print(command);

    if ( (_readSerial(5000).indexOf("ER")) == -1)
    {
      PRINT("the pinKode is not correct","");
      return false;
    }
    else {
      PRINT("the pinKode is correct","");
      return true;
    }
    // Error found, return 1
    // Error NOT found, return 0
}


String Sim800L::getProductInfo()
{
    this->SoftwareSerial::print("ATI\r");
    return (_readSerial());
}

String Sim800L::getOperatorsList()
{

    // Can take up to 45 seconds

    this->SoftwareSerial::print("AT+COPS=?\r");

    return _readSerial(45000);

}

String Sim800L::getOperator()
{

    this->SoftwareSerial::print("AT+COPS ?\r");

    return _readSerial();

}

int Sim800L::getSignalQuality()
{
  this->SoftwareSerial::print(F("AT+CSQ\r\n "));
  if(_readSerialChar())
  {
    return atoi(partMSG[1]);
  }
  return -1;
}

//проверка регистрации в сети
String Sim800L::registrationInNetwork() 
{
  byte attempt = 0;
  while(1)
  {
    this->SoftwareSerial::print(F("AT+CREG?\r\n "));
    if(_readSerialChar())
    {
      //2ой параметр
      switch (getInt(2)) {
        case 0:
          return "not registered on the network";
        case 1:
          return "registered,home network";
        case 2:     //поиск сети
            attempt++;
            delay(500);
            if(attempt == 14)     //ждём 7 сек, и идём дальше
            {
              return "not registered,the search for a new network will continue";
            }
          break;
        case 3:
          return "registration rejected";
        case 5:
          return "roaming";
        default:
          break;        
      }
    }
  }
}
int Sim800L::getChargeLevelBattery() 
{
  this->SoftwareSerial::print(F("AT+CBC\r\n "));
  //возвращает:  +СBC состояние, заряд АКБ, напряжение
//    СОСТОЯНИЕ - представлено числом от 0 до 5:
// 0 - Адаптер ЗУ не подключён.
// 1 - Адаптер ЗУ подключён.
// 2 - Адаптер ЗУ подключён, АКБ заряжается.
// 3 - Адаптер ЗУ подключён, АКБ заряжена.
// 4 - Зарядка прервана.
// 5 - Зарядка прервана из-за превышения темп
  if(_readSerialChar())
  {
    //if(equals(0 , "ERROR"))
    return atoll(partMSG[2]);
  }
  return -1;
}

int Sim800L::getVoltageBattery()
{
  this->SoftwareSerial::print(F("AT+CBC\r\n "));
  if(_readSerialChar())
  {
    //if(equals(0 , "ERROR"))
    return atoll(partMSG[2]);
  }
  return -1;
}




bool Sim800L::calculateLocation()
{
    /*
    	Type: 1  To get longitude and latitude
    	Cid = 1  Bearer profile identifier refer to AT+SAPBR
    */

    uint8_t type = 1;
    uint8_t cid = 1;
	
	String tmp = "AT+CIPGSMLOC=" + String(type) + "," + String(cid) + "\r\n";
	this->SoftwareSerial::print(tmp);
	
	/*
    this->SoftwareSerial::print("AT+CIPGSMLOC=");
    this->SoftwareSerial::print(type);
    this->SoftwareSerial::print(",");
    this->SoftwareSerial::print(cid);
    this->SoftwareSerial::print("\r");
	*/

    String data = _readSerial(20000);

    if (data.indexOf("ER")!=(-1)) return false;

    uint8_t indexOne;
    uint8_t indexTwo;

    indexOne = data.indexOf(":") + 1;
    indexTwo = data.indexOf(",");

    _locationCode = data.substring(indexOne, indexTwo);

    indexOne = data.indexOf(",") + 1;
    indexTwo = data.indexOf(",", indexOne);

    _longitude = data.substring(indexOne, indexTwo);

    indexOne = data.indexOf(",", indexTwo) + 1;
    indexTwo = data.indexOf(",", indexOne);

    _latitude = data.substring(indexOne, indexTwo);

    return true;


}
String Sim800L::getLocationCode()
{
    return _locationCode;
    /*
     Location Code:
     0      Success
     404    Not Found
     408    Request Time-out
     601    Network Error
     602    No Memory
     603    DNS Error
     604    Stack Busy
     65535  Other Error
    */
}

String Sim800L::getLongitude()
{
    return _longitude;
}

String Sim800L::getLatitude()
{
    return _latitude;
}


//
//PUBLIC METHODS
//

bool Sim800L::reset()
{
  PRINT("restart sim800",);
  if (LED_FLAG) digitalWrite(led_pin,1);
  digitalWrite(reset_pin,0);
  delay(1000);
  digitalWrite(reset_pin,1);
  delay(2000);
  //wait for the module response
  this->SoftwareSerial::print(F("AT\r\n"));
  byte attempt = 0;
  while (_readSerial().indexOf("OK")==-1 )
  {
    attempt++;
    if(attempt == 5) {
      PRINT("sim800 doesn't answer", "");
      return false;
    }       
    this->SoftwareSerial::print(F("AT\r\n"));
  }
  if (LED_FLAG) digitalWrite(led_pin,0);
  return true;
}

String  Sim800L::readMessage(){
  
  return _readSerial(10);
}

void Sim800L::sendMessage(String message){
  this->SoftwareSerial::print (message);
}

void Sim800L::setPhoneFunctionality()
{
    /*AT+CFUN=<fun>[,<rst>]
    Parameters
    <fun> 0 Minimum functionality
    1 Full functionality (Default)
    4 Disable phone both transmit and receive RF circuits.
    <rst> 1 Reset the MT before setting it to <fun> power level.
    */
    this->SoftwareSerial::print (F("AT+CFUN=1\r\n"));
}


String Sim800L::signalQuality()
{
    /*Response
    +CSQ: <rssi>,<ber>Parameters
    <rssi>
    0 -115 dBm or less
    1 -111 dBm
    2...30 -110... -54 dBm
    31 -52 dBm or greater
    99 not known or not detectable
    <ber> (in percent):
    0...7 As RXQUAL values in the table in GSM 05.08 [20]
    subclause 7.2.4
    99 Not known or not detectable
    */
    this->SoftwareSerial::print (F("AT+CSQ\r\n"));
    return(_readSerial());
}

//открытие GPRS соединения
void Sim800L::activateBearerProfile()
{
    this->SoftwareSerial::print (F(" AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r\n" ));
    _buffer=_readSerial();  // set bearer parameter
    this->SoftwareSerial::print (F(" AT+SAPBR=3,1,\"APN\",\"internet\" \r\n" ));
    _buffer=_readSerial();  // set apn
    this->SoftwareSerial::print (F(" AT+SAPBR=1,1 \r\n"));
    delay(1200);
    _buffer=_readSerial();			// activate bearer context
    this->SoftwareSerial::print (F(" AT+SAPBR=2,1\r\n "));
    delay(3000);
    _buffer=_readSerial(); 			// get context ip address
}

//закрытие GPRS соединения
void Sim800L::deactivateBearerProfile()
{
    this->SoftwareSerial::print (F("AT+SAPBR=0,1\r\n "));
    delay(1500);
}



bool Sim800L::answerCall()
{
    this->SoftwareSerial::print (F("ATA\r\n"));
    //Response in case of data call, if successfully connected
    if ( (_readSerial().indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}


void  Sim800L::callNumber(char* number)
{
    this->SoftwareSerial::print (F("ATD"));
    this->SoftwareSerial::print (number);
    this->SoftwareSerial::print (F(";\r\n"));
}



uint8_t Sim800L::getCallStatus()
{
    /*
      values of return:

     0 Ready (MT allows commands from TA/TE)
     2 Unknown (MT is not guaranteed to respond to tructions)
     3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
     4 Call in progress

    */
    this->SoftwareSerial::print (F("AT+CPAS\r\n"));
    _buffer=_readSerial();
    return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();

}


bool Sim800L::hangoffCall()
{
    this->SoftwareSerial::print (F("ATH\r\n"));
    _buffer=_readSerial();
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}


bool Sim800L::sendSms(const char* number,const char* text)
{

    // Can take up to 60 seconds

    this->SoftwareSerial::print (F("AT+CMGF=1\r")); 	//set sms to text mode
    _buffer=_readSerial();
    this->SoftwareSerial::print (F("AT+CMGS=\""));  	// command to send sms
    this->SoftwareSerial::print (number);
    this->SoftwareSerial::print(F("\"\r"));
    _buffer=_readSerial();
    this->SoftwareSerial::print (text);
    this->SoftwareSerial::print ("\r");
    _buffer=_readSerial();
    this->SoftwareSerial::print((char)26);
    _buffer=_readSerial(60000);
    // Serial.println(_buffer);
    //expect CMGS:xxx   , where xxx is a number,for the sending sms.
    if ((_buffer.indexOf("ER")) != -1) {
        return true;
    } else if ((_buffer.indexOf("CMGS")) != -1) {
        return false;
  	} else {
    	return true;
  	}
    // Error found, return 1
    // Error NOT found, return 0
}

//AT+CUSD=1,"*111#" – проверяем баланс SIM-карты.
bool Sim800L::prepareForSmsReceive()
{
	// Configure SMS in text mode
	this->SoftwareSerial::print(F("AT+CMGF=1\r"));
    _buffer=_readSerial();
    //Serial.print(_buffer);
    if((_buffer.indexOf("OK")) == -1)
    {
        return false;
    }
	this->SoftwareSerial::print(F("AT+CNMI=2,1,0,0,0\r"));
    _buffer=_readSerial();
    //Serial.print(_buffer);
    if((_buffer.indexOf("OK")) == -1)
    {
        return false;
    }
    return true;
}

const uint8_t Sim800L::checkForSMS()
{
	 _buffer = _readSerial(100);
	 if(_buffer.length() == 0)
	 {
	 	return 0;
	 }
     _buffer += _readSerial(1000);
	 // Serial.println(_buffer);
	 // +CMTI: "SM",1
	 if(_buffer.indexOf("CMTI") == -1)
	 {
	 	return 0;
	 }
	 return _buffer.substring(_buffer.indexOf(',')+1).toInt();
}

String Sim800L::getNumberSms(uint8_t index)
{
    _buffer=readSms(index);
    //Serial.println(_buffer.length());
    if (_buffer.length() > 10) //avoid empty sms
    {
        uint8_t _idx1=_buffer.indexOf("+CMGR:");
        _idx1=_buffer.indexOf("\",\"",_idx1+1);
        return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
    }
    else
    {
        return "";
    }
}

String Sim800L::readSms(uint8_t index)
{
    // Can take up to 5 seconds

    if(( _readSerial(5000).indexOf("ER")) != -1)
    {
    	return "";
    }

    this->SoftwareSerial::print (F("AT+CMGR="));
    this->SoftwareSerial::print (index);
    this->SoftwareSerial::print ("\r");
    _buffer=_readSerial();
    //Serial.println(_buffer);
    if (_buffer.indexOf("CMGR") == -1)
    {
    	return "";
    }

	_buffer = _readSerial(10000);
	byte first = _buffer.indexOf('\n', 2) + 1;
	byte second = _buffer.indexOf('\n', first);
    return _buffer.substring(first, second);
}

bool Sim800L::delAllSms()
{
    // Can take up to 25 seconds

    this->SoftwareSerial::print(F("at+cmgda=\"del all\"\n\r"));
    _buffer=_readSerial(25000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

String Sim800L::getRtc()
{
    this->SoftwareSerial::print(F("at+cclk?\r\n"));
    // if respond with ERROR try one more time.
    _buffer=_readSerial();
    if ((_buffer.indexOf("ERR"))!=-1)
    {
        delay(50);
        this->SoftwareSerial::print(F("at+cclk?\r\n"));
        _buffer=_readSerial();

    }
    if ((_buffer.indexOf("ERR"))==-1)
    {
        _buffer=_buffer.substring(_buffer.indexOf("\"")+1,_buffer.lastIndexOf("\"")-3);
        //меняем день и год в строке местами
        _buffer[0] = _buffer[0] + _buffer[6] - (_buffer[6] = _buffer[0]);
        _buffer[1] = _buffer[1] + _buffer[7] - (_buffer[7] = _buffer[1]);
        return _buffer;
    }
    return "";
}

//обновление переменных с временем и датой из модуля
void Sim800L::updateRtc()
{
    this->SoftwareSerial::print(F("at+cclk?\r\n"));
    // if respond with ERROR try one more time.
    _buffer=_readSerial();
    if ((_buffer.indexOf("ERR"))!=-1)
    {
        delay(50);
        this->SoftwareSerial::print(F("at+cclk?\r\n"));
        _buffer=_readSerial();

    }
    if ((_buffer.indexOf("ERR"))==-1)
    {
        _buffer=_buffer.substring(_buffer.indexOf("\"")+1,_buffer.lastIndexOf("\"")-3);
        year=_buffer.substring(0,2).toInt();
        month= _buffer.substring(3,5).toInt();
        day=_buffer.substring(6,8).toInt();
        hour=_buffer.substring(9,11).toInt();
        minute=_buffer.substring(12,14).toInt();
        second=_buffer.substring(15,17).toInt();
    }
}

//Get the time  of the base of GSM, возврат времени по гринвичу
String Sim800L::dateNet()
{
    this->SoftwareSerial::print(F("AT+CIPGSMLOC=2,1\r\n "));
    _buffer=_readSerial();

    if (_buffer.indexOf("OK")!=-1 )
    {
        return _buffer.substring(_buffer.indexOf(":")+2,(_buffer.indexOf("OK")-4));
    }
    else
        return "0";
}

// Update the RTC of the module with the date of GSM.
bool Sim800L::updateRtcGSM(int utc)
{

    activateBearerProfile();
    _buffer=dateNet();
    deactivateBearerProfile();

    _buffer=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length());
    String dt=_buffer.substring(0,_buffer.indexOf(","));
    String tm=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length()) ;

    int hour = tm.substring(0,2).toInt();
    int day = dt.substring(8,10).toInt();

    hour=hour+utc;

    String tmp_hour;
    String tmp_day;
    //TODO : fix if the day is 0, this occur when day is 1 then decrement to 1,
    //       will need to check the last month what is the last day .
    if (hour<0)
    {
        hour+=24;
        day-=1;
    }
    if (hour<10)
    {

        tmp_hour="0"+String(hour);
    }
    else
    {
        tmp_hour=String(hour);
    }
    if (day<10)
    {
        tmp_day="0"+String(day);
    }
    else
    {
        tmp_day=String(day);
    }
    //for debugging
    //Serial.println("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
    this->SoftwareSerial::print("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
    if ( (_readSerial().indexOf("ER")) ==-1)
    {      
      return true;
    }
    else return false;


}



//
//PRIVATE METHODS
//
String Sim800L::_readSerial()
{

    uint64_t timeOld = millis();

    while (!this->SoftwareSerial::available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL))
    {
        delay(13);
    }

    String str;

    while(this->SoftwareSerial::available())
    {
        if (this->SoftwareSerial::available()>0)
        {
            str += (char) this->SoftwareSerial::read();
        }
    }

    return str;

}

String Sim800L::_readSerial(uint32_t timeout)
{

    uint64_t timeOld = millis();

    while (!this->SoftwareSerial::available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while(this->SoftwareSerial::available())
    {
        if (this->SoftwareSerial::available()>0)
        {
            str += (char) this->SoftwareSerial::read();
        }
    }

    return str;

}

//чтение входящего буфера с разделением на подстроки
bool Sim800L::_readSerialChar()
{
  bool result = false;
  byte _countBuff = 0;
  _countDivBuff = 0; 
  // if(partMSG)    если не удалять можно по ошибке обратиться к данным предыдущего запроса.
  // {
  //   Serial.println(" delet new");
  //   delete[] partMSG ;
  // }
  //partMSG = new char*[10];
  partMSG[_countDivBuff++]=_buf;          //запоминаем первую подстроку
  uint64_t timeOld = millis();

  while (!this->SoftwareSerial::available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL))
  {
      delay(13);
  }

  while(this->SoftwareSerial::available())
  {
    result = true;
    char ch  = this->SoftwareSerial::read();
    if(ch == ':') 
    {
      _buf[_countBuff++] = '\0';
      partMSG[_countDivBuff++]=_buf + _countBuff;   //запоминаем начало след разделенной строки
      continue;
    }
    if(ch == _divider)                            //если разделитель, 
    {
      _buf[_countBuff++] = '\0';
      partMSG[_countDivBuff++]=_buf + _countBuff;   //запоминаем начало след разделенной строки
      continue;
    }
    if(_countBuff < BUFFER_INCOMING_SIZE - 1)         //если есть место
      _buf[_countBuff++] = ch;
  }
  return result;
}

int64_t Sim800L::getInt(int num) 
  { return atoll(partMSG[num]); }

float Sim800L::getFloat(int num) 
  { return atof(partMSG[num]); }

bool Sim800L::equals(int num, const char* comp)  // сравнить подстроку с другой строкой
  { return !strcmp(partMSG[num], comp);  }  

#if (DEBUGING == sim800)
#undef DEBUGING
#endif