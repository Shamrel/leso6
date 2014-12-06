/**
 \file ds18b20.h
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief Библиотека для работы с термодатчиком DS18B20 стенда LESO6.
 \details Библиотека содержит функции для работы с термодатчиком DS18B20.
 Датчик подключен по однопроводному интерфейсу 1-Wire (One Wire Interface -- OWI)
 \version   0.1
 \date 4.12.2014
 \copyright
Это программное обеспечение распространяется под лицензией BSD 2-ух пунктов.
Эта лицензия дает все права на использование и распространение программы в
двоичном виде или в виде исходного кода, при условии, что в исходном коде
сохранится указание авторских прав.

This software is licensed under the simplified BSD license. This license
gives everyone the right to use and distribute the code, either in binary or
source code format, as long as the copyright license is retained in
the source code.
 */

#ifndef DS18B20_H_
#define DS18B20_H_


#include <stdint.h>

/**
 \struct ds18b20_memory_
 \brief Структура отображает карту внутренней памяти DS18B20.
 */
typedef struct ds18b20_memory_
{
	uint8_t temper_LSB;
	uint8_t temper_MSB;
	uint8_t Th;
	uint8_t Tl;
	uint8_t config;
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t reserved3;
	uint8_t crc;
}
ds18b20_memory_t;

/**
\brief Процедура инициализации -- сброс и проверка наличия устройства.
\return  1 -- на шине есть устройство;
\return  0 -- на шине нет устройства;
*/
#define ds18b20_presence() OWI_presence();

/**
 \brief Функция запускает преобразование (измерение температуры).
 \return  0 -- преобразование запущено успешно;
 \return -1 -- устройство не отвечает.
*/
int8_t ds18b20_convert();

/**
 \brief Читаем память DS18b20 (SCRATCHPAD).
 \param *memory Указатель на структуру, описывающую карту памяти DS18B20
 \return  0 -- преобразование запущено успешно;
 \return -1 -- устройство не отвечает.
*/
int8_t ds18b20_read(ds18b20_memory_t *memory);

/**
 \brief Вычисляет контрольную сумм по полигону X^8+X^5+X^4+X^0
 \note Если последний байт входных данных равен контрольной сумме по
  всем предыдущим, то функция вернет 0.
 \note Расчет требует много процессорного времени.
 \param *data Указатель на байтовый массив данных.
 \param len Длинна массива.
 \return  Контрольная сумма.
*/
uint8_t ds18b20_crc8( uint8_t *data, uint8_t len );

#endif /* DS18B20_H_ */
