# Mars

Инициализация при запуске модуля, функция checkList():

    -проверка модуля  
    -отключение эхо отправленных команд  
    -заряд аккумулятора
    -готовность и текущее состояние модуля  
    -проверка регистрации в сети  
    -проверка/ввод пинкода - пока не добил  
    -Запрос уровня сигнала  
    -обновление времени в модуле из инета
    -подготовка хранилища смс
    
Отправляется смс : заряд батареи, уровень сигнала сети, температура, влажность, давление.

waitingSMS() - читает смс и сохраняет в EEEProm память значение из смс по ключевому слову.
(пока только до 19 разряда, дальше идёт переполнение, позже перепишу, пока что так, есть понимание, но уже лень)

  
