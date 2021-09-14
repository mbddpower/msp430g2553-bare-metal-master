TARGET 	= uart
MCU 	= msp430g2553

INCLUDES += -I../../include
INCLUDES += -I../../libs/msp430/include_gcc
#vpath %.h $(sort $(dir $(SRCS)))

LINKER_SCRIPT = $(MCU).ld

CROSS_COMPILE = msp430-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
DEBUG 	= $(CROSS_COMPILE)gdb

CFLAGS += -Wall
CFLAGS += -mmcu=$(MCU)
CFLAGS += -O0 -g

LDFLAGS  = -L../../libs/msp430/include_gcc
LDFLAGS += -T $(LINKER_SCRIPT)
#LDFLAGS += -nostartfiles # no start files are used
#LDFLAGS += -Wl,--gc-sections # linker garbage collector
#LDFLAGS += -Wl,-Map=$(TARGET).map #generate map file

all: clean compile burn
	@echo "Succesfully Finished..."

build: $(TARGET).elf $(TARGET).hex $(TARGET).out $(TARGET).o

$(TARGET).out: $(TARGET).c
	@echo "Building..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $< -o $@

%.o: %.c
	@echo ".c>.o"
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

%.elf: %.o
	@echo ".o>.elf"
	$(LD) $(LDFLAGS) -o $@ $^

%.hex: %.elf
	@echo ".elf>.hex"
	$(OBJCOPY) -O ihex $< $@

compile: $(TARGET).c
	@echo "Compiling..."
	@$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $(TARGET).elf $<
	@$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex
	@echo "Succesfully compiled..."

disass: $(TARGET).elf
	@$(OBJDUMP) -d $(TARGET).elf


clean:
	@echo "Cleaning..."
	@rm -f *.o
	@rm -f $(TARGET).out
	@rm -f $(TARGET).hex
	@rm -f $(TARGET).map
	@rm -f $(TARGET).elf

burn:
	@MSP430Flasher -w $(TARGET).hex -v -b -u -z[VCC]

debug:
	@$(DEBUG) $(TARGET).out

.PHONY: clean build compile disass burn debug