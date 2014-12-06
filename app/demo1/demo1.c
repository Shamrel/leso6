/*
 * keyboard.c
 *
 *  Created on: 24.11.2014
 *      Author: shamrel
 */
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "../../platform/lcd.h"
#include "../../platform/gpio.h"
#include "../../platform/uart.h"
#include "../../platform/rtc.h"
#include "../../platform/i2c.h"

#include "../../platform/ds18b20.h"

#define LEDS (PORTF)
#define  LED0    PORTF, 0, H
#define  LED1    PORTF, 1, H
#define  LED2    PORTF, 2, H
#define  LED3    PORTF, 3, H

#define  BIP     PORTB, 0, H

// keyboard
#define ROWS	(PIND>>4)
#define COLUMNS PORTG
#define SCAN_COLUMM(collum)			\
{	COLUMNS |= 0x07;				\
	COLUMNS &= ~(1<<(collum));		\
	_delay_loop_1(1);}

const char keyChars[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0',
		'#', '\n' };

#define getKeyChar() keyChars[getKey()-1]

uint8_t getKey() {
	uint8_t column, row;

	for (column = 0; column < 3; column++) {
		SCAN_COLUMM(column);
		for (row = 0; row < 4; row++) {
			if (!(ROWS & (1 << row)))
				return (row * 3 + column + 1);
		}
	}
	return 13;
}

volatile uint8_t key_mode;		// Переменная определяет режим работы программы

ISR(TIMER3_OVF_vect)			// Прерырвание по переполнению таймера3
{

	tg(BIP);
	TCNT3 = -8000;
}

ISR(TIMER4_OVF_vect)			// Прерырвание по переполнению таймера 4
{
	TCCR4B_struct.cs4 = 0;		// Выключаем таймер;
	TIMSK4 = 0;					// запрещаем все прерывания от этого таймера.
	key_mode = 0;				// Завершаем режим набора текста.
}

void uart_rx_cb(uint8_t ch) {
	if ((ch == '\n') || (ch == '\r')) {
		uart_putchar('\r', NULL);
		uart_putchar('\n', NULL);
	} else
		uart_putchar(ch, NULL);
}

rtc_data_r_t time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
rtc_data_w_t time_w = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int main() {
	// инициализация светодиодов
	DDRF |= (1 << DDF0 | 1 << DDF1 | 1 << DDF2 | 1 << DDF3);
	// Инициализация пьезоизлучателя.
	DDRB |= (1 << DDB0);
	// строки на ввод
	DDRD &= ~(1 << DDD4 | 1 << DDD5 | 1 << DDD6 | 1 << DDD7);
	// столбцы на ввод
	DDRG |= (1 << DDG0 | 1 << DDG1 | 1 << DDG2);

	// Инициализация таймера для излучателя звука
	TCCR3A = 0;
	TCCR3A = 0;
	TCCR3B = 0;
	TCCR3B_struct.cs3 = 0x01;		// Предделитель: F_CPU/1
	TIMSK3_struct.toie3 = 1;		// Разрешаем прерывание по переполнению
	TCNT3 = -8000;				// 8000 тактов центрального проессора ~ 500 мкс.

	// Инициализация таймера для смены режимов
	TCCR4A = 0;
	TCCR4A = 0;
	TCCR4B = 0;

	uint8_t key1, key2 = 0;
	uint8_t i;
	char tx_buff_str[16];
	int16_t temper = 0;
	ds18b20_memory_t ds18b20_memory = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	uint8_t sec;

	I2C_Master_Initialise(); 			// Инициализация I2C интерфейса.
	ds18b20_convert();					// Запуск преобразование температуры.
	uart_init();						// Инициализируем UART.
	uart_set_input_cb(uart_rx_cb);// Устанавливаем функцию обратного вызова на прием байта по UART.

	printf("\n\rLESO laboratory (c) 2014\r\nDemo test program\r\n");
	printf("LESO6 ATMEGA128RFA1\r\n");
	fprintf(stderr, "stderr: NO ERRORS\r\n");

//
	uint8_t ch[8] = { 0x06, /*x x x 0 0 1 1 0*/
	0x09, /*x x x 0 1 0 0 1*/
	0x09, /*x x x 0 1 0 0 1*/
	0x06, /*x x x 0 0 1 1 0*/
	0x00, /*x x x 0 0 0 0 0*/
	0x00, /*x x x 0 0 0 0 0*/
	0x00, /*x x x 0 0 0 0 0*/
	0x00 /*x x x 0 0 0 0 0*/
	};

	lcd_t lcd = { 0, 0, 0, 0, 0 };				// Управлящая структура для ЖКИ
	lcdInit(&lcd);								// Инициализируем ЖКИ;
	lcdCursor(&lcd, 0);							// Выключаем курсор.
	lcdCursorBlink(&lcd, 0);					// Выключаем мерцание курсора.
	sprintf(tx_buff_str, " LESO6 \n 2014%c  ", 0xb4);
	lcdPuts(&lcd, tx_buff_str);
	lcdCharDef(&lcd, 1, ch);					// Определяем новый символ.

	for (i = 0; i < 75; i++)
		_delay_ms(10);
	getTimeDS1338(&time);
	ds18b20_read(&ds18b20_memory);
	sec = time.Second;

	TIMSK3_struct.toie3 = 0;// Запрещаем прерывание при переполнении таймера 3.
	off(BIP);									// Выключаем звук.

	lcdClear(&lcd);
	lcdCursor(&lcd, 1);
	lcdPuts(&lcd, "Text:\n");
//char temper[9];
	while (1) {
		while (key_mode)		// В режиме набора текст:
		{
			key1 = getKeyChar();
			for(i=0; i<5; i++) _delay_ms(10);
			key2 = getKeyChar();

			if (key1 == '\n') continue;
			else if (key1 != key2) continue;

			TCNT4 = 0;				// Дополнительное время работы в этом режиме
			if((lcd.cx) == lcd.cols)// закончились символы в строке
			{
				lcdPuts(&lcd,"\r        \r");// стираем строку, возвращаем курсор в начало
				printf("\r\033[0K");// стираем строку в терминале
			}
			lcdPutchar(&lcd, key1);
			LEDS &= ~0x0F;
			LEDS |= 0x0F&key1;
			putchar(key1);
			TIMSK3_struct.toie3 = 1;			// пик
			for(i=0; i<20; i++) _delay_ms(10);
			TIMSK3_struct.toie3 = 0;
		}

#if 1
		printf("\r\033[0K%02u:%02u:%02u",time.Hour, time.Minute ,time.Second);
		sprintf(tx_buff_str, "%02u:%02u:%02u\n",time.Hour, time.Minute ,time.Second);
		lcdHome(&lcd);
		lcdPuts(&lcd, tx_buff_str);

		if(ds18b20_crc8((uint8_t *)&ds18b20_memory, sizeof(ds18b20_memory)))
		fprintf(stderr,"ERROR read DS18B20\r\n");
		else
		{
			temper = (ds18b20_memory.temper_MSB << 8) | ds18b20_memory.temper_LSB;
			printf(" T= %d.%u",temper>>4, ((temper&0xf)*1000)/(16));
			sprintf(tx_buff_str,"%02d.%u%cC  ",temper>>4, ((temper&0xf)*1000)/(16), 0x01);
			lcdPuts(&lcd, tx_buff_str);
		}

		ds18b20_convert();		// Запуск преобразование температуры.
		while(sec == time.Second)
		{
			if (getTimeDS1338(&time))
			{
				fprintf(stderr,"ERROR: read date fail..!\n\r");
				continue;
			}

			key1 = getKeyChar();
			if (key1 != '\n')
			{
				key_mode = 1;
				lcdClear(&lcd);
				lcdCursor(&lcd, 1);
				lcdPuts(&lcd,"Text:\n");

				TCCR4B_struct.cs4 = 0x04;		// Предделитель: F_CPU/256
				TIMSK4_struct.toie4 = 1;// Разрешаем прерывание по переполнению
				TCNT4 = 0;
				printf("\r\033[0K");// стираем строку в терминале
				break;
			}
			for(i=0; i<20; i++) _delay_ms(10);
		}
		sec = time.Second;
		ds18b20_read(&ds18b20_memory);
		if(ds18b20_crc8((uint8_t *)&ds18b20_memory, sizeof(ds18b20_memory)))
		fprintf(stderr,"ERROR read DS18B20\r\n");
#endif
	}

	return 0;
}
