# Mars

Инициализация при запуске модуля, функция checkList():

    -проверка модуля  
    -отключение эхо отправленных команд  
    -готовность и текущее состояние модуля
  
    -проверка/ввод пинкода - пока не добил.
  
    -проверка регистрации в сети
  
    -Запрос уровня сигнала
  
    -подготовка хранилища смс
    -заряд аккумулятора
Отправляется смс : заряд батареи, уровень сигнала сети, температура, влажность, давление.

waitingSMS() - читает смс и сохраняет в EEEProm память значение из смс по ключевому слову.
(пока только до 19 разряда, дальше идёт переполнение, позже перепишу, пока что так, есть понимание, но уже лень)

  
