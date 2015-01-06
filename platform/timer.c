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

#include "timer.h"
#include <avr/io.h>

extern void start_timer_0(CLOCK_t clk);
extern void stop_timer_0();
extern void set_timer_0_mode(TIMER_MODE_t mode);
extern uint8_t getTimer0CNT();
extern void setTimer0InterruptFlag(uint8_t interrupt);
extern void clearTimer0InterruptFlag(uint8_t interrupt);
extern uint8_t checkTimer0InterruptFlag(uint8_t interrupt);
extern void timer0PWM_B(PORT_MODE_t mode);
extern void timer0PWM_A(PORT_MODE_t mode);
extern void start_timer_1(CLOCK_t clk);
extern void stop_timer_1();
extern void set_timer_1_mode(TIMER_MODE_t mode);
extern uint16_t getTimer1CNT();
extern void setTimer1InterruptFlag(uint8_t interrupt);
extern void clearTimer1InterruptFlag(uint8_t interrupt);
extern uint8_t checkTimer1InterruptFlag(uint8_t interrupt);
extern void timer1PWM_C(PORT_MODE_t mode);
extern void timer1PWM_B(PORT_MODE_t mode);
extern void timer1PWM_A(PORT_MODE_t mode);
extern void start_timer_2(CLOCK_t clk);
extern void stop_timer_2();
extern void set_timer_2_mode(TIMER_MODE_t mode);
extern uint8_t getTimer2CNT();
extern void setTimer2InterruptFlag(uint8_t interrupt);
extern void clearTimer2InterruptFlag(uint8_t interrupt);
extern uint8_t checkTimer2InterruptFlag(uint8_t interrupt);
extern void timer2PWM_A(PORT_MODE_t mode);
extern void async_mode_on();
extern void async_mode_off();

volatile static Timer0_obj timer_0;
volatile static Timer1_obj timer_1;
volatile static Timer2_obj timer_2;

Timer0_obj *Timer0Init()
{
	timer_0.checkTimerInterruptFlag = checkTimer0InterruptFlag;
	timer_0.clearTimerInterruptFlag = clearTimer0InterruptFlag;
	timer_0.getTimerCNT = getTimer0CNT;
	timer_0.setTimerInterruptFlag = setTimer0InterruptFlag;
	timer_0.set_timer_mode = set_timer_0_mode;
	timer_0.start_timer = start_timer_0;
	timer_0.stop_timer = stop_timer_0;
	timer_0.timerPWM_A = timer0PWM_A;
	timer_0.timerPWM_B = timer0PWM_B;

	return (Timer0_obj*)&timer_0;
}

Timer1_obj *Timer1Init()
{
	timer_1.checkTimerInterruptFlag = checkTimer1InterruptFlag;
	timer_1.clearTimerInterruptFlag = clearTimer1InterruptFlag;
	timer_1.getTimerCNT = getTimer1CNT;
	timer_1.setTimerInterruptFlag = setTimer1InterruptFlag;
	timer_1.set_timer_mode = set_timer_1_mode;
	timer_1.start_timer = start_timer_1;
	timer_1.stop_timer = stop_timer_1;
	timer_1.timerPWM_A = timer1PWM_A;
	timer_1.timerPWM_B = timer1PWM_B;
	timer_1.timerPWM_C = timer1PWM_C;

	return (Timer1_obj*)&timer_1;
}

Timer2_obj *Timer2Init()
{
	timer_2.async_mode_off = async_mode_off;
	timer_2.async_mode_on = async_mode_on;
	timer_2.checkTimerInterruptFlag = checkTimer2InterruptFlag;
	timer_2.clearTimerInterruptFlag = clearTimer2InterruptFlag;
	timer_2.getTimerCNT = getTimer2CNT;
	timer_2.setTimerInterruptFlag = setTimer2InterruptFlag;
	timer_2.clearTimerInterruptFlag = clearTimer2InterruptFlag;
	timer_2.set_timer_mode = set_timer_2_mode;
	timer_2.start_timer = start_timer_2;
	timer_2.stop_timer = stop_timer_2;
	timer_2.timerPWM_A = timer2PWM_A;

	return (Timer2_obj*)&timer_2;
}

void start_timer_0(CLOCK_t clk)
{
	TCCR0B |= TO_INT(clk);
}

void stop_timer_0()
{
	TCCR0B &= ~CSx_BITS;
}

void set_timer_0_mode(TIMER_MODE_t mode)
{
	uint8_t _mode = TO_INT(mode);

	if(_mode <= 3)
	{
		TCCR0A |= _mode & 0x03;
		TCCR0B &= ~(1<<WGM02);
	}
	else
	{
		TCCR0A |= _mode & 0x03;
		TCCR0B |= 1<<WGM02;
	}
}

uint8_t getTimer0CNT()
{
	return TCNT0;
}

void setTimer0InterruptFlag(uint8_t interrupt)
{
	TIMSK0 |= interrupt;
}

void clearTimer0InterruptFlag(uint8_t interrupt)
{
	TIMSK0 &= ~interrupt;
}

uint8_t checkTimer0InterruptFlag(uint8_t interrupt)
{
	uint8_t _int_tmp = 0;
	switch(interrupt)
	{
	case TOV0: _int_tmp = TOV0;  break;
	case OCF0A: _int_tmp = OCF0A; break;
	case OCF0B: _int_tmp = OCF0B; break;
	}

	if((TIFR0&INT_BITS) == _int_tmp)
		return 1;

	return 0;
}

void timer0PWM_B(PORT_MODE_t mode)
{
	DDRG |= (1<<5); // Pin 19 Out mode
	TCCR0A |= (TO_INT(mode))<<COM0B;
}

void timer0PWM_A(PORT_MODE_t mode)
{
	DDRB |= (1<<7); // Pin 43 Out mode
	TCCR0A |= (TO_INT(mode))<<COM0A;
}

void start_timer_1(CLOCK_t clk)
{
	TCCR1B = TO_INT(clk);
}

void stop_timer_1()
{
	TCCR1B &= ~CSx_BITS;
}

void set_timer_1_mode(TIMER_MODE_t mode)
{
	switch(mode)
	{
	case NORMAL_MODE:
		TCCR1A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR1B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_1:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_2:
			TCCR1A |= (1<<WGM11)|(1<<WGM10);
			TCCR1B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case CTC_MODE:
		TCCR1A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR1B &= ~(1<<WGM13);
		TCCR1B |= (1<<WGM12); break;
	case FAST_PWM_MODE_1:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B &= ~(1<<WGM13);
		TCCR1B |= (1<<WGM12); break;
	case FAST_PWM_MODE_2:
			TCCR1A |= (1<<WGM11)|(1<<WGM10);
			TCCR1B &= ~(1<<WGM13);
			TCCR1B |= (1<<WGM12); break;
	default:
		TCCR1A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR1B &= ~((1<<WGM13)|(1<<WGM12)); break;
	}
}

uint16_t getTimer1CNT()
{
	return TCNT1;
}

void setTimer1InterruptFlag(uint8_t interrupt)
{
	TIMSK1 |= interrupt;
}

void clearTimer1InterruptFlag(uint8_t interrupt)
{
	TIMSK1 &= ~interrupt;
}

uint8_t checkTimer1InterruptFlag(uint8_t interrupt)
{
	uint8_t _int_tmp = 0;
	switch(interrupt)
	{
	case TOV1: _int_tmp = TOV1;  break;
	case OCF1A: _int_tmp = OCF1A; break;
	case OCF1B: _int_tmp = OCF1B; break;
	case ICF1: _int_tmp = ICF1; break;
	}

	if((TIFR1&INT_BITS) == _int_tmp)
		return 1;

	return 0;
}

void timer1PWM_C(PORT_MODE_t mode)
{
	DDRB |= 1<<PB7; // Pin 43
	TCCR3A |= (TO_INT(mode))<<COM0C;
}

void timer1PWM_B(PORT_MODE_t mode)
{
	DDRB |= 1<<PB6; // Pin 42
	TCCR3A |= (TO_INT(mode))<<COM0B;
}

void timer1PWM_A(PORT_MODE_t mode)
{
	DDRB |= 1<<PB5; // Pin 41
	TCCR3A |= (TO_INT(mode))<<COM0A;
}

///////////////////////////////////// Timer 2 function //////////////////////////

void start_timer_2(CLOCK_t clk)
{
	TCCR2B |= TO_INT(clk);
}

void stop_timer_2()
{
	TCCR2B &= ~CSx_BITS;
}

void set_timer_2_mode(TIMER_MODE_t mode)
{
	uint8_t _mode = TO_INT(mode);

	if(_mode <= 3)
	{
		TCCR2A |= _mode & 0x03;
		TCCR2B &= ~(1<<WGM02);
	}
	else
	{
		TCCR2A |= _mode & 0x03;
		TCCR2B |= 1<<WGM02;
	}
}

uint8_t getTimer2CNT()
{
	return TCNT2;
}

void setTimer2InterruptFlag(uint8_t interrupt)
{
	TIMSK2 |= interrupt;
}

void clearTimer2InterruptFlag(uint8_t interrupt)
{
	TIMSK2 &= ~interrupt;
}

uint8_t checkTimer2InterruptFlag(uint8_t interrupt)
{
	uint8_t _int_tmp = 0;
	switch(interrupt)
	{
	case TOV2: _int_tmp = TOV2;  break;
	case OCF2A: _int_tmp = OCF2A; break;
	case OCF2B: _int_tmp = OCF2B; break;
	}

	if((TIFR2&INT_BITS) == _int_tmp)
		return 1;

	return 0;
}

void timer2PWM_A(PORT_MODE_t mode)
{
	DDRB |= (1<<PB4); // Pin 40 Out mode
	TCCR2A |= (TO_INT(mode))<<COM0A;
}

void async_mode_on()
{
	ASSR |= (1<<AS2);
}

void async_mode_off()
{
	ASSR &= ~(1<<AS2);
}
