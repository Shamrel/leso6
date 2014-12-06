/**
 \file ds18b20.c
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

#include <avr/io.h>
#include <util/delay.h>

#include "ds18b20.h"

#define OWI_PORT 	PORTB
#define OWI_PIN 	PINB
#define OWI_DDR 	DDRB
#define OWI_BIT		(4)


#define OWI_CMD_SKIPROM			(0xCC)					//!< Команда для доступа ко всем устройствам на шине сразу.

#define THERM_CMD_CONVERTTEMP 	(0x44)					//!< Команда однократного преобразования температуры.
#define THERM_CMD_RSCRATCHPAD 	(0xBE)					//!< Команда чтения памяти DS18B20.
#define THERM_CMD_WSCRATCHPAD 	(0x4E)					//!< Команда записи в память DS18B20.

#define OWN_HIGH()		{ OWI_DDR &= ~(1<<OWI_BIT);}	//!< Отпускаем линию. Конфигурируем ее на ввод.
#define OWN_LOW()		{ OWI_DDR |= (1<<OWI_BIT);}		//!< Устанавливаем ноль. Конфигурируем ее на вывод.

void OWI_write_bit(uint8_t bit)
{
	// инициализируем таймслот
	OWN_LOW();						// Удерживаем шину в нуле 1 мкс.
	_delay_us(2);
	if(bit) OWN_HIGH(); 			// Если требуется передать "1", то отпускаем шину.
	_delay_us(60);					// Ждем пока приемник воспримет бит.
	OWN_HIGH(); 					// Отпускаем шину.
	_delay_us(2);
}

uint8_t OWI_read_bit(void)
{
	uint8_t bit;
	// инициализируем таймслот
	OWN_LOW();							// Удерживаем шину в нуле 1 мкс.
	_delay_us(1);
	OWN_HIGH();							// Отпускаем шину.
	_delay_us(15);
	bit = OWI_PIN&(1<<OWI_BIT) ? 1:0;	// Читаем состояние шины.
	_delay_us(45);						// Дожидаемся конца таймслота.
	return bit;
}

void OWI_write_byte(uint8_t byte)
{
	uint8_t i;
	for(i=0; i<8; i++) OWI_write_bit(byte&(1 << i));
}

uint8_t OWI_read_byte(void)
{
	uint8_t i, byte = 0;

	for(i=0; i<8; i++)	byte |=  OWI_read_bit() << i;

	return byte;
}

/**
\brief Процедура инициализации -- сброс и проверка наличия устройства.
\return  1 -- на шине есть устройство;
\return  0 -- на шине нет устройства;
*/
uint8_t OWI_presence(void)
{
	uint8_t	status;

	status = OWI_PIN&(1<<OWI_BIT) ? 1:0;	// Читаем состояние шины.
	if(!status) return status;				// Если на шине уже был ноль, значит либо сбой,
											// либо преобразование еще не завершено.
	OWI_PORT &= ~(1<<OWI_BIT);				// Записываем в регистр вывода ноль.
	OWN_LOW();								// Устанавливаем ноль на линии.
	_delay_us(480);
	OWN_HIGH();								// Отпускаем шину.
	_delay_us(60);							// Ждем пока устроство ответит.
	status = OWI_PIN&(1<<OWI_BIT) ? 0:1;	// Читаем состояние шины.
	_delay_us(420);

	return status;
}

int8_t ds18b20_convert()
{
	if(!OWI_presence()) return (-1);		// Устройство не ответило.
	OWI_write_byte(OWI_CMD_SKIPROM);		// Обращаемся ко всем устройствам на шине сразу.
	OWI_write_byte(THERM_CMD_CONVERTTEMP);	// Команда на запуск преобразования.
	return 0;
}

int8_t ds18b20_read(ds18b20_memory_t *memory)
{
	uint8_t *scrathpad;
	uint8_t i;

	if(!OWI_presence()) return (-1);		// Устройство не ответило.
	OWI_write_byte(OWI_CMD_SKIPROM);		// Обращаемся ко всем устройствам на шине сразу.
	OWI_write_byte(THERM_CMD_RSCRATCHPAD);	// Команда на чтение памяти.

	scrathpad = (uint8_t *) memory;			// Устанавливаем указатель на начало структуры
	for(i=0; i<sizeof(ds18b20_memory_t); i++)
		scrathpad[i] = OWI_read_byte();		// Считываем побайтно память.
	return 0;
}

#define CRC8_POLY    0x18              ///!< Образующий полином: 0X18 = X^8+X^5+X^4+X^0

uint8_t ds18b20_crc8( uint8_t *data, uint8_t len )
{
	uint8_t  crc = 0, i, byte;

	while (len--)
	{
		byte = *data++;
		for (i = 0; i < 8; i++)
		{
			crc = ((crc ^ byte)&1)? ((crc^CRC8_POLY)>>1)|0x80 : (crc>>1);
			byte >>= 1;
		}
	}
	return crc;
}
