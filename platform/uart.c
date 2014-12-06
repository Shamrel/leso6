/**
 \file usart.c
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief Библиотека для работы с uart стенда LESO6
 \details Библиотека содержит функции для работы с последовательным
 асинхронным портом USART
 \version   0.1
 \date 27.11.2014
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


#include <stdint.h>
#include "uart.h"


/**
\brief Буфер на передачу
*/
uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];

#if UART_TX_BUFFER_SIZE <= 256
volatile uint8_t tx_wr_index,tx_rd_index,tx_counter;
#else
volatile uint16_t tx_wr_index,tx_rd_index,tx_counter;
#endif


/**
\brief Буфер на прием
*/
char uart_rx_buffer[UART_RX_BUFFER_SIZE];

#if UART_RX_BUFFER_SIZE <= 256
volatile uint8_t rx_wr_index,rx_rd_index,rx_counter;
#else
volatile uint16_t rx_wr_index,rx_rd_index,rx_counter;
#endif

/**
\brief Флаг переполнения буфера
*/
uint8_t uart_rx_buffer_overflow;

/**
\brief Прототип функции обратного вызова (Callback).
\details Функция вызывается в прерывании по приходу байта
*/
static void (*uart_input_cb)(uint8_t c);


void
uart_set_input_cb(void (*input)( uint8_t c))
{
  uart_input_cb = input;
}

/**
\brief Прерывание по завершению передачи байта
*/
ISR(UART_TX_COMPLETE_IRQ)
{
	if (tx_counter)
	{
		--tx_counter;
		UDRx = uart_tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
		if (tx_rd_index == UART_TX_BUFFER_SIZE) tx_rd_index=0;
#endif
	}
}

/**
\brief Прерывание по завершению периема байта
*/
ISR(UART_RX_IRQ)
{
	uint8_t status,data;
	status = UCSRxA;
	data = UDRx;

	if ((status & (1 << FEx | 1 << UPEx | 1 << DORx)) == 0)
	{
		uart_rx_buffer[rx_wr_index++]=data;
#if UART_RX_BUFFER_SIZE == 256

		if (++rx_counter == 0)
		{
#else
		if (rx_wr_index == UART_RX_BUFFER_SIZE) rx_wr_index = 0;
		if (++rx_counter == UART_RX_BUFFER_SIZE)
		{
		rx_counter = 0;
#endif
		uart_rx_buffer_overflow = 1;
		}
	}

	if (uart_input_cb!=NULL)			// если Callback функция определена,
				uart_input_cb(data);	 //вызываем ее
}

/**
\brief Стандартные потоки ввода*вывода.
*/
#ifdef USE_STDOUT
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
#endif
#ifdef USE_STDIN
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
#endif



void uart_init(void)
{
	// Устанавливаем скорость UART

	UBRRxH = UBRRH_VALUE;
	UBRRxL = UBRRL_VALUE;

#if USE_2X
UCSRxA |= (1 << U2Xx);
#endif

	// Разрешаем Rx и Tx, разрешаем соответствующие прерывания
	UCSRxB = (1 << RXENx) | (1 << TXENx) | (1 << RXCIEx) | (1 << TXCIEx);

	// 8-bit, 1 stop bit, no parity, асинхронный UART
	UCSRxC = (1 << UCSZx1) | (1 << UCSZx0);

	uart_input_cb = NULL;
#ifdef USE_STDOUT
	stdout = &uart_output;	// назначаем поток вывода
#endif
#ifdef USE_STDIN
	stdin = &uart_input;	// назначаем поток ввод
#endif
#ifdef USE_STDERR
	stderr = &uart_output;	// назначаем поток вывода ошибок
#endif
}

#ifdef USE_STDOUT
void uart_putchar(uint8_t data, FILE *stream)
#else
void uart_putchar(uint8_t data, void *stream)
#endif
 {
	while (tx_counter == UART_TX_BUFFER_SIZE);	// ждем пока освободиться место в буфере на отправку

	cli(); 	// Запрещаем прерывания
	if (tx_counter || ((UCSRxA & (1<<UDREx))== 0))	// если в данный момент буфер не пустой или идет
	{												// отправка байта,
		uart_tx_buffer[tx_wr_index++] = data;		// то помещаем байт в очередь
#if TX_BUFFER_SIZE != 256
		if (tx_wr_index == UART_TX_BUFFER_SIZE) tx_wr_index = 0;
#endif
		++tx_counter;
	}
	else		// преемопередатчик свободен, можно отправить непосредственно
	UDRx = data;
    sei();	// Разрешаем прерывания
 }


#ifdef USE_STDIN
uint8_t uart_getchar(FILE *stream)
#else
uint8_t uart_getchar()
#endif
{
	uint8_t data;
	while (rx_counter == 0);		// ждем пока появится байт
									// если буфер приема не пуст, то вычитываем байт
	data = uart_rx_buffer[rx_rd_index++];
	#if UART_RX_BUFFER_SIZE_0 != 256
	if (rx_rd_index == UART_RX_BUFFER_SIZE) rx_rd_index=0;
	#endif
	cli();
	--rx_counter;
	sei();
	return data;
}

void uart_printStr_RAM (const char * str)
{
	while (*str != 0)
		uart_putchar(*str++, NULL);
}

void uart_printStr_PM(const char *str)
{
	uint8_t ch=0;
	while ((ch = pgm_read_byte(str++)) != 0)
		uart_putchar(ch, NULL);
}

