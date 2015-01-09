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

/*!
  	Библиотека содержит 6 объектов соответствующих аппаратным таймерам

   	 Timer0_obj timer_0;
	 Timer1_obj timer_1;
	 Timer2_obj timer_2;
	 Timer1_obj timer_3;
	 Timer1_obj timer_4;
	 Timer1_obj timer_5;
	 
	 В целях оптимизации, компилироваться должны только те объекты, 
	 которые используются в программе, поэтому чтобы их использовать
	 нужно в файле "platform/timer.h" установить значение макроса 
	 USE_TIMER_X в единицу. Где X - номер используемого таймера
	 Например в программе нужно использовать объект timer_2 и timer_5,
	 тогда в файле "platform/timer.h", должно быть:
\code	 
		#define USE_TIMER_0 0 	
		#define USE_TIMER_1 0 	
		#define USE_TIMER_2 1 // Использование Таймера 1 разрешено 	
		#define USE_TIMER_3 0 	
		#define USE_TIMER_4 0 	
		#define USE_TIMER_5 1 // Использование Таймера 5 разрешено
\endcode
		Неиспользуемые в программе таймера следует запретить, достаточно установить
		USE_TIMER_X в ноль. Это позволит сократить объем программы, и уменьшить 
		использования ОЗУ.
	
	Условная компиляция применяется и для управление ШИМ
\code
		#define USE_PWM_TIMER_0 		0 
		#define USE_PWM_TIMER_1_3_4_5 	1 // Разрешить ШИМ для таймеров 1,3,4,5	
		#define USE_PWM_TIMER_2 		0	
\endcode	
		
	Тогда использовать остальные таймера нельзя так как при компиляции будет
	ошибка. 
	Благодаря такой условной компиляции уменьшается размер кода и использованния ОЗУ.	

	Пример работы с библиотекой

\code
	
	// В файле timer.h макрос USE_TIMER_0 должен быть установлен в 1
	// #define USE_TIMER_0 1
	
     #include <avr/io.h>
     #include "platform/timer.h" 

     ISR(TIMER0_OVF_vect) // Прерывание по переполнению таймера
     {
     }

     int main()
     {
     	timer_0->clear(); // Очистить регистры
     	timer_0->setTimerInterruptFlag(TIMER_OVERFLOW_INT); // Разрешить прерывания по
     	                                                    // переполнению таймера
     	timer_0->start_timer(CLK_DIV_8); // Запуск таймера от (тактовой частоты / 8)
     	sei(); // Разрешить прерывания глобально
     	while(1)
     	{
     	}
     }
\endcode
*/

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <stdint.h>

#define USE_TIMER_0 1 	//!< Разрешить использование таймера 0
#define USE_TIMER_1 1 	//!< Разрешить использование таймера 1
#define USE_TIMER_2 1 	//!< Разрешить использование таймера 2
#define USE_TIMER_3 1 	//!< Разрешить использование таймера 3
#define USE_TIMER_4 1 	//!< Разрешить использование таймера 3
#define USE_TIMER_5 1 	//!< Разрешить использование таймера 4

#define USE_PWM_TIMER_0 		1 	//!< Разрешить использование ШИМ таймера 0
#define USE_PWM_TIMER_1_3_4_5 	1 	//!< Разрешить использование ШИМ таймера 1,3,4,5
#define USE_PWM_TIMER_2 		1	//!< Разрешить использование ШИМ таймера 2

#define CSx_BITS 0x07
#define INT_BITS 0x07

/**
\brief Флаги прерываний таймера
*/
#define TIMER_OVERFLOW_INT 1 	//!< Флаг переполнения таймера
#define TIMER_COMPARE_A_INT 2 	//!< Флаг совпадения с компаратором A
#define TIMER_COMPARE_B_INT 4 	//!< Флаг совпадения с компаратором B
#define TIMER_COMPARE_C_INT 8 	//!< Флаг совпадения с компаратором C (Поддерживает только Timer1,3,4,5)
#define TIMER_CAPTURE_INT 32 	//!< Поддерживает только Timer1,3,4,5
#define TIMER_ALL_INT 255 		//!< Флаг всех прерываний

/**
\brief Макросы для установки значений компараторов
*/
#define SET_OCR0A(x) OCR0A = x 	//!< Установить компаратор A таймера 0
#define SET_OCR0B(x) OCR0B = x 	//!< Установить компаратор A таймера 0
#define SET_OCR1A(x) OCR1A = x 	//!< Установить компаратор A таймера 1
#define SET_OCR1B(x) OCR1B = x 	//!< Установить компаратор B таймера 1
#define SET_OCR1C(x) OCR1C = x 	//!< Установить компаратор C таймера 1
#define SET_OCR2A(x) OCR2A = x 	//!< Установить компаратор A таймера 2
#define SET_OCR2B(x) OCR2B = x 	//!< Установить компаратор B таймера 2
#define SET_OCR3A(x) OCR3A = x 	//!< Установить компаратор A таймера 3
#define SET_OCR3B(x) OCR3B = x 	//!< Установить компаратор B таймера 3
#define SET_OCR3C(x) OCR3C = x 	//!< Установить компаратор C таймера 3
#define SET_OCR4A(x) OCR4A = x 	//!< Установить компаратор A таймера 4
#define SET_OCR4B(x) OCR4B = x 	//!< Установить компаратор B таймера 4
#define SET_OCR4C(x) OCR4C = x 	//!< Установить компаратор C таймера 4
#define SET_OCR5A(x) OCR5A = x 	//!< Установить компаратор A таймера 5
#define SET_OCR5B(x) OCR5B = x 	//!< Установить компаратор B таймера 5
#define SET_OCR5C(x) OCR5C = x 	//!< Установить компаратор C таймера 5

#define TO_INT(x) (uint8_t)x

#define COM0B 4
#define COM0A 6
#define COM0C 2

/**
\brief Параметр запуска таймера
*/
typedef enum CLOCK
{
	NO_CLOCK = 0, 	//!< Таймер не запущен
	CLK_DIV_1, 		//!< Таймер тактируется от CLK/1
	CLK_DIV_8, 		//!< Таймер тактируется от CLK/8
	CLK_DIV_64, 	//!< Таймер тактируется от CLK/64
	CLK_DIV_256, 	//!< Таймер тактируется от CLK/256
	CLK_DIV_1024 	//!< Таймер тактируется от CLK/1024
} CLOCK_t;

/**
\brief Параметры ШИМ
\details Данные режимы не поддерживаются платой LESO6
*/
typedef enum TIMER_MODE
{
	NORMAL_MODE = 0x00, 	//!< Нормальный режим
	PWM_MODE_1, 			//!< TOP 0xFF
	CTC_MODE, 				//!< TOP OCRA
	FAST_PWM_MODE_1, 		//!< TOP 0xFF
	PWM_MODE_2 = 0x05, 		//!< TOP OCRA
	FAST_PWM_MODE_2 = 0x07 	//!< TOP OCRA
} TIMER_MODE_t;

/**
\brief Режимы компараторов
\details Данные режимы не поддерживаются платой LESO6
*/
typedef enum OC_MODE
{
	PORT_DISCONNECT = 0, 	//!< Вывод отключен от компаратора
	PORT_TOGGLE_MODE, 		//!< При срабатывании компаратора вывод меняет свое состояния на противоположное
	PORT_CLEAR_MODE, 		//!< При срабатывании компаратора вывод сбрасывается в лог.0
	PORT_SET_MODE 			//!< При срабатывании компаратора вывод устанавливается в лог.1
} PORT_MODE_t;

/**
\struct Timer0_obj
\brief Структура содержащая указатели на функции
управления таймера 0.
*/
typedef struct Timer0_class
{
	void (*start_timer)(CLOCK_t clk); 						//!< Запуск таймера
	void (*stop_timer)(); 									//!< Остановка таймера
	void (*set_timer_mode)(TIMER_MODE_t mode); 				//!< Установка режима ШИМ
	void (*setTimerInterruptFlag)(uint8_t interrupt); 		//!< Включения прерываний
	void (*clearTimerInterruptFlag)(uint8_t interrupt); 	//!< Выключения прерываний
	uint8_t (*getTimerCNT)(); 								//!< Получить значения счетчика
	uint8_t (*checkTimerInterruptFlag)(uint8_t interrupt); 	//!< Проверка флага прерывания
#if USE_PWM_TIMER_0
	void (*timerPWM_B)(PORT_MODE_t mode); 					//!< Включить ШИМ на вывод 43
	void (*timerPWM_A)(PORT_MODE_t mode); 					//!< Включить ШИМ на вывод 19
#endif
	void (*setCNT)(uint8_t cnt); 							//!< Изменить значение счетчика
	void (*clear)(); 										//!< Очистить регистры
}Timer0_obj;

/**
\struct Timer1_obj
\brief Структура содержащая указатели на функции
управления таймеров 1, 3, 4 и 5.
*/
typedef struct Timer1_class
{
	void (*start_timer)(CLOCK_t clk); 						//!< Запуск таймера
	void (*stop_timer)(); 									//!< Остановка таймера
	void (*set_timer_mode)(TIMER_MODE_t mode); 				//!< Установка режима ШИМ В таймере 1 ШИМ работает в режиме 10-bit
	void (*setTimerInterruptFlag)(uint8_t interrupt); 		//!< Включения прерываний
	void (*clearTimerInterruptFlag)(uint8_t interrupt); 	//!< Выключения прерываний
	uint16_t (*getTimerCNT)();  							//!< Получить значения счетчика
	uint8_t (*checkTimerInterruptFlag)(uint8_t interrupt); 	//!< Проверка флага прерывания
#if	USE_PWM_TIMER_1_3_4_5
	void (*timerPWM_C)(PORT_MODE_t mode); 					//!< ШИМ на выводе 43, только для таймера 1
	void (*timerPWM_B)(PORT_MODE_t mode); 					//!< ШИМ на выводе 42, только для таймера 1
	void (*timerPWM_A)(PORT_MODE_t mode); 					//!< ШИМ на выводе 41, только для таймера 1
#endif
	void (*setCNT)(uint16_t cnt); 							//!< Изменить значение счетчика
	void (*clear)(); 										//!< Очистить регистры
}Timer1_obj;

/**
\struct Timer2_obj
\brief Структура содержащая указатели на функции
управления таймера 2.
*/
typedef struct Timer2_class
{
	void (*start_timer)(CLOCK_t clk); 						//!< Запуск таймера
	void (*stop_timer)();           						//!< Остановка таймера
	void (*set_timer_mode)(TIMER_MODE_t mode); 				//!< Установка режима ШИМ
	void (*setTimerInterruptFlag)(uint8_t interrupt); 		//!< Включения прерываний
	void (*clearTimerInterruptFlag)(uint8_t interrupt); 	//!< Выключения прерываний
	uint8_t (*getTimerCNT)();                      			//!< Получить значения счетчика
	uint8_t (*checkTimerInterruptFlag)(uint8_t interrupt); 	//!< Проверка флага прерывания
#if USE_PWM_TIMER_2
	void (*timerPWM_A)(PORT_MODE_t mode); 					//!< Включить ШИМ на вывод 40
#endif
	void (*async_mode_on)(); 								//!< Включения асинхронного режима от кварца 32 KHz
	void (*async_mode_off)(); 								//!< Выключения асинхронного режима
	void (*setCNT)(uint8_t cnt); 							//!< Изменить значение счетчика
	void (*clear)(); 										//!< Очистить регистры
}Timer2_obj;

#if USE_TIMER_0
/**
\Timer0_obj timer_0
\brief Объект управления таймером 0
*/
extern Timer0_obj timer_0;

#endif

#if USE_TIMER_1
/**
\Timer1_obj timer_1
\brief Объект управления таймером 1
*/
extern Timer1_obj timer_1;

#endif

#if USE_TIMER_2
/**
\Timer2_obj timer_2
\brief Объект управления таймером 2
*/
extern Timer2_obj timer_2;

#endif

#if USE_TIMER_3
/**
\Timer1_obj timer_3
\brief Объект управления таймером 3
*/
extern Timer1_obj timer_3;

#endif

#if USE_TIMER_4
/**
\Timer1_obj timer_4
\brief Объект управления таймером 4
*/
extern Timer1_obj timer_4;

#endif

#if USE_TIMER_5
/**
\Timer1_obj timer_5
\brief Объект управления таймером 5
*/
extern Timer1_obj timer_5;

#endif

#endif /* TIMER_H_ */
