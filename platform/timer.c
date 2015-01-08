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

// Timer 0
void start_timer_0(CLOCK_t clk);
void stop_timer_0();
void set_timer_0_mode(TIMER_MODE_t mode);
uint8_t getTimer0CNT();
void setTimer0InterruptFlag(uint8_t interrupt);
void clearTimer0InterruptFlag(uint8_t interrupt);
uint8_t checkTimer0InterruptFlag(uint8_t interrupt);
void timer0PWM_B(PORT_MODE_t mode);
void timer0PWM_A(PORT_MODE_t mode);
void setCNT_Timer0(uint8_t cnt);
void clear_reg_0();

// Timer 1
void start_timer_1(CLOCK_t clk);
void stop_timer_1();
void set_timer_1_mode(TIMER_MODE_t mode);
uint16_t getTimer1CNT();
void setTimer1InterruptFlag(uint8_t interrupt);
void clearTimer1InterruptFlag(uint8_t interrupt);
uint8_t checkTimer1InterruptFlag(uint8_t interrupt);
void timer1PWM_C(PORT_MODE_t mode);
void timer1PWM_B(PORT_MODE_t mode);
void timer1PWM_A(PORT_MODE_t mode);
void setCNT_Timer1(uint16_t cnt);
void clear_reg_1();

// Timer 2
void start_timer_2(CLOCK_t clk);
void stop_timer_2();
void set_timer_2_mode(TIMER_MODE_t mode);
uint8_t getTimer2CNT();
void setTimer2InterruptFlag(uint8_t interrupt);
void clearTimer2InterruptFlag(uint8_t interrupt);
uint8_t checkTimer2InterruptFlag(uint8_t interrupt);
void timer2PWM_A(PORT_MODE_t mode);
void async_mode_on();
void async_mode_off();
void setCNT_Timer2(uint8_t cnt);
void clear_reg_2();

// Timer 3
void start_timer_3(CLOCK_t clk);
void stop_timer_3();
void set_timer_3_mode(TIMER_MODE_t mode);
uint16_t getTimer3CNT();
void setTimer3InterruptFlag(uint8_t interrupt);
void clearTimer3InterruptFlag(uint8_t interrupt);
uint8_t checkTimer3InterruptFlag(uint8_t interrupt);
void timer3PWM_C(PORT_MODE_t mode);
void timer3PWM_B(PORT_MODE_t mode);
void timer3PWM_A(PORT_MODE_t mode);
void setCNT_Timer3(uint16_t cnt);
void clear_reg_3();

// Timer 4
void start_timer_4(CLOCK_t clk);
void stop_timer_4();
void set_timer_4_mode(TIMER_MODE_t mode);
uint16_t getTimer4CNT();
void setTimer4InterruptFlag(uint8_t interrupt);
void clearTimer4InterruptFlag(uint8_t interrupt);
uint8_t checkTimer4InterruptFlag(uint8_t interrupt);
void timer4PWM_C(PORT_MODE_t mode);
void timer4PWM_B(PORT_MODE_t mode);
void timer4PWM_A(PORT_MODE_t mode);
void setCNT_Timer4(uint16_t cnt);
void clear_reg_4();

// Timer 5
void start_timer_5(CLOCK_t clk);
void stop_timer_5();
void set_timer_5_mode(TIMER_MODE_t mode);
uint16_t getTimer5CNT();
void setTimer5InterruptFlag(uint8_t interrupt);
void clearTimer5InterruptFlag(uint8_t interrupt);
uint8_t checkTimer5InterruptFlag(uint8_t interrupt);
void timer5PWM_C(PORT_MODE_t mode);
void timer5PWM_B(PORT_MODE_t mode);
void timer5PWM_A(PORT_MODE_t mode);
void setCNT_Timer5(uint16_t cnt);
void clear_reg_5();

Timer0_obj timer_0 = {
	start_timer_0,
	stop_timer_0,
	set_timer_0_mode,
	setTimer0InterruptFlag,
	clearTimer0InterruptFlag,
	getTimer0CNT,
	checkTimer0InterruptFlag,
	timer0PWM_B,
	timer0PWM_A,
	setCNT_Timer0,
	clear_reg_0
};

Timer1_obj timer_1 = {
	start_timer_1,
	stop_timer_1,
	set_timer_1_mode,
	setTimer1InterruptFlag,
	clearTimer1InterruptFlag,
	getTimer1CNT,
	checkTimer1InterruptFlag,
	timer1PWM_C,
	timer1PWM_B,
	timer1PWM_A,
	setCNT_Timer1,
	clear_reg_1
};

Timer2_obj timer_2 = {
	start_timer_2,
	stop_timer_2,
	set_timer_2_mode,
	setTimer2InterruptFlag,
	clearTimer2InterruptFlag,
	getTimer2CNT,
	checkTimer2InterruptFlag,
	timer2PWM_A,
	async_mode_on,
	async_mode_off,
	setCNT_Timer2,
	clear_reg_2
};

Timer1_obj timer_3 = {
	start_timer_3,
	stop_timer_3,
	set_timer_3_mode,
	setTimer3InterruptFlag,
	clearTimer3InterruptFlag,
	getTimer3CNT,
	checkTimer3InterruptFlag,
	timer3PWM_C,
	timer3PWM_B,
	timer3PWM_A,
	setCNT_Timer3,
	clear_reg_3
};

Timer1_obj timer_4 = {
	start_timer_4,
	stop_timer_4,
	set_timer_4_mode,
	setTimer4InterruptFlag,
	clearTimer4InterruptFlag,
	getTimer4CNT,
	checkTimer4InterruptFlag,
	timer4PWM_C,
	timer4PWM_B,
	timer4PWM_A,
	setCNT_Timer4,
	clear_reg_4
};

Timer1_obj timer_5 = {
	start_timer_5,
	stop_timer_5,
	set_timer_5_mode,
	setTimer5InterruptFlag,
	clearTimer5InterruptFlag,
	getTimer5CNT,
	checkTimer5InterruptFlag,
	timer5PWM_C,
	timer5PWM_B,
	timer5PWM_A,
	setCNT_Timer5,
	clear_reg_5
};

// ========================================================================
//                         Timer 0

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

void setCNT_Timer0(uint8_t cnt)
{
	TCNT0 = cnt;
}

void clear_reg_0()
{
	TCNT0 = 0;
	TCCR0A = 0;
	TCCR0B = 0;
	TIMSK0 = 0;
}

// ========================================================================
//                         Timer 1

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
	TCCR1A |= (TO_INT(mode))<<COM0C;
}

void timer1PWM_B(PORT_MODE_t mode)
{
	DDRB |= 1<<PB6; // Pin 42
	TCCR1A |= (TO_INT(mode))<<COM0B;
}

void timer1PWM_A(PORT_MODE_t mode)
{
	DDRB |= 1<<PB5; // Pin 41
	TCCR1A |= (TO_INT(mode))<<COM0A;
}

void setCNT_Timer1(uint16_t cnt)
{
	TCNT1 = cnt;
}

void clear_reg_1()
{
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK1 = 0;
}


// ========================================================================
//                         Timer 2

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

void setCNT_Timer2(uint8_t cnt)
{
	TCNT2 = cnt;
}

void clear_reg_2()
{
	TCNT2 = 0;
	TCCR2A = 0;
	TCCR2B = 0;
	TIMSK2 = 0;
}


// ========================================================================
//                         Timer 3

void start_timer_3(CLOCK_t clk)
{
	TCCR3B = TO_INT(clk);
}

void stop_timer_3()
{
	TCCR3B &= ~CSx_BITS;
}

void set_timer_3_mode(TIMER_MODE_t mode)
{
	switch(mode)
	{
	case NORMAL_MODE:
		TCCR3A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR3B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_1:
		TCCR3A |= (1<<WGM11)|(1<<WGM10);
		TCCR3B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_2:
			TCCR3A |= (1<<WGM11)|(1<<WGM10);
			TCCR3B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case CTC_MODE:
		TCCR3A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR3B &= ~(1<<WGM13);
		TCCR3B |= (1<<WGM12); break;
	case FAST_PWM_MODE_1:
		TCCR3A |= (1<<WGM11)|(1<<WGM10);
		TCCR3B &= ~(1<<WGM13);
		TCCR3B |= (1<<WGM12); break;
	case FAST_PWM_MODE_2:
			TCCR3A |= (1<<WGM11)|(1<<WGM10);
			TCCR3B &= ~(1<<WGM13);
			TCCR3B |= (1<<WGM12); break;
	default:
		TCCR3A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR3B &= ~((1<<WGM13)|(1<<WGM12)); break;
	}
}

uint16_t getTimer3CNT()
{
	return TCNT3;
}

void setTimer3InterruptFlag(uint8_t interrupt)
{
	TIMSK3 |= interrupt;
}

void clearTimer3InterruptFlag(uint8_t interrupt)
{
	TIMSK3 &= ~interrupt;
}

uint8_t checkTimer3InterruptFlag(uint8_t interrupt)
{
	uint8_t _int_tmp = 0;
	switch(interrupt)
	{
	case TOV3: _int_tmp = TOV3;  break;
	case OCF3A: _int_tmp = OCF3A; break;
	case OCF3B: _int_tmp = OCF3B; break;
	case ICF3: _int_tmp = ICF3; break;
	}

	if((TIFR3&INT_BITS) == _int_tmp)
		return 1;

	return 0;
}

void timer3PWM_C(PORT_MODE_t mode)
{
	DDRE |= 1<<PE5; // Pin 51
	TCCR3A |= (TO_INT(mode))<<COM0C;
}

void timer3PWM_B(PORT_MODE_t mode)
{
	DDRE |= 1<<PE4; // Pin 50
	TCCR3A |= (TO_INT(mode))<<COM0B;
}

void timer3PWM_A(PORT_MODE_t mode)
{
	DDRE |= 1<<PE3; // Pin 49
	TCCR3A |= (TO_INT(mode))<<COM0A;
}

void setCNT_Timer3(uint16_t cnt)
{
	TCNT3 = cnt;
}

void clear_reg_3()
{
	TCNT3 = 0;
	TCCR3A = 0;
	TCCR3B = 0;
	TIMSK3 = 0;
}


// ========================================================================
//                         Timer 4

void start_timer_4(CLOCK_t clk)
{
	TCCR4B = TO_INT(clk);
}

void stop_timer_4()
{
	TCCR4B &= ~CSx_BITS;
}

void set_timer_4_mode(TIMER_MODE_t mode)
{
	switch(mode)
	{
	case NORMAL_MODE:
		TCCR4A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR4B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_1:
		TCCR4A |= (1<<WGM11)|(1<<WGM10);
		TCCR4B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_2:
			TCCR4A |= (1<<WGM11)|(1<<WGM10);
			TCCR4B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case CTC_MODE:
		TCCR4A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR4B &= ~(1<<WGM13);
		TCCR4B |= (1<<WGM12); break;
	case FAST_PWM_MODE_1:
		TCCR4A |= (1<<WGM11)|(1<<WGM10);
		TCCR4B &= ~(1<<WGM13);
		TCCR4B |= (1<<WGM12); break;
	case FAST_PWM_MODE_2:
		TCCR4A |= (1<<WGM11)|(1<<WGM10);
		TCCR4B &= ~(1<<WGM13);
		TCCR4B |= (1<<WGM12); break;
	default:
		TCCR4A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR4B &= ~((1<<WGM13)|(1<<WGM12)); break;
	}
}

uint16_t getTimer4CNT()
{
	return TCNT4;
}

void setTimer4InterruptFlag(uint8_t interrupt)
{
	TIMSK4 |= interrupt;
}

void clearTimer4InterruptFlag(uint8_t interrupt)
{
	TIMSK4 &= ~interrupt;
}

uint8_t checkTimer4InterruptFlag(uint8_t interrupt)
{
	uint8_t _int_tmp = 0;
	switch(interrupt)
	{
	case TOV4: _int_tmp = TOV4;  break;
	case OCF4A: _int_tmp = OCF4A; break;
	case OCF4B: _int_tmp = OCF4B; break;
	case ICF4: _int_tmp = ICF4; break;
	}

	if((TIFR4&INT_BITS) == _int_tmp)
		return 1;

	return 0;
}

void timer4PWM_C(PORT_MODE_t mode)
{
	DDRB |= 1<<PB7; // Pin 43
	TCCR4A |= (TO_INT(mode))<<COM0C;
}

void timer4PWM_B(PORT_MODE_t mode)
{
	DDRB |= 1<<PB6; // Pin 42
	TCCR4A |= (TO_INT(mode))<<COM0B;
}

void timer4PWM_A(PORT_MODE_t mode)
{
	DDRB |= 1<<PB5; // Pin 41
	TCCR4A |= (TO_INT(mode))<<COM0A;
}

void setCNT_Timer4(uint16_t cnt)
{
	TCNT4 = cnt;
}

void clear_reg_4()
{
	TCNT4 = 0;
	TCCR4A = 0;
	TCCR4B = 0;
	TIMSK4 = 0;
}


// ========================================================================
//                         Timer 5

void start_timer_5(CLOCK_t clk)
{
	TCCR5B = TO_INT(clk);
}

void stop_timer_5()
{
	TCCR5B &= ~CSx_BITS;
}

void set_timer_5_mode(TIMER_MODE_t mode)
{
	switch(mode)
	{
	case NORMAL_MODE:
		TCCR5A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR5B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_1:
		TCCR5A |= (1<<WGM11)|(1<<WGM10);
		TCCR5B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case PWM_MODE_2:
		TCCR5A |= (1<<WGM11)|(1<<WGM10);
		TCCR5B &= ~((1<<WGM13)|(1<<WGM12)); break;
	case CTC_MODE:
		TCCR5A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR5B &= ~(1<<WGM13);
		TCCR5B |= (1<<WGM12); break;
	case FAST_PWM_MODE_1:
		TCCR5A |= (1<<WGM11)|(1<<WGM10);
		TCCR5B &= ~(1<<WGM13);
		TCCR5B |= (1<<WGM12); break;
	case FAST_PWM_MODE_2:
		TCCR5A |= (1<<WGM11)|(1<<WGM10);
		TCCR5B &= ~(1<<WGM13);
		TCCR5B |= (1<<WGM12); break;
	default:
		TCCR5A &= ~((1<<WGM11)|(1<<WGM10));
		TCCR5B &= ~((1<<WGM13)|(1<<WGM12)); break;
	}
}

uint16_t getTimer5CNT()
{
	return TCNT5;
}

void setTimer5InterruptFlag(uint8_t interrupt)
{
	TIMSK5 |= interrupt;
}

void clearTimer5InterruptFlag(uint8_t interrupt)
{
	TIMSK5 &= ~interrupt;
}

uint8_t checkTimer5InterruptFlag(uint8_t interrupt)
{
	uint8_t _int_tmp = 0;
	switch(interrupt)
	{
	case TOV5: _int_tmp = TOV5;  break;
	case OCF5A: _int_tmp = OCF5A; break;
	case OCF5B: _int_tmp = OCF5B; break;
	case ICF5: _int_tmp = ICF5; break;
	}

	if((TIFR5&INT_BITS) == _int_tmp)
		return 1;

	return 0;
}

void timer5PWM_C(PORT_MODE_t mode)
{
	DDRB |= 1<<PB7; // Pin 43
	TCCR5A |= (TO_INT(mode))<<COM0C;
}

void timer5PWM_B(PORT_MODE_t mode)
{
	DDRB |= 1<<PB6; // Pin 42
	TCCR5A |= (TO_INT(mode))<<COM0B;
}

void timer5PWM_A(PORT_MODE_t mode)
{
	DDRB |= 1<<PB5; // Pin 41
	TCCR5A |= (TO_INT(mode))<<COM0A;
}

void setCNT_Timer5(uint16_t cnt)
{
	TCNT5 = cnt;
}

void clear_reg_5()
{
	TCNT5 = 0;
	TCCR5A = 0;
	TCCR5B = 0;
	TIMSK5 = 0;
}

