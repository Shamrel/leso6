/**
 \file lcd.c
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief Библиотека для работы с I2C (TWI) ATMEGA128RFA1 стенда LESO6
 \details Библиотека содержит функции для отправки/приема данных
 по двупроводному последовательму интерфейсу i2C, в режиме "Мастер".
 \version   0.1
 \date 1.12.2014
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
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdint.h>

#include "i2c.h"

// I2C pins
#define I2C_PORT			PORTD
#define I2C_PORT_DIR		DDRD
#define I2C_PORT_STATUS		PIND
#define I2C_BIT_SDA			_BV(PD1)
#define I2C_BIT_SCL			_BV(PD0)


/**
 \brief  Определения для статусов. Описаны случаи для работы в режиме "Ведущи" (Master).
 */
#define I2C_START					0x08  //!< Был передат стар-бит.
#define I2C_START_REP				0x10  //!< Старт-бит был передан повторно.
#define I2C_SLAW_ACK				0x18  //!< Передан адрес ведомго на запись (SLA+W), получен ACK.
#define I2C_SLAW_NACK				0x20  //!< Передан адрес ведомго на запись (SLA+W), получен NACK.
#define I2C_TDATA_ACK				0x28  //!< Байт данных был передан успешно, получили ACK.
#define I2C_TDATA_NACK				0x30  //!< Байт данных был передан не успешно, получили NACK.
#define I2C_ARB_LOST				0x38  //!< Потеря SLA+W или байта данных, вызванная ошибкой арбитража.
#define I2C_SLAR_ACK				0x40  //!< Передан адрес ведомго на чтение (SLA+R), получен ACK.
#define I2C_SLAR_NACK				0x48  //!< Передан адрес ведомго на чтение (SLA+R), получен NACK.
#define I2C_RDATA_ACK				0x50  //!< Байт данных принят успешно, отправили ACK.
#define I2C_RDATA_NACK				0x58  //!< Неудалось принять байт, отправили NACK.


/**
 \brief  Число байт для передачи.
 */
static uint8_t I2C_msgSize;

/**
 \brief  Указатель на принимаемый/передаваем массив данных
 */
uint8_t *I2C_buf_ptr;

/**
 \brief Статус последней передачи данных.
 \details  Если последняя передача данных была успешна, то переменная
 хранит 1, если последняя передача неуспешна или еще не завершена, то 0.
 */
uint8_t transOK = 1;

inline uint8_t I2C_Transceiver_Busy(void)
{
  return ( TWCR & (1<<TWIE) );                  	// IF TWI Флаг прерывания не сброшен, значит что-то происходит
  	  	  	  	  	  	  	  	  	  	  	  		// и шина не свободна
}

void I2C_Master_Initialise(void)
{
	// Конфигурируем соответствующие порты ввода вывода
	DDRD &= ~(1<<DDD0 | 1<<DDD1);					// SDA и SCL -- на вход.
	PORTD |= (1<<PD0 | 1<<PD1);						// Поддтягивающий резистр на питание.

    TWSR = 0;                         				// Младшие 2 бита регистра TWSR -- предделитель для скорости шины.
    TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  				// Задаем скорость шины.

	TWDR = 0xff;                               		// Регистр данных, значение по умолчанию.
	TWCR = (1<<TWEN)|                           	// Включаем TWI интерфейс.
		   (0<<TWIE)|(0<<TWINT)|                	// Запрещаем прерывание.
		   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|     	// Не генерируем никаких сигналов.
		   (0<<TWWC);
}

void I2C_send_data( uint8_t *msg, uint8_t msgSize )
{
	while (I2C_Transceiver_Busy());					// Ждем пока приемопередатчик освободится.

	I2C_msgSize = msgSize;							// Число байт данных для передачи.
	I2C_buf_ptr = msg;								// Устанавливаем указатель на начало передаваемого сообщения.
	transOK = 0;									// Статус передачи "незавершена, или заврешена с ошибкой".
	TWCR = (1<<TWEN)|								// Включаем TWI интерфейс.
		   (1<<TWIE)|(1<<TWINT)|					// Разрешаем прерывание и очищаем флаг.
		   (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|			// Initiate a START condition.
		   (0<<TWWC);
}

int8_t I2C_get_status(void)
{
  if(I2C_Transceiver_Busy()) return I2C_STATUS_BUSY;// Шина занята, идет передача или прием данных.
  else if (transOK)	return I2C_STATUS_READY;		// Шина свободна, последний обен данными был успешным.
  	  else return I2C_STATUS_READY_AFT_ERR;			// Шина свободна, последний обмен данными был с ошибкой.
}

// Для разъяснения логики работы смотрите пункт "Master Transmitter Mode" и
// "Master Receiver Mode" документации на ATMEGA128RFA1 (8266F-MCU Wireless-09/14)
ISR(TWI_vect)
{
  static uint8_t I2C_bufPtr;

  switch (TWSR)
  {
  // События при передаче:
    case I2C_START:										// Старт-бит отправлен.
    case I2C_START_REP:									// Старт-бит отправлен повторно.
		I2C_bufPtr = 0;									// Set buffer pointer to the TWI Address location

    case I2C_SLAW_ACK:									// Передади SLA+W успешно (получили ACK).
    case I2C_TDATA_ACK:									// Байт данных передали успешно.
		if(I2C_bufPtr < I2C_msgSize)					// В буфере еще есть данные для отправки.
		{
			TWDR = I2C_buf_ptr[I2C_bufPtr++];
			TWCR = (1<<TWEN)|							// Включаем TWI интерфейс.
				   (1<<TWIE)|(1<<TWINT)|				// Разрешаем прерывание и очищаем флаг.
				   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|		//
				   (0<<TWWC);							//
		}else                    						// Буфер на отправку пуст, передаем стоп-бит.
		{
			transOK = 1;								// Обмен данными успешно завершен. Устанавливаем флаг.
			TWCR = (1<<TWEN)|							// Включаем TWI интерфейс.
				   (0<<TWIE)|(1<<TWINT)|				// Запрещаем прерывание и очищаем флаг.
				   (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|		// Передаем STOP-бит.
				   (0<<TWWC);
		}
		break;
/// Зачем еще раз передавать-то, если облажались?
    case I2C_SLAW_NACK:									// Передали SLA+W, получили NACK. Приемник не ответил.
    case I2C_TDATA_NACK:     							// Передали неуспешно байт данных, получили NACK.
		TWCR = 	(1<<TWEN)|								// Включаем TWI интерфейс.
				(1<<TWIE)|(1<<TWINT)|               	// Разрешаем прерывание и очищаем флаг.
				(1<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|    	// Передаем START-бит.
				(0<<TWWC);
		break;


// События при приеме

    case I2C_RDATA_ACK:									// Байт данных был принят успешно.
    	I2C_buf_ptr[I2C_bufPtr++] = TWDR;

    case I2C_SLAR_ACK:									// Передали успешно SLA+R, Получили ACK.
    	if(I2C_bufPtr < (I2C_msgSize-1))				// Принятый байт не был последним, принимаем следующий.
		{
			TWCR = (1<<TWEN)|							// Включаем TWI интерфейс.
				   (1<<TWIE)|(1<<TWINT)|				// Разрешаем прерывание и очищаем флаг.
				   (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|		// Шлем ACK.
				   (0<<TWWC);
		}else                    						// Далее принимать ничего не нужно. Шлем STOP
		{
			TWCR = (1<<TWEN)|							// Включаем TWI интерфейс.
				   (1<<TWIE)|(1<<TWINT)|				// Разрешаем прерывание и очищаем флаг.
				   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|		// Шлем NACK
				   (0<<TWWC);
		}
		break;

    case I2C_RDATA_NACK:								// Принят последний байт в сообщении, отправили NACK.
    	I2C_buf_ptr[I2C_bufPtr] = TWDR;
    	transOK = 1;									// Set status bits to completed successfully.
		TWCR = (1<<TWEN)|								// Включаем TWI интерфейс.
			   (0<<TWIE)|(1<<TWINT)|					// Запрещаем прерывание и очищаем флаг.
			   (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|			// Шлем STOP-бит.
			   (0<<TWWC);
		break;
////// может быть тут запретить прерывание?
    case I2C_SLAR_NACK:      							// Передали SLA+R, ведомый не ответил.
 		TWCR = 	(1<<TWEN)|								// Включаем TWI интерфейс.
				(1<<TWIE)|(1<<TWINT)|               	// Разрешаем прерывание и очищаем флаг.
				(1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|    	// Шлем STOP-бит.
				(0<<TWWC);
		break;

    default:
  		TWCR = (1<<TWEN)|								// Включаем TWI интерфейс.
			 (1<<TWIE)|(1<<TWINT)|						// Разрешаем прерывание и очищаем флаг.
			 (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|			// Шлем STOP-бит.
			 (0<<TWWC);
      	break;
  }
}
