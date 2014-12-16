/**
 \file bootloader.c
 \author Shauerman Alexander <shamrel@yandex.ru>  www.labfor.ru
 \details Загрузчик совместим с протоколом AVR109 AVR911 "Butterfly"
 Для ввода в режим программирования следует удерживая клавишу "1", нажать RESET,
 или в течечении 1 секунды после рестарта микроконтроллера послать по UART символ 's'.
 \version   0.1
 \date 16.12.2014
 \copyright Это программное обеспечение реализовано на базе загрузчика Мартина Томаса
 (Copyright (c) 2006-2008 M. Thomas, U. Bonnes, O. Rempel), распростроняемого по лицензии BSD.
 Оригинальные коды можно найти: http://siwawi.bauing.uni-kl.de/avr_projects/
 Это программное обеспечение распространяется под лицензией BSD 2-ух пунктов. Эта лицензия дает
 все права на использование и распространение программы в двоичном виде или
 в виде исходного кода, при условии, что в исходном коде сохранится указание
 авторских прав.

 This software is licensed under the simplified BSD license. This license gives
 everyone the right to use and distribute the code, either in binary or
 source code format, as long as the copyright license is retained in
 the source code.
 */


#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/* Part-Code ISP */
#define DEVTYPE_ISP     0x43
/* Part-Code Boot */
#define DEVTYPE_BOOT    0x44

#define SIG_BYTE1	0x1E
#define SIG_BYTE2	0xA7
#define SIG_BYTE3	0x01

#define APP_END (FLASHEND - (BOOTSIZE * 2))


/* Device-Type:
   For AVRProg the BOOT-option is prefered 
   which is the "correct" value for a bootloader.
   avrdude may only detect the part-code for ISP */
#define DEVTYPE     DEVTYPE_BOOT
// #define DEVTYPE     DEVTYPE_ISP

/*
 * Watchdog-reset is issued at exit 
 * define the timeout-value here (see avr-libc manual)
 */
#define EXIT_WDT_TIME   WDTO_250MS
//#define EXIT_WDT_TIME   WDTO_30MS

//#define START_SIMPLE
#define START_WAIT
//#define START_POWERSAVE
//#define START_BOOTICE

/* character to start the bootloader in mode START_WAIT */
#define START_WAIT_UARTCHAR 'S'

/* wait-time for START_WAIT mode ( t = WAIT_TIME * 10ms ) */
#define WAIT_VALUE 100 /* here: 100*10ms = 1000ms = 1sec */


/*
 * enable/disabl нe readout of fuse and lock-bits
 * (AVRPROG has to detect the AVR correctly by device-code
 * to show the correct information).
 */
#define ENABLEREADFUSELOCK

/* enable/disable write of lock-bits
 * WARNING: lock-bits can not be reseted by bootloader (as far as I know)
 * Only protection no unprotection, "chip erase" from bootloader only
 * clears the flash but does no real "chip erase" (this is not possible
 * with a bootloader as far as I know)
 * Keep this undefined!
 */
//#define WRITELOCKBITS

/*
 * define the following if the bootloader should not output
 * itself at flash read (will fake an empty boot-section)
 */
#define READ_PROTECT_BOOTLOADER


#define VERSION_HIGH '0'
#define VERSION_LOW  '1'

uint8_t gBuffer[SPM_PAGESIZE];


/* make the linker happy - it wants to see __vector_default */
// void __vector_default(void) { ; }
void __vector_default(void) { ; }

static void sendchar(uint8_t data)
{
	while (!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}

static uint8_t recvchar(void)
{
	while (!(UCSR1A & (1<<RXC1)));
	return UDR1;
}


static inline void eraseFlash(void)
{
	// erase only main section (bootloader protection)
	uint32_t addr = 0;
	while (APP_END > addr) {
		boot_page_erase(addr);		// Perform page erase
		boot_spm_busy_wait();		// Wait until the memory is erased.
		addr += SPM_PAGESIZE;
	}
	boot_rww_enable();
}

static inline void recvBuffer(uint16_t size)
{
	uint16_t cnt;
	uint8_t *tmp = gBuffer;

	for (cnt = 0; cnt < sizeof(gBuffer); cnt++) {
		*tmp++ = (cnt < size) ? recvchar() : 0xFF;
	}
}

static inline uint16_t writeFlashPage(uint16_t waddr, uint16_t size)
{
	uint32_t pagestart = (uint32_t)waddr<<1;
	uint32_t baddr = pagestart;
	uint16_t data;
	uint8_t *tmp = gBuffer;

	do {
		data = *tmp++;
		data |= *tmp++ << 8;
		boot_page_fill(baddr, data);	// call asm routine.

		baddr += 2;			// Select next word in memory
		size -= 2;			// Reduce number of bytes to write by two
	} while (size);				// Loop until all bytes written

	boot_page_write(pagestart);
	boot_spm_busy_wait();
	boot_rww_enable();		// Re-enable the RWW section

	return baddr>>1;
}

static inline uint16_t writeEEpromPage(uint16_t address, uint16_t size)
{
	uint8_t *tmp = gBuffer;

	do {
		eeprom_write_byte( (uint8_t*)address, *tmp++ );
		address++;			// Select next byte
		size--;				// Decreas number of bytes to write
	} while (size);				// Loop until all bytes written

	// eeprom_busy_wait();

	return address;
}

static inline uint16_t readFlashPage(uint16_t waddr, uint16_t size)
{
	uint32_t baddr = (uint32_t)waddr<<1;
	uint16_t data;

	do {
#ifndef READ_PROTECT_BOOTLOADER
#warning "Bootloader not read-protected"
#if defined(RAMPZ)
		data = pgm_read_word_far(baddr);
#else
		data = pgm_read_word_near(baddr);
#endif
#else
		// don't read bootloader
		if ( baddr < APP_END ) {
#if defined(RAMPZ)
			data = pgm_read_word_far(baddr);
#else
			data = pgm_read_word_near(baddr);
#endif
		}
		else {
			data = 0xFFFF; // fake empty
		}
#endif
		sendchar(data);				// send LSB
		sendchar((data >> 8));		// send MSB
		baddr += 2;					// Select next word in memory
		size -= 2;					// Subtract two bytes from number of bytes to read
	} while (size);					// Repeat until block has been read

	return baddr>>1;
}

static inline uint16_t readEEpromPage(uint16_t address, uint16_t size)
{
	do {
		sendchar( eeprom_read_byte( (uint8_t*)address ) );
		address++;
		size--;					// Decrease number of bytes to read
	} while (size);				// Repeat until block has been read

	return address;
}

#if defined(ENABLEREADFUSELOCK)
static uint8_t read_fuse_lock(uint16_t addr)
{
	uint8_t mode = (1<<BLBSET) | (1<<SPMEN);
	uint8_t retval;

	asm volatile
	(
		"movw r30, %3\n\t"		/* Z to addr */ \
		"sts %0, %2\n\t"		/* set mode in SPM_REG */ \
		"lpm\n\t"				/* load fuse/lock value into r0 */ \
		"mov %1,r0\n\t"			/* save return value */ \
		: "=m" (SPMCSR),
		  "=r" (retval)
		: "r" (mode),
		  "r" (addr)
		: "r30", "r31", "r0"
	);
	return retval;
}
#endif

static void send_boot(void)
{
	sendchar('A');
	sendchar('V');
	sendchar('R');
	sendchar('B');
	sendchar('O');
	sendchar('O');
	sendchar('T');
}

static void (*jump_to_app)(void) = 0x0000;

#define L_RUN_APL				(0x00)
#define L_START					(0x0F)
#define L_RUN					(0x02)
#define L_ERASE_FLASH			(0x03)
#define L_PM_MODE				(0x04)
#define L_LV_PM_MODE			(0x05)
#define L_RT_PM_MODE			(0x06)
#define L_ERASE_FLASH_COPLT		(0x07)


// Выводим текущее состояние ботлоадера на светодиоды.
static inline print_state_led(char state)
{
	PORTF &= ~0x0F;
	PORTF |= state;
};

int main(void)
{
	uint16_t address = 0;
	uint8_t device = 0, val;

	//! Инициализация портов для светодиодов.
	DDRF |= (1 << DDF0 | 1 << DDF1 | 1 << DDF2 | 1 << DDF3);
	//! Инициализация портов для строк клавиатуры. Строки на ввод. Поддяжка к питанию.
	DDRD &= ~(1 << DDD4);
	PORTD |= (1 << PORTD4);
	//! Инициализация портов для столбцоы клавиатуры. столбцы на вывод.
	DDRG |= (1 << DDG0 );
	PORTG &= ~(1 << PORTG0);


	print_state_led(L_START);


	//! Инициализируем watchdog таймер и останавливаем его.
	cli();
	wdt_reset();
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR 	= (1<<WDE) | (1<<WDP2) | (1<<WDP0);
	MCUSR = 0;
	wdt_disable();

	// Инициализируем UART
	UBRR1 = 16;			// 115200

	UCSR1A = ( 1<<U2X1 );
	UCSR1B = ((1<<TXEN1) | (1<<RXEN1));
	UCSR1C = ((1<<UCSZ11) | (1<<UCSZ10));
	


	// Ожидаем событие для входа в режим прошивки.
	uint8_t cnt = 0;
	while (1) {

		if(!(PIND & (1 << PIND4))) break;			// Нажата первая клавиша.
		if (UCSR1A & (1 << RXC1))
			if (UDR1 == START_WAIT_UARTCHAR)
				break;

		if (cnt++ >= WAIT_VALUE) {
			print_state_led(L_RUN_APL);
			jump_to_app();							// Событие не наступило, запускаем приложение.
		}
		_delay_ms(10);
	}
	send_boot();									// Печатаем приветствие.

	print_state_led(L_RUN);
	for(;;) {

		val = recvchar();
		// Autoincrement?
		if (val == 'a') {
			sendchar('Y');			// Autoincrement is quicker

		//write address
		} else if (val == 'A') {
			address = recvchar();		//read address 8 MSB
			address = (address<<8) | recvchar();
			sendchar('\r');

		// Buffer load support
		} else if (val == 'b') {
			sendchar('Y');					// Report buffer load supported
			sendchar((sizeof(gBuffer) >> 8) & 0xFF);	// Report buffer size in bytes
			sendchar(sizeof(gBuffer) & 0xFF);

		// Start buffer load
		} else if (val == 'B') {
			uint16_t size;
			size = recvchar() << 8;				// Load high byte of buffersize
			size |= recvchar();				// Load low byte of buffersize
			val = recvchar();				// Load memory type ('E' or 'F')
			recvBuffer(size);

			if (device == DEVTYPE) {
				if (val == 'F') {
					address = writeFlashPage(address, size);
				} else if (val == 'E') {
					address = writeEEpromPage(address, size);
				}
				sendchar('\r');
			} else {
				sendchar(0);
			}

		// Block read
		} else if (val == 'g') {
			uint16_t size;
			size = recvchar() << 8;				// Load high byte of buffersize
			size |= recvchar();				// Load low byte of buffersize
			val = recvchar();				// Get memtype

			if (val == 'F') {
				address = readFlashPage(address, size);
			} else if (val == 'E') {
				address = readEEpromPage(address, size);
			}

		// Chip erase
 		} else if (val == 'e') {
			if (device == DEVTYPE) {
				print_state_led(L_ERASE_FLASH);
				eraseFlash();
				print_state_led(L_ERASE_FLASH_COPLT);
			}
			sendchar('\r');

		// Exit upgrade
		} else if (val == 'E') {
			wdt_enable(EXIT_WDT_TIME); // Enable Watchdog Timer to give reset
			//jump_to_app();
			sendchar('\r');

#ifdef WRITELOCKBITS
#warning "Extension 'WriteLockBits' enabled"
		// TODO: does not work reliably
		// write lockbits
		} else if (val == 'l') {
			if (device == DEVTYPE) {
				// write_lock_bits(recvchar());
				boot_lock_bits_set(recvchar());	// boot.h takes care of mask
				boot_spm_busy_wait();
			}
			sendchar('\r');
#endif
		// Enter programming mode
		} else if (val == 'P') {
			sendchar('\r');
			print_state_led(L_PM_MODE);
		// Leave programming mode
		} else if (val == 'L') {
			sendchar('\r');
			print_state_led(L_LV_PM_MODE);

		// return programmer type
		} else if (val == 'p') {
			sendchar('S');		// always serial programmer
			print_state_led(L_RT_PM_MODE);
#ifdef ENABLEREADFUSELOCK

		// read "low" fuse bits
		} else if (val == 'F') {
			sendchar(read_fuse_lock(GET_LOW_FUSE_BITS));

		// read lock bits
		} else if (val == 'r') {
			sendchar(read_fuse_lock(GET_LOCK_BITS));

		// read high fuse bits
		} else if (val == 'N') {
			sendchar(read_fuse_lock(GET_HIGH_FUSE_BITS));

		// read extended fuse bits
		} else if (val == 'Q') {
			sendchar(read_fuse_lock(GET_EXTENDED_FUSE_BITS));
#endif

		// Return device type
		} else if (val == 't') {
			sendchar(DEVTYPE);
			sendchar(0);

		// clear and set LED ignored
		} else if ((val == 'x') || (val == 'y')) {
			recvchar();
			sendchar('\r');

		// set device
		} else if (val == 'T') {
			device = recvchar();
			sendchar('\r');

		// Return software identifier
		} else if (val == 'S') {
			send_boot();

		// Return Software Version
		} else if (val == 'V') {
			sendchar(VERSION_HIGH);
			sendchar(VERSION_LOW);

		// Return Signature Bytes (it seems that 
		// AVRProg expects the "Atmel-byte" 0x1E last
		// but shows it first in the dialog-window)
		} else if (val == 's') {
			sendchar(SIG_BYTE3);
			sendchar(SIG_BYTE2);
			sendchar(SIG_BYTE1);

		/* ESC */
		} else if(val != 0x1b) {
			sendchar('?');
		}
	}
	return 0;
}
