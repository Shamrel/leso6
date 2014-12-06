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

#ifndef I2C_H_
#define I2C_H_

// Бит адреса i2c, определяющий чтение/запись
#define I2C_READ	(1)
#define I2C_WRITE   (0)

/**
 \brief  Скорость шины (бит/с).
 */
#define SCL_CLOCK		400000L

/**
 \brief  Статус шины.
 \details Шина свободна, последний обен данными был успешным.
 */
#define I2C_STATUS_READY				(0)

/**
 \brief  Статус шины.
 \details Шина занята, идет передача или прием данных.
 */
#define I2C_STATUS_BUSY					(-1)

/**
 \brief  Статус шины.
 \details Шина свободна, последний обмен данными был с ошибкой.
 */
#define I2C_STATUS_READY_AFT_ERR		(-2)

/**
 \brief Инициализировать интерфейс I2C (TWI).
 \details Данная функция должна быть вызвана перед первым использованием
 функций из текущей библиотеки, или функций, опирающихся на данный интерфейс.
 */
void I2C_Master_Initialise(void);

/**
 \brief Функция инициирует передачу байтового массива данных.
 \details Во время передачи передаваемы массив не должен быть изменен.
 Окончание передачи данных можно узначть с помощью функции I2C_get_status().
 \param *msg Указатель на массив.
 \param msgSize длина массива.
 */
void I2C_send_data( uint8_t *msg, uint8_t msgSize );

/**
 \brief  Возвращает статус приемопередатчика i2c.
 \return I2C_STATUS_READY Шина свободна, последний обмен данными был без ошибок.
 \throw I2C_STATUS_BUSY Шина занята, идет передача или прием данных.
 \throw I2C_STATUS_READY_AFT_ERR Шина свободна, последний обмен данными был с ошибкой.
 */
int8_t I2C_get_status(void);


#endif /* I2C_H_ */
