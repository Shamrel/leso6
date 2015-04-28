/**
 \file
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief LESO6 LabVIEW измерение температуры.
 \details  Программа для демонстрации работы LabVIEW с учебным стендом.
 По команде возвращает значение температуры с датчика DS18B20. Команда --
 произвольный байт.
 \version   0.1
 \date 26.04.2015
 \copyright
Это программное обеспечение распространяется под лицензией BSD 2-ух пунктов. Эта лицензия дает
все права на использование и распространение программы в двоичном виде или
в виде исходного кода, при условии, что в исходном коде сохранится указание
авторских прав.

This software is licensed under the simplified BSD license. This license gives
everyone the right to use and distribute the code, either in binary or
source code format, as long as the copyright license is retained in
the source code.
 */
#include <avr/io.h>
#include "uart.h"
#include "ds18b20.h"

int main()
{
	uart_init();						// Инициализируем UART.
	// Структура для чтения температуры.
	ds18b20_memory_t ds18b20_memory = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	for(;;)								// Бесконечный цикл.
	{
		ds18b20_convert();				// Запускаем преобразование температуры.
		// Ждем команды.
		uart_getchar(NULL);
		ds18b20_read(&ds18b20_memory);	// Читаем температуру.

		// Проверяем контрольную сумму считанных из датчика данных.
		if(!ds18b20_crc8((uint8_t *)&ds18b20_memory, sizeof(ds18b20_memory)))
		{	// Если сошлось, то отправляем значение температуры:
			// Передаем целую часть значения температуры.
			uart_putchar(ds18b20_memory.temper_MSB, NULL);
			// Передаем дробную часть значения температуры.
			uart_putchar(ds18b20_memory.temper_LSB, NULL);
		}
	}
	return 0;
}
