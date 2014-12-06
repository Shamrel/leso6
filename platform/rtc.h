/**
 \file rtc.h
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief Библиотека для работы с часами реального времени (RTC) стенда LESO6
 \details Библиотека содержит функции для работы с часами реального времени
 DS1338. Для работы должна быть инициализирована шина i2c.
 \version   0.1
 \date 30.11.2014
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

#ifndef RTC_H_
#define RTC_H_

/**
\brief Адрес RTC на шине i2c
*/
#define DS1338          0xD0

/**
\brief Определения для CONTROL REGISTER (07H)
*/
#define RTC_CTRL_OUT		(7)
#define RTC_CTRL_OSF		(5)
#define RTC_CTRL_SQWE		(4)
#define RTC_CTRL_RS1		(1)
#define RTC_CTRL_RS0		(0)

/**
 \struct rtc_data_r
 \brief Структура для чтения данных из rtc
 */
typedef struct rtc_data_r
{
	uint8_t		i2c_address;
	uint8_t		Second;
	uint8_t		Minute;
	uint8_t		Hour;			// 1-12, 0-23 (depending on am pm/24 bit 6)
	uint8_t		Day;			// Sun=1, Mon=2, Tue=3, Wed=4, Thur=5, Fri=6, Sat=7
	uint8_t		Date;
	uint8_t		Month;			// Jan=1,... Dec=12
	uint8_t		Year;			// '00 ... '99
	uint8_t		Control;
} rtc_data_r_t;

/**
 \struct rtc_data_r
 \brief Структура для отправки данных в rtc
 */
typedef struct rtc_data_w
{
	uint8_t		i2c_address;
	uint8_t		cmd;			// внутренний адрес
	uint8_t		Second;
	uint8_t		Minute;
	uint8_t		Hour;			// 1-12, 0-23 (depending on am pm/24 bit 6)
	uint8_t		Day;			// Sun=1, Mon=2, Tue=3, Wed=4, Thur=5, Fri=6, Sat=7
	uint8_t		Date;
	uint8_t		Month;			// Jan=1,... Dec=12
	uint8_t		Year;			// '00 ... '99
	uint8_t		Control;
} rtc_data_w_t;

/**
\brief Устанавливаем время
\details Функция приводит время и дату в соответствуие с внутренним форматом DS1338 и
 по шине i2c записывает их во внутренние регистры. Асинхронный вызов.
\param *rtc_data Указатель на соответствующую структуру с временем и датой.
*/
uint8_t seTimeDS1338(rtc_data_w_t *rtc_data);

/**
\brief Получить время и дату из RTC.
\param *rtc_data Указатель на соответствующую структуру с временем и датой.
*/
int8_t getTimeDS1338( rtc_data_r_t *rtc_data);

/**
\brief Проверяет содержимое структуры time на допустимость значений.
\return 0 данные допустимы
\return	-1 данные ошибочны
*/
int8_t dateTimeValid(rtc_data_w_t *rtc_data);

/**
\brief Возвращает номер дня в недели.
\param D день (1 .. 31).
\param M месяц (1 .. 12).
\param Y Год от 1 .. 2013, 2014 ..
\return Номер дня в недели (0 -- понедельник .. 6 -- воскресенье)
*/
uint8_t dayWeek( uint8_t D, uint8_t M, uint16_t Y );


#endif /* RTC_H_ */
