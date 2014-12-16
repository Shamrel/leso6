TARGET=bootloader
 
CC = avr-gcc
OBJCOPY = avr-objcopy
 
# Список исходников
SRCS= bootloader.c 
 
OBJECTS = $(SRCS:.c=.o)
 
# Тип микроконтроллера
MCU=atmega128rfa1

# Настройки avrdude
DUDE_PROGRAMMER = ftbb
DUDE_AVR_DEVICE = m128rfa1

 
# Частота процессора. Нужна для некоторых макросов 
F_CPU=16000000
#Размер bootloader (в словах)
BOOTSIZE=1024
#Адрес Bootloader'а  (в батах)
MT_BOOTLOADER_ADDRESS = 0x1F800 

 
# Флаг для компилятора, чтобы выставить стандартный вид языка Си
#     c89   = "ANSI" C
#     gnu89 = c89 плюс расширения GCC
#     c99   = ISO C99 standard (до конца до сих пор не реализован)
#     gnu99 = c99 плюс расширения GCC
CSTANDARD = -std=gnu99
 
#define, которые хотим передать препроцессору 
CDEFS=-DF_CPU=$(F_CPU) -DBOOTSIZE=$(BOOTSIZE) -DBOOTLOADERHASNOVECTORS -D__AVR_ATmega128RFA1__
 

# Флаги компилятора
#CFLAGS = -mmcu=$(MCU) -Wall -g -Os  -lm  -mcall-prologues 
CFLAGS = -mmcu=$(MCU) -Wall -g -Os  -lm 
CFLAGS += $(CDEFS)
CFLAGS += $(CSTANDARD)

LDFLAGS = -mmcu=$(MCU)  -Wall -g -Os  
LDFLAGS += -Wl,--section-start=.text=$(MT_BOOTLOADER_ADDRESS)
 

LDFLAGS += -T./ldscripts_no_vector/avr5.x

 
all: $(TARGET) size
 
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJECTS) -lm
#	$(OBJCOPY) -O binary -R .eeprom -R .nwram  $@.elf $@.bin
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
 
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	rm -f *.elf *.bin *.hex  $(OBJECTS) *.map	
	
SIZE = avr-size
ELFSIZE1 = $(SIZE)  -A $(TARGET).elf
ELFSIZE2 = $(SIZE) --format=avr --mcu=$(MCU) $(TARGET).elf
size:
	@if [ -f $(TARGET).elf ]; then echo; $(ELFSIZE1); $(ELFSIZE2); echo; fi	
	
	
boot:
	avrdude -p$(DUDE_AVR_DEVICE) -c$(DUDE_PROGRAMMER) \
	-Uflash:w:$(TARGET).hex:a	
