
Демонстрация работы с перефирией ATMEGA128RFA1 стенда LESO6
====================================================================
**Автор**: Shauerman Alexander <shamrel@yandex.ru>  
**Сайт**:  www.labfor.ru  
**Оборудование**: [LESO6](http://www.labfor.ru/devices/leso6)  
**Версия** 0.1  
**Дата** 5.12.2014  
**Компилятор**: AVR-GCC  
 
 В программе демонстрируется работа с жидкокристаллическим
 индикатором(ЖКИ), с матричной клавиатурой (3х4), с часами реального
 времни RTC DC1338 (подключен по i2c), цифровым термометром -- DS18B20
 (интерфейс 1-ware). Ход времени и температура отображаются на дисплее
 и выводятся в uart (8-бит, 115200 бит/с). Цифры, набираемые с клавиатуры,
 отображаются на экране. При запуске программы и наборе цифр подается
 звуковой сигнал.
