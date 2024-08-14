CC = "C:\\Program Files (x86)\\Atmel\\Studio\\7.0\\toolchain\\avr8\\avr8-gnu-toolchain\\bin\\avr-gcc"
OBJCOPY = "C:\\Program Files (x86)\\Atmel\\Studio\\7.0\\toolchain\\avr8\\avr8-gnu-toolchain\\bin\\avr-objcopy"
AVRDUDE = "C:\\Users\\ariel\\Downloads\\avrdude-v7.3-windows-x64\\avrdude.exe"

OUTNAME = pi

CFLAGS = \
 -Wall \
 -Os \
 -fpack-struct \
 -fshort-enums \
 -ffunction-sections \
 -fdata-sections \
 -std=gnu99 \
 -funsigned-char \
 -funsigned-bitfields \
 -mmcu=atmega328p \
 -DF_CPU=16000000UL \
 -MMD \
 -MP \
 -MD \
 -I../include
LDLIBS = 
LDFLAGS = -mmcu=atmega328p

OBJ = ds18b20.o onewire.o main.o termometro.o lcd.o buttons.o main_fsm.o interface.o variables.o

ifeq ($(LOG), 1)
OBJ += usart.o 
CFLAGS += -DLOG
else
CFLAGS += -UDLOG
endif

vpath %.c ../src ../src/ds18b20

ELF := $(OUTNAME).elf
HEX := $(OUTNAME).hex

OUTPUTS := $(OBJ) $(OBJ:%.o=%.d) $(ELF) $(HEX)

all: $(OBJ)

$(ELF): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

$(HEX): $(ELF)
	$(OBJCOPY) -R .eeprom -R .fuse -R .lock -R .signature -O ihex $< $@

arduino: $(HEX)
	$(AVRDUDE) -pm328p -carduino -P COM3 -Uflash:w:$<:a

-include $(OBJ:%.o=%.d)

.PHONY: clean

clean:
	rm -f $(OUTPUTS)

%.gitignore:
	echo $(@:%.gitignore=%) >> .gitignore

.gitignore: $(addsuffix .gitignore, $(OUTPUTS))
