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

#ifndef UART_H_
#define UART_H_
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>

/**
 \brief  Скорость UART (бит/с)
 */
#define BAUD 115200
#include <util/setbaud.h>

/*************************************************************************/
/**
 Настройка модуля
 */

/**
 \brief  Если UART_1 определен, то в модуле исполь usart1, иначе usart0
 */
#define UART_1

/**
 \brief  Размер буфера на прием
 */
#define UART_RX_BUFFER_SIZE 16

/**
 \brief  Размер буфера на передачу
 */
#define UART_TX_BUFFER_SIZE 128

/**
 \brief  Использование потока stdout <stdio.h>
 \details Опция настраивает стандартный поток вывода stdout на работу с uart
 Опция нужна для работы таких функций как putchar(), printf() из библитеки <stdio.h>
 */
#define USE_STDOUT

/**
 \brief  Использование потока stdin <stdio.h>
 \details Опция настраивает стандартный поток ввода stdin на работу с uart.
 Опция нужна для работы таких функций как getchar(), scanf() из библитеки <stdio.h>
 */
#define USE_STDIN

/**
 \brief  Использование потока stderr <stdio.h>
 \details Опция настраивает стандартный поток вывода stderr на работу с uart.
 */
#define USE_STDERR
/*************************************************************************/

/**
\brief Инициализация приемопередатчика uart
\details Данная функция должна быть вызвана до любой другой функции
из этого модуля.
*/
void uart_init(void);

/**
\brief Отправляем байт данных
\details Если приемопередатчик UASRT свободен, то помещаем байт в регистр передатчика,
если приемопередатчик в данный момент передает данные или имеются байты в очереде,
то помещаем байт в очередь. Если очередь заполнена, то ждем пока в ней не появится место.
\param data Байт для отправки.
\param *stream Поток ввода-вывода. Если не используется, вызывать со значением NULL
*/
#ifdef USE_STDOUT
void uart_putchar(uint8_t data, FILE *stream);
#else
void uart_putchar(uint8_t data, void *stream);
#endif


/**
\brief Получить байт данных
\details Если приемопередатчик UASRT свободен, то помещаем байт в регистр передатчика,
если приемопередатчик в данный момент передает данные или имеются байты в очереде,
то помещаем байт в очередь. Если очередь заполнена, то ждем пока в ней не появится место.
\return Байт данных.
\param *stream Поток ввода-вывода. Если не используется, вызывать со значением NULL
*/
#ifdef USE_STDIN
uint8_t uart_getchar(FILE *stream);
#else
uint8_t uart_getchar();
#endif

/**
\brief Устанавливает функцию обратного вызова(Callback)
\param Указатель на функцию соответствующего типа.
*/
void uart_set_input_cb(void (*input)( uint8_t c));

/**
\brief Отправить строку.
\details Функция выводит строку через UART. Передоваемое сообщение
содержится в ОЗУ. Использовать для вывода переменных.
\param * str – указатель на строку
*/
void uart_printStr_RAM (const char * str);

/**
\brief Отправить строку из FLASH.
\details Функция выводит строку через UART. Передоваемое сообщение
содержится в FLASH. Использовать для вывода строковых констант.
\param * str – указатель на строку в памяти программ.
Использовать совместно с макросом PSTR()
\code{.c} uart_printStr(PSTR("Hello World!\r\n"));
\endcode
*/
void uart_printStr_PM(const char *str);

/**
\brief Макрос для вывода строки из FLASH.
\code{.c} uart_printStr("Hello World!\r\n");
\endcode
*/
#define uart_printStr(...) uart_printStr_PM(PSTR(__VA_ARGS__))


#ifdef UART_1
#define UBRRxH					UBRR1H
#define UBRRxL					UBRR1L
#define UCSRxA					UCSR1A
#define UDREx					UDRE1
#define RXCx 					RXC1
#define U2Xx					U2X1
#define UCSRxB					UCSR1B
#define UCSRxC					UCSR1C
#define UDRx					UDR1
#define FEx						FE1
#define UPEx					UPE1
#define DORx					DOR1
#define RXENx					RXEN1
#define TXENx					TXEN1
#define RXCIEx					RXCIE1
#define TXCIEx					TXCIE1
#define UCSZx0					UCSZ10
#define UCSZx1					UCSZ11
#define UART_TX_COMPLETE_IRQ	USART1_TX_vect
#define UART_RX_IRQ 			USART1_RX_vect
#else
#define UBRRxH					UBRR0H
#define UBRRxL					UBRR0L
#define UCSRxA					UCSR0A
#define UDREx					UDRE0
#define RXCx 					RXC0
#define U2Xx					U2X0
#define UCSRxB					UCSR0B
#define UCSRxC					UCSR0C
#define UDRx					UDR0
#define FEx						FE0
#define UPEx					UPE0
#define DORx					DOR0
#define RXENx					RXEN0
#define TXENx					TXEN0
#define RXCIEx					RXCIE0
#define TXCIEx					TXCIE0
#define UCSZx0					UCSZ00
#define UCSZx1					UCSZ01
#define UART_TX_COMPLETE_IRQ	USART0_TX_vect
#define UART_RX_IRQ 			USART0_RX_vect
#endif


#endif /* UART_H_ */
