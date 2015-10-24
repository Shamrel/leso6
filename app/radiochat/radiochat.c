/**
 \file
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief Демонстрация работы с радио трансивером ATMEGA128RFA1 стенда LESO6.
 \details
 Программа Программа отправляет в эфир принятую по uart (8-бит, 115200 бит/с)
 строку. Принятая по радио строка, отправляется по uart.
 \version   0.1
 \date 24.10.2015
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

#include <string.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "gpio.h"
#include "uart.h"

//  Определения для светодиодов.
#define LEDS (PORTF)
#define  LED0    PORTF, 0, H
#define  LED1    PORTF, 1, H
#define  LED2    PORTF, 2, H
#define  LED3    PORTF, 3, H

#define RF_BUFFER_SIZE (127)
char rf_RX_buffer[RF_BUFFER_SIZE];
char rf_TX_buffer[RF_BUFFER_SIZE];

uint8_t uart_rx_ptr = 0;

void rf_init(void)
{
	// Сбрасываем регистры трансивера и конечный автомат.
	TRXPR |= (1<<TRXRST);
	// Разрешаем прерывания.
	IRQ_MASK = (1<<RX_END_EN) | (1<<TX_END_EN);
	// Переводим приемопередатчик в режим приема.
	TRX_STATE = CMD_RX_ON;
	// Ждем, пока установится состояние.
	while (((TRX_STATUS)& 0x1F) != RX_ON);
}

void rf_send(const void *data, uint8_t len)
{
	on(LED0);
	// Переводим приемопередатчик в режим передачи.
	TRX_STATE = CMD_PLL_ON;
	// Ждем, пока установится состояние.
	while (((TRX_STATUS)& 0x1F) != PLL_ON);

	// В первый байт записываем длинну пакета.
	TRXFBST = len + 2;
	// Копируем пакет в буфер передатчика.
	memcpy((void *)(&TRXFBST+1), data, len);

	// Запускаем передачу.
	TRX_STATE = CMD_TX_START;
}

// Обработчик прерывания. Конец передачи пакета.
ISR(TRX24_TX_END_vect)
{
	off(LED0);
	// Переводим приемопередатчик в режим приема.
	TRX_STATE = CMD_RX_ON;
}

// Обработчик прерывания. Пакет принят полностью.
ISR(TRX24_RX_END_vect)
{
	uint8_t length;
	tg(LED2);

	// Проверка контрольной суммы пакета.
	if(PHY_RSSI & (1<<RX_CRC_VALID))
	{	// Проверка контрольной суммы пакета.
		tg(LED3);
		length = TST_RX_LENGTH - 2;
		// Копируем буфер приемника
		memcpy(rf_RX_buffer, (void*)&TRXFBST, length);
		rf_RX_buffer[length] = '\0';
		printf("> %s\n",rf_RX_buffer);
	}
}

//!< Функция вызывается, когда принят по uart байт.
void uart_rx_cb(uint8_t ch)
{
	rf_TX_buffer[uart_rx_ptr++] = ch;
	if(uart_rx_ptr == RF_BUFFER_SIZE) uart_rx_ptr = 0;

	if ((ch == '\n') || (ch == '\r'))
	{
		uart_putchar('\r', NULL);
		uart_putchar('\n', NULL);
		rf_send(rf_TX_buffer, uart_rx_ptr);
		uart_rx_ptr = 0;
	} else
	{
		uart_putchar(ch, NULL);
	}
}

int main() 
{
	//! Инициализация портов для светодиодов.
	DDRF |= (1 << DDF0 | 1 << DDF1 | 1 << DDF2 | 1 << DDF3);
	uart_init();						//!< Инициализируем UART.
	// Устанавливаем функцию обратного вызова на прием байта по UART.
	uart_set_input_cb(uart_rx_cb);

	printf("%s\r\n", __TIME__);
	printf("LESO6 ATMEGA128RFA1\r\n");

	// Инициализация радио трансивера.
	rf_init();

	while(1);

	return 0;
}



