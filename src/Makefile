MCU = atmega8515

CFLAGS = -mmcu=$(MCU)
CFLAGS += -g0
CFLAGS += -DF_CPU=16000000UL
CFLAGS += -Os
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu99

all:
	avr-gcc -c main.c -o main.o ${CFLAGS}
	avr-gcc -c lcd_driver.c -o lcd_driver.o ${CFLAGS}
	avr-gcc main.o lcd_driver.o -o main.elf -Wl,-u,vfprintf -lprintf_min  
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex
