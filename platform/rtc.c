/**
 \file rtc.c
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
#include <stdint.h>

#include "i2c.h"
#include "rtc.h"

#include <avr/io.h>
#include <util/delay.h>


/**
\brief Преобразует число в "binary-coded decimal" формат (BCD). Внутренняя функция.
*/
uint8_t decToBcd(uint8_t val)
{
    return ( (val/10*16) + (val%10) );
}

/**
\brief Преобразует число в формате "binary-coded decimal" в обычную форму. Внутренняя функция.
*/
uint8_t bcdToDec(uint8_t val)
{
    return ( (val/16*10) + (val%16) );
}

uint8_t seTimeDS1338(rtc_data_w_t *rtc_data)
{
	// формируем данные
	rtc_data->i2c_address 	= DS1338 + I2C_WRITE;			// Адресс устройства + метка, что будет запись
	rtc_data->cmd	 		= 0x00;							// Внутренний адрес, с которого начнется запись (0x00 -- Seconds)
	rtc_data->Second		= decToBcd (rtc_data->Second);	// 0-59
	rtc_data->Minute		= decToBcd (rtc_data->Minute);	// 0-59
	rtc_data->Hour			= decToBcd (rtc_data->Hour);	// 1-23
	rtc_data->Day			= decToBcd (rtc_data->Day);		// Sun=1, Mon=2, Tue=3, Wed=4, Thur=5, Fri=6, Sat=7
	rtc_data->Date			= decToBcd (rtc_data->Date);	// 1-28/29/30/31
	rtc_data->Month	  		= decToBcd (rtc_data->Month);	// Jan=1,... Dec=12
	rtc_data->Year			= decToBcd (rtc_data->Year);	// '00 - '99
	rtc_data->Control		= 0;

	// отправляем данные
	I2C_send_data( (uint8_t *)rtc_data, sizeof(rtc_data_w_t));

	return 0;
}

int8_t getTimeDS1338( rtc_data_r_t *rtc_data)
{
    uint8_t I2C_command_buf[ 2 ];

    // Устанавливаем указатель данных в памяти DS на нулевой адресс
	I2C_command_buf[0] = DS1338 + I2C_WRITE; 		// Адресс устройства + метка, что будет запись
	I2C_command_buf[1] = 0x00;                      // Внутренний адресс  0x00 -- регистор "Seconds"

	//if (I2C_Check_Free_After_Stop() == 1 )
	I2C_send_data( (uint8_t *)&I2C_command_buf, 2 );	// передаем указатель

	rtc_data->i2c_address = DS1338 + I2C_READ;		// Адресс устройства + метка, что будет чтение

	//if (I2C_Check_Free_After_Stop() == 1 )

	// Передаем приемопередатчику i2c структуру для отправки,
	// так как у адреса есть метка I2C_READ, после отправления адреса
	// приемопередатчик самостоятельно переходит в режим приема.
	I2C_send_data( (uint8_t *)rtc_data, sizeof(rtc_data_r_t));

	int8_t status;
	while((status = I2C_get_status()) == I2C_STATUS_BUSY);	// Ждем пока отработает приемопередатчик.
	if(status == I2C_STATUS_READY_AFT_ERR)
		return  (-1);			// ошибка в процессе приема данных.

	rtc_data->Second =  bcdToDec( rtc_data->Second & 0x7f );
	rtc_data->Minute =  bcdToDec( rtc_data->Minute & 0x7f );
	rtc_data->Hour   =  bcdToDec( rtc_data->Hour   & 0x3f );
	rtc_data->Day    =  bcdToDec( rtc_data->Day    & 0x07 );
	rtc_data->Date   =  bcdToDec( rtc_data->Date   & 0x3f );
	rtc_data->Month  =  bcdToDec( rtc_data->Month  & 0x1f );
	rtc_data->Year   =  bcdToDec( rtc_data->Year );

	return 0;					// ошибок нет
}

int8_t dateTimeValid(rtc_data_w_t *time)
{
	if (time->Second >60 || time->Minute >60 || time->Hour > 23 ||
			time->Month > 12 || time->Month == 0 || time->Year > 99 || time->Date > 31  )
		return (-1);
	if((time->Month == 2)&&(time->Date > 29))
		return (-1);
	return 0;
}

uint8_t dayWeek( uint8_t D, uint8_t M, uint16_t Y )
{
	int a, y, m, R;
    a = ( 14 - M ) / 12;
    y = Y - a;
    m = M + 12 * a - 2;
    R = 7000 + ( D + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12 );
    return R % 7;
}

