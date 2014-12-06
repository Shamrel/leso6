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

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>


/**
 \struct lcd_t
 \brief Структура для описания LCD.
 */
typedef struct lcd
{
	uint8_t rows;				//!< Количество строк
	uint8_t cols;				//!< Количество столбцов
	uint8_t cx, cy;				//!< Текущая позиция курсора
	uint8_t lcdContrl;			//!< Состояние индикатора
}lcd_t;


/**
\brief Инициализация ЖКИ.
\details Производит первичную настройку контроллера ЖКИ, включает дисплей,
заполняет структуру lcd.
\param lcd Указатель на структуру с описанием lcd.
*/
void lcdInit(lcd_t *lcd);

/**
\brief Ощищаем экран
\details Устанавливает адрес DDRAM в ноль, возвращает курсор в его начальную позицию
Во все ячейки DDRAM записываем код символа "пробел" - 0x20.
\param lcd указатель на структуру сописанием lcd.
*/
void lcdClear (lcd_t *lcd);

/**
\brief Возвращает курсор в начало экрана
\details Устанавливает адрес DDRAM в ноль, возвращает курсор в его начальную позицию
При этом содержимое DDRAM не изменяется
\param lcd указатель на структуру с описанием lcd.
*/
void lcdHome (lcd_t *lcd);

/**
\brief Устанавливает курсор в заданную позицию.
\param lcd Указатель на структуру с описанием lcd.
\param x Позиция в строке.
\param y Номер строки.
\throw -1 Указанная позиция выходит за предел экрана.
*/
uint8_t lcdPosition (lcd_t *lcd, uint8_t x, uint8_t y);

/**
\brief Выводим символ на экран
\details Учитывает позиция курсора, автоматический перевод строки.
\param lcd Указатель на структуру с описанием lcd.
\param ch Код выводимого символа
*/

void lcdPutchar (lcd_t *lcd, char ch);

/**
\brief Выводим на экран строку
\details Учитывает позиция курсора, автоматический перевод строки.
\param lcd Указатель на структуру с описанием lcd.
\param string Указатель на выводимую строку
*/
void lcdPuts (lcd_t *lcd, const char *string);


/**
\brief Определяем новый символ
\details В память DGRAM записываем очертания нового символа.
\param lcd Указатель на структуру с описанием lcd.
\param index Код нового символа (от 0 до 7)
\param data Указатель на массив из 8-ими байт, в котором содержиться
изображение символа. Например:
uint8_t ch[8] =
	{
		0x1F,	//x x x 1 1 1 1 1
		0x0E,	//x x x 0 1 1 1 0
		0x0E,	//x x x 0 1 1 1 0
		0x04,	//x x x 0 0 1 0 0
		0x04,	//x x x 0 0 1 0 0
		0x0A,	//x x x 0 1 0 1 0
		0x11,	//x x x 1 0 0 0 1
		0x1F	//x x x 1 1 1 1 1
	};
*/
void lcdCharDef (lcd_t *lcd, uint8_t index, uint8_t *data);

/**
\brief Включаем отображение курсора
\param lcd указатель на структуру с описанием lcd.
\param cursorOn 1 -- включить, 0 -- выключить.
*/
void lcdCursor (lcd_t *lcd, uint8_t cursorOn);

/**
\brief Включаем мигание курсора
\param lcd указатель на структуру с описанием lcd.
\param cursorBlinkOn 1 -- включить, 0 -- выключить.
*/
void lcdCursorBlink (lcd_t *lcd, uint8_t cursorBlinkOn);


#endif /* LCD_H_ */
