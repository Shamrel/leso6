/**
 \file lcd.c
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \brief Библиотека для работы с LCD стенда LESO6
 \details Библиотека содержит функции для работы с символьным жидкокристаллическим
 дисплеем. Контроллер ЖКИ поддерживает команды HD44780
 \version   0.1
 \date 26.11.2014
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

#include "gpio.h"
#include "lcd.h"

#define LCD_COLS			(8)
#define LCD_ROWS			(2)

#define	LCD_CLEAR			(0x01)
#define	LCD_HOME			(0x02)
#define	LCD_ENTRY			(0x04)
#define	LCD_CTRL			(0x08)
#define	LCD_CDSHIFT			(0x10)
#define	LCD_FUNC			(0x20)
#define	LCD_CGRAM			(0x40)
#define	LCD_DGRAM			(0x80)

#define	LCD_BLINK_ON		0x01
#define	LCD_CURSOR_ON		0x02
#define	LCD_DISPLAY_ON		0x04

#define	LCD_ENTRY_SH		0x01
#define	LCD_ENTRY_ID		0x02

#define	LCD_FUNC_F	0x04
#define	LCD_FUNC_N	0x08
#define	LCD_FUNC_DL	0x10

#define	LCD_CDSHIFT_RL	0x04

/**
 \brief  Смещения адреса для строк.
 */
static const uint8_t rowOff [4] = { 0x00, 0x40, 0x14, 0x54 } ;

#define  RS    	PORTB, 5, H
#define  RW    	PORTB, 6, H
#define  E    	PORTB, 7, H

#define DATA	PORTE

/**
\brief Инициализируем порты ввода/вывода. Внутренняя функция.
*/
static inline void initGPIO()
{
	DDRE = 0xFF;
	DDRB |= (1<<DDB5 | 1<<DDB6 | 1<<DDB7);
}

/**
\brief Фурмирует импульс на выводе E. Внутренняя функция.
*/
static inline
void strobe ()
{
	on(E);
	_delay_us(20);
	off(E);
	_delay_us(20);
}

/**
\brief Посылаем команду контроллеру LCD. Внутренняя функция.
\details Устанавливаем выводы RS и RW  в ноль, на линиях данных устанавливаем
код команды, посылаем строб. Ждем 2 мс пока контроллер выполнит внутренние действия.
\param command код команды.
*/
static inline void putCommand (uint8_t command)
{
	off(RS);
	off(RW);
	DATA =  command;
	strobe();
	_delay_us(50);
}

/**
\brief Включаем экран. Внутренняя функция.
\param lcd указатель на структуру с описанием lcd.
\param lcdOn 1 -- включить, 0 -- выключить.
*/
void lcdDispOn (lcd_t *lcd, uint8_t lcdOn)
{
	if(lcdOn) lcd->lcdContrl |= LCD_DISPLAY_ON;
	else lcd->lcdContrl &= ~LCD_DISPLAY_ON;
	putCommand (LCD_CTRL | lcd->lcdContrl) ;
}

void lcdHome (lcd_t *lcd)
{
  putCommand (LCD_HOME);
  lcd->cx = lcd->cy = 0;
  _delay_ms(2);				// команда требует дополнительного времени
}


void lcdClear (lcd_t *lcd)
{

  putCommand (LCD_CLEAR);
  _delay_ms(2);				// команда требует дополнительного времени
  putCommand (LCD_HOME);
  _delay_ms(2);				// команда требует дополнительного времени
  lcd->cx = lcd->cy = 0 ;
}


void lcdCursor (lcd_t *lcd, uint8_t cursorOn)
{
	if(cursorOn) lcd->lcdContrl |= LCD_CURSOR_ON;
	else lcd->lcdContrl &= ~LCD_CURSOR_ON;
	putCommand (LCD_CTRL | lcd->lcdContrl) ;
}

void lcdCursorBlink (lcd_t *lcd, uint8_t cursorBlinkOn)
{
	if(cursorBlinkOn) lcd->lcdContrl |= LCD_BLINK_ON;
	else lcd->lcdContrl &= ~LCD_BLINK_ON;
	putCommand (LCD_CTRL | lcd->lcdContrl) ;
}

void lcdPutchar (lcd_t *lcd, char ch)
{
	// Проверяем на управляющие символы.
	if(ch == '\r')
	{
		lcd->cx = 0; 		// Возврат каретки в начало строки.
		putCommand(lcd->cx + (LCD_DGRAM | rowOff [lcd->cy]));
		return;
	} else if(ch == '\n')
	{
		lcd->cx = 0;				// начинаем с новой строки
		if (++lcd->cy == lcd->rows)	// закончились строки
			lcd->cy = 0 ;
		putCommand(lcd->cx + (LCD_DGRAM | rowOff [lcd->cy]));
		return;
	}
	// Выводим символ.
	on(RS);
	off(RW);
	DATA =  ch;
	strobe();
	lcd->cx++;						// Увеличиваем позицию.
//
//	if(++lcd->cx == lcd->cols)		// закончились символы в строке
//	{
//		lcd->cx = 0;				// начинаем с новой строки
//		if (++lcd->cy == lcd->rows)	// закончились строки
//			lcd->cy = 0 ;
//		putCommand(lcd->cx + (LCD_DGRAM | rowOff [lcd->cy]));
//	}
}

void lcdPuts (lcd_t *lcd, const char *string)
{
	while (*string)
		lcdPutchar (lcd, *string++) ;
}

uint8_t lcdPosition (lcd_t *lcd, uint8_t x, uint8_t y)
{
	if ((x > lcd->cols) || (y > lcd->rows))
		return (-1);
	putCommand (x + (LCD_DGRAM | rowOff [y])) ;
	lcd->cx = x ;
	lcd->cy = y ;
	return (0);
}


void lcdCharDef (lcd_t *lcd, uint8_t index, uint8_t *data)
{
	uint8_t i ;
	putCommand (LCD_CGRAM | ((index & 7) << 3)) ;	//устанавливаем адресс DGRAM
	on(RS);
	off(RW);
	for (i = 0 ; i < 8 ; ++i)		// передаем данные в DGRAM
	{
		DATA = data[i];
		strobe();
	}
	putCommand (lcd->cx + (LCD_DGRAM | rowOff [lcd->cy])) ; // возвращаем курсор в предыдущее место
}


void lcdInit(lcd_t *lcd)
{
	initGPIO();
	lcd->cols = LCD_COLS;
	lcd->rows = LCD_ROWS;
	lcd->lcdContrl = 0;

	_delay_ms(10);
	_delay_ms(10);
	_delay_ms(10);
	_delay_ms(10);

	putCommand(LCD_FUNC | LCD_FUNC_DL) ;
	_delay_ms (5);
	putCommand(LCD_FUNC | LCD_FUNC_DL) ;
	_delay_ms (1);
	putCommand(LCD_FUNC | LCD_FUNC_DL) ;
	_delay_ms (1);

	putCommand(LCD_FUNC | LCD_FUNC_DL | LCD_FUNC_N) ;
	_delay_ms (1);


	lcdDispOn(lcd, 1);

	lcdDispOn(lcd, 1);
	lcdCursor(lcd, 1);
	lcdCursorBlink(lcd, 1);


	putCommand(LCD_ENTRY   | LCD_ENTRY_ID);
	//putCommand(LCD_CDSHIFT | LCD_CDSHIFT_RL);
	lcdClear(lcd);
}















