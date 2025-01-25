CROSS ?= riscv64-unknown-elf-

CC = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
SIZE = $(CROSS)size
ICEPROG = iceprog

# CFLAGS=-Wall -O2 -march=rv32im -mabi=ilp32 -ffreestanding -flto -nostartfiles -fomit-frame-pointer -Wl,--gc-section --specs=nano.specs -I..

INCLUDES =
INCLUDES += -I ..
INCLUDES += -I a-core-library/a-core-utils/include
INCLUDES += -I ../../../../ACoreChip/verilog/include

# Program and data memory specifications
PROGMEM_START ?= 0x00001000
PROGMEM_LENGTH ?= 1M
DATAMEM_START ?= 0x20000000
# DATAMEM_LENGTH ?= 1024K
DATAMEM_LENGTH ?= 16M
CFLAGS ?= -march=rv32im_zicsr -mabi=ilp32 -ffreestanding -nostartfiles -Os -fdata-sections -ffunction-sections --specs=nosys.specs $(INCLUDES)
# LDOPTS ?= -T a-core.ld -lc_nano -lgcc -Wl,--gc-sections \
# 	-Wl,--defsym=PROGMEM_START=$(PROGMEM_START),--defsym=PROGMEM_LENGTH=$(PROGMEM_LENGTH),--defsym=DATAMEM_START=$(DATAMEM_START),--defsym=DATAMEM_LENGTH=$(DATAMEM_LENGTH)
LDOPTS ?= -T a-core.ld -lc_nano -lgcc -Wl,--gc-sections \
	-Wl,--defsym=PROGMEM_START=$(PROGMEM_START),--defsym=PROGMEM_LENGTH=$(PROGMEM_LENGTH),--defsym=DATAMEM_START=$(DATAMEM_START),--defsym=DATAMEM_LENGTH=$(DATAMEM_LENGTH)


CFLAGS += \
	-DNORMALUNIX \
	$(NULL)


include ../sources.mk

# Filter out d_main, we provide our own simplified one
SOURCES_doom := $(filter-out d_main.c,$(SOURCES_doom))

# Filter out s_sound, we provide a dummy one
SOURCES_doom := $(filter-out s_sound.c,$(SOURCES_doom))


SOURCES_doom_arch := \
	d_main.c \
	i_main.c \
	i_net.c \
	i_sound.c \
	i_system.c \
	i_video.c \
	s_sound.c \
	console.c  \
	libc_backend.c  \
	mini-printf.c \
	a-core-library/a-core-utils/src/a-core-utils.c \
	miniwad.o \
	$(NULL)

SOURCES := $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)


all: doom-riscv.bin

miniwad.o: miniwad.wad
# $(OBJCOPY) -I binary -O elf32-littleriscv -B riscv miniwad.wad miniwad.o --rename-section .data=.rodata
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv miniwad.wad miniwad.o --rename-section .data=.miniwad_rodata


# doom-riscv.elf: riscv.lds $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)
# 	$(CC) $(CFLAGS) -Wl,-Bstatic,-T,riscv.lds,--strip-debug -o $@ $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)
# 	$(SIZE) $@
doom-riscv.elf: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) acore_start.s -o $@ $(LDOPTS)
	$(SIZE) $@

clean:
	rm -f *.bin *.hex *.elf *.o *.gen.h


%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

prog: doom-riscv.bin
	$(ICEPROG) -o 1M $<

prog_wad: data/doomu.wad
	$(ICEPROG) -o 2M $<


.PHONY: all clean prog prog_wad
.PRECIOUS: *.elf
