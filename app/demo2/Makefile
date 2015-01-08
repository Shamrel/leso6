TARGET=demo2
 
CC = avr-gcc
OBJCOPY = avr-objcopy
PLATFORM_DIR = ../../platform
 
# Список исходников
SRCS= demo2.c 
SRCS+= $(PLATFORM_DIR)/ds18b20.c
SRCS+= $(PLATFORM_DIR)/i2c.c
SRCS+= $(PLATFORM_DIR)/lcd.c
SRCS+= $(PLATFORM_DIR)/rtc.c
SRCS+= $(PLATFORM_DIR)/uart.c
SRCS+= $(PLATFORM_DIR)/timer.c

# Настройки avrdude
DUDE_PROGRAMMER = avr911
DUDE_PORT = /dev/ttyUSB0
DUDE_BAUDRATE = 115200
DUDE_AVR_DEVICE = m128rfa1
 
OBJECTS = $(SRCS:.c=.o)
 
# Тип микроконтроллера
MCU=atmega128rfa1
 
# Частота процессора. Нужна для некоторых макросов 
F_CPU=16000000
 
# Флаги компилятора
CFLAGS = -mmcu=$(MCU) -Wall -g -Os  -lm  -mcall-prologues -DF_CPU=$(F_CPU)
CFLAGS += -I$(PLATFORM_DIR) 
LDFLAGS = -mmcu=$(MCU)  -Wall -g -Os  
 
all: $(TARGET) size
 
$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJECTS) -lm
#	$(OBJCOPY) -O binary -R .eeprom -R .nwram  $@.elf $@.bin
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
 
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	rm -fv *.elf *.bin *.hex  $(OBJECTS) *.map $(PLATFORM_DIR)/*.o

	

SIZE = avr-size
ELFSIZE1 = $(SIZE)  -A $(TARGET).elf
ELFSIZE2 = $(SIZE) --format=avr --mcu=$(MCU) $(TARGET).elf
size:
	@if [ -f $(TARGET).elf ]; then echo; $(ELFSIZE1); $(ELFSIZE2); echo; fi	
	
boot:
	avrdude -p$(DUDE_AVR_DEVICE) -c$(DUDE_PROGRAMMER) \
	-P$(DUDE_PORT) -b$(DUDE_BAUDRATE) -Uflash:w:$(TARGET).hex:a
	
	
	