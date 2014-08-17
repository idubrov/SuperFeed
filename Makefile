# Add .d to Make's recognized suffixes.
SUFFIXES += .d

# We don't need to clean up when we're making these targets
NODEPS := clean

# Sources
SOURCES := $(wildcard src/*.c) \
		   $(wildcard src/*.cpp) \
		   $(wildcard src/tui/*.cpp) \
           $(wildcard src/tui/menu/*.cpp) \
		   $(wildcard src/hw/*.cpp) \
		   $(wildcard src/stepper/*.cpp) \
		   $(wildcard system/src/cmsis/*.c) \
		   $(wildcard system/src/cortexm/*.c) \
		   $(wildcard system/src/diag/*.c) \
		   $(wildcard system/src/newlib/*.c) \
		   $(wildcard system/src/newlib/*.cpp) \
		   system/src/stm32f1-stdperiph/misc.c \
		   system/src/stm32f1-stdperiph/stm32f10x_gpio.c \
		   system/src/stm32f1-stdperiph/stm32f10x_rcc.c \
		   system/src/stm32f1-stdperiph/stm32f10x_tim.c \
		   system/src/stm32f1-stdperiph/stm32f10x_flash.c \
		   system/src/stm32f1-stdperiph/stm32f10x_dma.c

# These are the dependency files, which make will clean up after it creates them
DEPFILES := $(patsubst %.c,build/%.d,$(patsubst %.cpp,build/%.d,$(SOURCES)))

# Object files
OBJECTS := $(patsubst %.c,build/%.o,$(patsubst %.cpp,build/%.o,$(SOURCES)))

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJ_NAME := SuperFeed

OPENOCD_PATH ?= /usr/share/openocd

# Location of the linker scripts
LDSCRIPT_INC := ldscripts

# location of OpenOCD Board .cfg files (only used with 'make program')
OPENOCD_BOARD_DIR := ${OPENOCD_PATH}/scripts/board

# Configuration (cfg) file containing programming directives for OpenOCD
OPENOCD_PROC_FILE := extra/stm32vl-openocd.cfg

# that's it, no need to change anything below this line!

CC := arm-none-eabi-gcc
CXX := arm-none-eabi-g++
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE := arm-none-eabi-size

CFLAGS := -std=c11
CXXFLAGS := -std=c++11 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics

CPPFLAGS := -I include -I system/include -I system/include/cmsis -I system/include/stm32f1-stdperiph
CPPFLAGS += -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -DHSE_VALUE=8000000
CPPFLAGS += -mcpu=cortex-m3 -mthumb
CPPFLAGS += -fmessage-length=0
CPPFLAGS += -Os -flto -fuse-linker-plugin
CPPFLAGS += -fsigned-char -ffunction-sections -fdata-sections -ffreestanding
CPPFLAGS += -Wall -Wextra
CPPFLAGS += -T mem.ld -T libs.ld -T sections.ld
CPPFLAGS += -nostartfiles -Xlinker --gc-sections
CPPFLAGS += -Lldscripts/
CPPFLAGS += -Wl,-Map,build/$(PROJ_NAME).map --specs=nano.specs

.PHONY: program test

build/$(PROJ_NAME).elf : $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
	$(OBJCOPY) -O ihex $@ build/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $@ build/$(PROJ_NAME).bin
	$(OBJDUMP) -St $@ > build/$(PROJ_NAME).lst
	$(SIZE) build/$(PROJ_NAME).elf

# Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    # Chances are, these files don't exist.  GMake will create them and
    # clean up automatically afterwards
    -include $(DEPFILES)
endif

# This is the rule for creating the dependency files
build/%.d: %.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT '$(patsubst %.cpp,build/%.o,$<)' $< -MF $@

build/%.d: %.c
	mkdir -p `dirname $@`
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM -MT '$(patsubst %.c,build/%.o,$<)' $< -MF $@

build/%.o: %.cpp build/%.d
	mkdir -p `dirname $@`
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

build/%.o: %.c build/%.d
	mkdir -p `dirname $@`
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	
build/stepgen: test/stepgen.cpp src/stepper/stepgen.cpp include/stepper/stepgen.hpp
	mkdir -p `dirname $@`
	g++ -std=c++0x -Iinclude -o $@ test/stepgen.cpp src/stepper/stepgen.cpp
	
test: build/stepgen
	mkdir -p build/testresult
	cat test/testdata | \
	while read CMD; do \
		build/stepgen $$CMD > "build/testresult/$${CMD// /_}" ; \
	done
	diff -u test/expected build/testresult/
	
all: build/$(PROJ_NAME).elf build/$(PROJ_NAME).hex build/$(PROJ_NAME).bin build/$(PROJ_NAME).lst

openocd:
	openocd -f $(OPENOCD_BOARD_DIR)/stm32vldiscovery.cfg

program: build/$(PROJ_NAME).elf
	openocd -f $(OPENOCD_BOARD_DIR)/stm32vldiscovery.cfg -f $(OPENOCD_PROC_FILE) -c "stm_flash `pwd`/build/$(PROJ_NAME).bin" -c shutdown

clean:
	rm -rf build/
