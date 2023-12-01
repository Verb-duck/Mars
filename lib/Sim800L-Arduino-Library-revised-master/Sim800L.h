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

#define DEBUGING 1

#ifndef Sim800L_h
#define Sim800L_h
#include <SoftwareSerial.h>
#include "Arduino.h"

#define DEFAULT_RX_PIN 		10
#define DEFAULT_TX_PIN 		11
#define DEFAULT_RESET_PIN 	15		// pin to the reset pin Sim800L

#define DEFAULT_LED_FLAG	true 	// true: use led.	 false: don't user led.
#define DEFAULT_LED_PIN 	13 		// pin to indicate states.

#define BUFFER_RESERVE_MEMORY	255
#define DEFAULT_BAUD_RATE		9600
#define TIME_OUT_READ_SERIAL	5000
#define BUFFER_INCOMING_SIZE 30

#if (DEBUGING)
#define PRINT(title, y) \
  Serial.print(title); \
  Serial.print(": "); \
  Serial.println(y);
#else 
#define PRINT(title, y) 
#endif
int64_t numberSearch(const String &strSearch);

class Sim800L : public SoftwareSerial
{
private:

  uint32_t _baud;
  int _timeout;
  String _buffer;
  bool _sleepMode;
  uint8_t _functionalityMode;
  String _locationCode;
  String _longitude;
  String _latitude;
  String PINKode;

  //read serial sim800  
  char _buf[BUFFER_INCOMING_SIZE];    //буфер входяшего сообщения
  char **partMSG = new char*[10];            //буфер разделенных строк 
  byte _countDivBuff = 0;             //количество подстрок
  char _divider = ',';                //символ для разделения строки на подстроки 
  String _readSerial();
  String _readSerial(uint32_t timeout);
  bool _readSerialChar();
  bool _saveSettingsInEEPROM();                //save settings in EEPROM sim800l
  int64_t getInt(int num);            //получить int из подстроки  
  float getFloat(int num);            //получить float
  bool equals(int num, const char* comp);  // сравнить подстроку с другой строкой
  


public:  

  uint8_t	rx_pin;
  uint8_t tx_pin;
  uint8_t reset_pin;
  uint8_t led_pin;
  bool	LED_FLAG;
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;

  Sim800L(void);
  Sim800L(uint8_t rx, uint8_t tx);
  Sim800L(uint8_t rx, uint8_t tx, uint8_t rst);
  Sim800L(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led);

  void begin();					//Default baud 9600
  void begin(uint32_t baud);
  bool reset();
  void checkList();
  
  String readMessage();
  void sendMessage(String message);

  bool setSleepMode(bool state);
  bool getSleepMode();
  bool setFunctionalityMode(uint8_t fun);
  uint8_t getFunctionalityMode();
  int getChargeLevelBattery();
  int getVoltageBattery();

  bool statusPin ();
  void setPIN(String pin);
  bool enterPin();
  
  String getProductInfo();    //Запрос информации об устройстве

  String getOperatorsList();
  String getOperator();
  String registrationInNetwork();     //проверка регистрации в сети
  int getSignalQuality();
  int getSignalBer();

  bool calculateLocation();
  String getLocationCode();
  String getLongitude();
  String getLatitude();

  bool answerCall();                //Ответить на входящий голосовой вызов.
  void callNumber(char* number);
  bool hangoffCall();               //Разъединение голосового вызова.ф
  uint8_t getCallStatus();

  const uint8_t checkForSMS();    
    //return number sms
    //0 dont new sms
  bool prepareForSmsReceive();
  bool sendSms(const char* number,const char* text);
  String readSms(uint8_t index);
  String getNumberSms(uint8_t index);
  bool delAllSms();

  void setPhoneFunctionality();
  void activateBearerProfile();           //открытие GPRS соединения
  void deactivateBearerProfile();         //закрытие GPRS соединения
  //bool setMode();

  String getRtc();    //получение дата время строкой из модуля
  void updateRtc();   //получение времени и даты из модуля в переменные day, month, hour и т.д.
  bool updateRtcGSM(int utc);   //обновление время и дата модуля из инета с поправкой на часовой пояс
  private:
  String _dateNet();   //Get the time  of the base of GSM, возврат времени по гринвичу
  void setSettingRtcMobilNetwork();  
    /* set the settings for to receive the time form mobile operator.
    calling one time*/
};


#endif
