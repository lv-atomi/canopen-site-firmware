PROJ_NAME=main

DEBUG=1

CC=arm-none-eabi-gcc --specs=nosys.specs
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
OBJDIR = build

LIBROOT := Libraries
CMSISROOT := $(LIBROOT)/cmsis/cm4
DRIVERROOT := $(LIBROOT)/Drivers

ifdef DEBUG
CFLAGS  = -O0 -g3 -MMD
else
CFLAGS  = -O2 -finline -finline-functions-called-once
endif
CFLAGS += -Wall -Wno-missing-braces -std=c99 -mthumb -mcpu=cortex-m4
#CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -mfloat-abi=soft
# TODO: hard float was causing an exception; see what's up.
LDFLAGS = -Wl,-Map,$(OBJDIR)/$(PROJ_NAME).map -g -T$(CMSISROOT)/devicesupport/gcc/linker/AT32F403AxG_FLASH.ld
DEFINES =  -DAT32F403ACGU7 -DAT_START_F403A_V1	# device & board selection
export DEFINES


CFLAGS += $(DEFINES) -Iuser -I $(CMSISROOT)/coresupport -I $(CMSISROOT)/devicesupport -I $(DRIVERROOT)/inc


SRCS = $(wildcard user/*.c) \
	$(wildcard $(DRIVERROOT)/src/*.c) \
	$(CMSISROOT)/devicesupport/system_at32f403a_407.c \
	$(CMSISROOT)/devicesupport/gcc/startup_at32f403a_407.s


OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))


all: proj

# $(FWLIB): $(wildcard $(LIBROOT)/STM32F4xx_StdPeriph_Driver/*.h) $(wildcard $(LIBROOT)/STM32F4xx_StdPeriph_Driver/inc/*.h)
# 	@cd $(FWROOT) && $(MAKE)

proj: $(OBJDIR)/$(PROJ_NAME).elf $(OBJDIR)/$(PROJ_NAME).hex $(OBJDIR)/$(PROJ_NAME).bin

$(OBJDIR)/%.elf: $(OBJS) $(FWLIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJDIR):
	mkdir -p $@

clean:
	rm -f $(OBJDIR)/$(PROJ_NAME).elf
	rm -f $(OBJDIR)/$(PROJ_NAME).hex
	rm -f $(OBJDIR)/$(PROJ_NAME).bin
	rm -f $(OBJDIR)/$(PROJ_NAME).map
	find $(OBJDIR) -type f -name '*.[odt]' -print0 | xargs -0 -r rm


flash: $(OBJDIR)/$(PROJ_NAME).elf
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg -f program.cfg

openocd:
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg

gdb: $(OBJDIR)/$(PROJ_NAME).elf
	$(GDB) --tui $(OBJDIR)/$(PROJ_NAME).elf -ex "target remote :3333"

# Dependdencies
$(OBJDIR)/$(PROJ_NAME).elf: $(FWLIB) $(OBJS) | $(OBJDIR)


TAGFILES    := $(OBJS:.o=.t)

%.t: %.d
	@(cat $< 2>/dev/null || true) | sed 's/.*://' | tr -d '\\' | tr "\n" " " | (xargs -n 1 readlink -f > $@ 2>/dev/null || true)

%.d: %.o
	@echo >/dev/null	#do nothing, specify dependency only

-include $($(OBJS:.o=.d))

tags: $(TAGFILES)
	@(cat $(TAGFILES) 2>/dev/null || true) | sort | uniq | xargs etags
