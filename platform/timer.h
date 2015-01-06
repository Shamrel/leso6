/**
\file timer.c
\author Ryasanov Ilya <ryasanov@gmail.com> www.labfor.ru
\brief Библиотека для работы с таймерами
\details Библиотека содержит функции для работы с таймерами
микроконтроллера atmega128rfa1
\version 0.1
\date 06.01.2015
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

/*
 * Пример работы с библиотекой
 *
 * 	Timer0_obj *tim0 = Timer0Init(); // Получение укателя на объект
	tim0->setTimerInterruptFlag(TIMER_OVERFLOW_INT); // Вызов функций осуществляется через
													 //	указатель
	tim0->start_timer(CLK_DIV_8); // Запуск таймера от (тактовой частоты / 8)
	sei(); // Разрешить прерывания глобально

 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <stdint.h>

#define CSx_BITS 0x07
#define INT_BITS 0x07

/**
\brief Флаги прерываний таймера
*/
#define TIMER_OVERFLOW_INT 1 // Флаг переполнения таймера
#define TIMER_COMPARE_A_INT 2 // Флаг совпадения с компаратором A
#define TIMER_COMPARE_B_INT 4 // Флаг совпадения с компаратором B
#define TIMER_COMPARE_C_INT 8 // Флаг совпадения с компаратором B
							  // Поддерживает только Timer1
#define TIMER_CAPTURE_INT 32 // Поддерживает только Timer 1

#define SET_OCR0A(x) OCR0A = x // Установить компаратор A таймера 0
#define SET_OCR0B(x) OCR0B = x // Установить компаратор A таймера 0
#define SET_OCR1A(x) OCR1A = x // Установить компаратор A таймера 1
#define SET_OCR1B(x) OCR1B = x // Установить компаратор B таймера 1
#define SET_OCR1C(x) OCR1C = x // Установить компаратор C таймера 1
#define SET_OCR2A(x) OCR2A = x // Установить компаратор A таймера 2
#define SET_OCR2B(x) OCR2B = x // Установить компаратор B таймера 2

#define TO_INT(x) (uint8_t)x

#define COM0B 4
#define COM0A 6
#define COM0C 2

#define TIMER_0 0 // Timer 0 8 bit
#define TIMER_1 1 // Timer 1 16 bit
#define TIMER_2 2 // Timer 2 8 bit

/**
\brief Параметр запуска таймера
*/
typedef enum CLOCK
{
	NO_CLOCK = 0, // Таймер не запущен
	CLK_DIV_1, // Таймер тактируется от CLK/1
	CLK_DIV_8, // Таймер тактируется от CLK/8
	CLK_DIV_64, // Таймер тактируется от CLK/64
	CLK_DIV_256, // Таймер тактируется от CLK/256
	CLK_DIV_1024 // Таймер тактируется от CLK/1024
} CLOCK_t;

/**
\brief Параметры ШИМ
\details Данные режимы не поддерживаются платой LESO6
*/
typedef enum TIMER_MODE
{
	NORMAL_MODE = 0x00,
	PWM_MODE_1, // TOP 0xFF
	CTC_MODE, // TOP OCRA
	FAST_PWM_MODE_1, // TOP 0xFF
	PWM_MODE_2 = 0x05, // TOP OCRA
	FAST_PWM_MODE_2 = 0x07 // TOP OCRA
} TIMER_MODE_t;

/**
\brief Режимы компараторов
\details Данные режимы не поддерживаются платой LESO6
*/
typedef enum OC_MODE
{
	PORT_DISCONNECT = 0, // Вывод отключен от компаратора
	PORT_TOGGLE_MODE, // При срабатывании компаратора вывод
						// меняет свое состояния на противоположное
	PORT_CLEAR_MODE, // При срабатывании компаратора вывод сбрасывается в лог.0
	PORT_SET_MODE // При срабатывании компаратора вывод устанавливается в лог.1
} PORT_MODE_t;

/**
\struct Timer0_obj
\brief Структура содержащая указатели на функции
управления таймера 0.
\details Перед использованием необходимо
вызвать функцию инициализации Timer0Init.
*/
typedef struct Timer0_class
{
	void (*start_timer)(CLOCK_t clk); // Запуск таймера
	void (*stop_timer)(); // Остановка таймера
	void (*set_timer_mode)(TIMER_MODE_t mode); // Установка режима ШИМ
	void (*setTimerInterruptFlag)(uint8_t interrupt); // Включения прерываний
	void (*clearTimerInterruptFlag)(uint8_t interrupt); // Выключения прерываний
	uint8_t (*getTimerCNT)(); // Получить значения счетчика
	uint8_t (*checkTimerInterruptFlag)(uint8_t interrupt); // Проверка флага прерывания
	void (*timerPWM_B)(PORT_MODE_t mode); // Включить ШИМ на вывод 43
	void (*timerPWM_A)(PORT_MODE_t mode); // Включить ШИМ на вывод 19
}Timer0_obj;

/**
\struct Timer1_obj
\brief Структура содержащая указатели на функции
управления таймера 1.
\details Перед использованием необходимо
вызвать функцию инициализации Timer1Init.
*/
typedef struct Timer1_class
{
	void (*start_timer)(CLOCK_t clk); // Запуск таймера
	void (*stop_timer)(); // Остановка таймера
	void (*set_timer_mode)(TIMER_MODE_t mode); // Установка режима ШИМ
										// В таймере 1 ШИМ работает в режиме 10-bit
	void (*setTimerInterruptFlag)(uint8_t interrupt); // Включения прерываний
	void (*clearTimerInterruptFlag)(uint8_t interrupt); // Выключения прерываний
	uint16_t (*getTimerCNT)();  // Получить значения счетчика
	uint8_t (*checkTimerInterruptFlag)(uint8_t interrupt); // Проверка флага прерывания
	void (*timerPWM_C)(PORT_MODE_t mode); // Включить ШИМ на вывод 43
	void (*timerPWM_B)(PORT_MODE_t mode); // Включить ШИМ на вывод 42
	void (*timerPWM_A)(PORT_MODE_t mode); // Включить ШИМ на вывод 41
}Timer1_obj;

/**
\struct Timer2_obj
\brief Структура содержащая указатели на функции
управления таймера 2.
\details Перед использованием необходимо
вызвать функцию инициализации Timer2Init.
*/
typedef struct Timer2_class
{
	void (*start_timer)(CLOCK_t clk); // Запуск таймера
	void (*stop_timer)();           // Остановка таймера
	void (*set_timer_mode)(TIMER_MODE_t mode); // Установка режима ШИМ
	void (*setTimerInterruptFlag)(uint8_t interrupt); // Включения прерываний
	void (*clearTimerInterruptFlag)(uint8_t interrupt); // Выключения прерываний
	uint8_t (*getTimerCNT)();                      // Получить значения счетчика
	uint8_t (*checkTimerInterruptFlag)(uint8_t interrupt); // Проверка флага прерывания
	void (*timerPWM_A)(PORT_MODE_t mode); // Включить ШИМ на вывод 40
	void (*async_mode_on)(); // Включения асинхронного режима от кварца 32 KHz
	void (*async_mode_off)(); // Выключения асинхронного режима
}Timer2_obj;

/**
\Timer0_obj *Timer0Init()
\brief Функция инициализации для таймера 0
\details Необходимо вызвать перед использованием.
Возвращает указатель на структуру соответствующую
таймеру 0.
*/
Timer0_obj *Timer0Init();

/**
\Timer1_obj *Timer1Init()
\brief Функция инициализации для таймера 1
\details Необходимо вызвать перед использованием.
Возвращает указатель на структуру соответствующую
таймеру 1.
*/
Timer1_obj *Timer1Init();

/**
\Timer2_obj *Timer2Init()
\brief Функция инициализации для таймера 2
\details Необходимо вызвать перед использованием.
Возвращает указатель на структуру соответствующую
таймеру 2.
*/
Timer2_obj *Timer2Init();

#endif /* PLATFORM_TIMER_H_ */
