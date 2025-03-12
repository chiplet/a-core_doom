RISCV_PREFIX ?= riscv64-unknown-elf-
GCC = $(RISCV_PREFIX)gcc
OBJDUMP = $(RISCV_PREFIX)objdump
OBJCOPY = $(RISCV_PREFIX)objcopy
SIZE = $(RISCV_PREFIX)size

# Platform to run the program on, e.g. sim, fpga, asic
# Determines UART baud rate
# use "sim" for simulations
PLATFORM ?= sim

# Core clock frequency for physical implementations
FREQ_CLK_CORE ?= 50000000

# CFLAGS=-Wall -O2 -march=rv32im -mabi=ilp32 -ffreestanding -flto -nostartfiles -fomit-frame-pointer -Wl,--gc-section --specs=nano.specs -I..

# A-Core library PATH (Relative to this Makefile)
A_CORE_LIB_PATH ?= ../../../../lib/a-core-library

# A-Core generated header files
A_CORE_HEADERS_PATH ?= ../../../../../acorechip/chisel/include
A_CORE_HEADERS = $(wildcard $(A_CORE_HEADERS_PATH)/*.h)

# Find library objects in path variable LIBRARIES (Assume they are compiled)
LIBS = $(foreach path, $(LIBRARIES), $(wildcard $(path)/*.o))

INCLUDES =
INCLUDES += -I ..
INCLUDES += -I $(A_CORE_LIB_PATH)/a-core-utils/include
INCLUDES += -I ../../../../ACoreChip/verilog/include
INCLUDES ?= $(foreach path, $(LIBRARIES), -I$(path)/include) 	# Auto-add libs
INCLUDES += -I$(A_CORE_HEADERS_PATH)							# Manual add


# Program and data memory specifications
PROGMEM_START ?= 0x00001000
PROGMEM_LENGTH ?= 16M
DATAMEM_START ?= 0x20000000
DATAMEM_LENGTH ?= 16M

CFLAGS ?= -march=rv32im_zicsr -mabi=ilp32 -ffreestanding -nostartfiles -Os -fdata-sections -ffunction-sections --specs=nosys.specs $(INCLUDES)
# LDOPTS ?= -T a-core.ld -lc_nano -lgcc -Wl,--gc-sections \
# 	-Wl,--defsym=PROGMEM_START=$(PROGMEM_START),--defsym=PROGMEM_LENGTH=$(PROGMEM_LENGTH),--defsym=DATAMEM_START=$(DATAMEM_START),--defsym=DATAMEM_LENGTH=$(DATAMEM_LENGTH)
LDOPTS ?= -T a-core.ld -lc_nano -lgcc -Wl,--gc-sections \
	-Wl,--defsym=PROGMEM_START=$(PROGMEM_START),--defsym=PROGMEM_LENGTH=$(PROGMEM_LENGTH),--defsym=DATAMEM_START=$(DATAMEM_START),--defsym=DATAMEM_LENGTH=$(DATAMEM_LENGTH) \
	-Wl,-Map=mapfile.map


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
	newlib_mods.c \
	$(NULL)

# miniwad.o \
# $(A_CORE_LIB_PATH)/a-core-utils/src/a-core-utils.c  \

CSOURCES := $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)


all: size

# miniwad.o: miniwad.wad
# $(OBJCOPY) -I binary -O elf32-littleriscv -B riscv miniwad.wad miniwad.o --rename-section .data=.rodata
	# $(OBJCOPY) -I binary -O elf32-littleriscv -B riscv miniwad.wad miniwad.o --rename-section .data=.miniwad_rodata

# doom-riscv.elf: riscv.lds $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)
# 	$(CC) $(CFLAGS) -Wl,-Bstatic,-T,riscv.lds,--strip-debug -o $@ $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)
# 	$(SIZE) $@
# doom-riscv.elf: $(SOURCES)
# 	$(CC) $(SOURCES) $(CFLAGS) acore_start.s -o $@ $(LDOPTS)
# 	$(SIZE) $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

prog: doom-riscv.bin
	$(ICEPROG) -o 1M $<

prog_wad: data/doomu.wad
	$(ICEPROG) -o 2M $<


.PHONY: all clean dump size

# Saves the previous platform so that it know to recompile if it changes
.PHONY: force
platform_selection: force
	echo '$(PLATFORM)' | cmp -s - $@ || echo '$(PLATFORM)' > $@

# Compile and link the program pointed by CSOURCES with libraries in LIBS
%.elf: $(CSOURCES) $(A_CORE_HEADERS) acore_start.s $(LIBS) a-core.ld platform_selection
# %.elf: miniwad.o
	$(GCC) $(CSOURCES) $(CFLAGS) -Wall acore_start.s -o $@ $(LIBS) $(LDOPTS) -D$(PLATFORM) -DFREQ_CLK_CORE=$(FREQ_CLK_CORE) $(INCLUDES)

syms: $(PLATFORM).elf
	$(OBJDUMP) -x $< | less

dump: $(PLATFORM).elf
	$(OBJDUMP) -x -d -Mno-aliases $< | less

size: $(PLATFORM).elf
	$(SIZE) $<

clean:
	rm -f *.elf *.bin *.hex *.gen.h *.o platform_selection
