$(foreach app,$(APPS),$(eval $(call build-rule,$(app))))
define build-rule
$1: PROJ_NAME = $1
$1: app/app_$1.c
	$$(CC) -o $1 app/app_$1.c
endef
.PHONY: all $(APPS)


DEBUG=1

CC=arm-none-eabi-gcc --specs=nosys.specs -Werror -Wno-unused-variable
#GDB=arm-none-eabi-gdb
GDB=gdb-multiarch
OBJCOPY=arm-none-eabi-objcopy
OBJDIR = build

LIBROOT := libraries
CMSISROOT := $(LIBROOT)/cmsis/cm4
DRIVERROOT := $(LIBROOT)/drivers

OPENOCD_CMSIS := ../OpenOCD/bin/openocd -s ../OpenOCD/scripts -f interface/cmsis-dap.cfg -f target/at32f421xx.cfg --command "adapter driver cmsis-dap; cmsis_dap_backend hid"
OPENOCD_ATLINK := ../OpenOCD/bin/openocd -s ../OpenOCD/scripts -f interface/atlink.cfg -f target/at32f421xx.cfg
OPENOCD := $(OPENOCD_ATLINK)

ifdef DEBUG
CFLAGS  = -O0 -g3 -MMD
else
CFLAGS  = -O2 -finline -finline-functions-called-once
endif
CFLAGS += -Wall -Wno-missing-braces -std=c99 -mthumb -mcpu=cortex-m4 -fdata-sections -ffunction-sections -Wl,--gc-sections
#CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -mfloat-abi=soft
# TODO: hard float was causing an exception; see what's up.
LDFLAGS = -Wl,-Map,$(OBJDIR)/$(PROJ_NAME).map -g -T$(CMSISROOT)/device_support/startup/gcc/linker/AT32F421x8_FLASH.ld
DEFINES =  -DAT32F421C8T7 -DAT_START_F421_V1	# device & board selection, use APPLICATION
export DEFINES

CFLAGS += $(DEFINES) -I. -I../user/subsystem -I$(CMSISROOT)/core_support -I$(CMSISROOT)/device_support -I$(DRIVERROOT)/inc

LIBSRCS = $(wildcard $(DRIVERROOT)/src/*.c) \
	$(wildcard ../user/subsystem/*.c) \
	$(CMSISROOT)/device_support/system_at32f421.c \
	$(CMSISROOT)/device_support/startup/gcc/startup_at32f421.s

LIBOBJS := $(LIBSRCS:.c=.o)
LIBOBJS := $(LIBOBJS:.s=.o)
LIBOBJS := $(addprefix $(OBJDIR)/lib/,$(LIBOBJS))

SRCS = $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJS)) $(LIBOBJS)

all: proj

proj: $(OBJDIR)/$(PROJ_NAME).elf $(OBJDIR)/$(PROJ_NAME).hex $(OBJDIR)/$(PROJ_NAME).bin

$(OBJDIR)/%.elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJDIR)/lib/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJDIR)/lib/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

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
	$(OPENOCD) -f ../program.cfg

reset: $(OBJDIR)/$(PROJ_NAME).elf
	$(OPENOCD) -f ../program_reset.cfg

flash-pyocd: $(OBJDIR)/$(PROJ_NAME).elf
	pyocd flash -v --config programmer_pyocd/pyocd_at32.yaml -t _at32f403acgu7 $<

pyocd: $(OBJDIR)/$(PROJ_NAME).elf
	pyocd gdbserver --config programmer_pyocd/pyocd_at32.yaml -t _at32f403acgu7


openocd:
	$(OPENOCD)

gdb: $(OBJDIR)/$(PROJ_NAME).elf
	$(GDB) $(OBJDIR)/$(PROJ_NAME).elf -ex "target remote :3333"

# Dependdencies
$(OBJDIR)/$(PROJ_NAME).elf: $(OBJS) | $(OBJDIR)


TAGFILES    := $(OBJS:.o=.t)

%.t: %.d
	@(cat $< 2>/dev/null || true) | sed 's/.*://' | tr -d '\\' | tr "\n" " " | (xargs -n 1 readlink -f > $@ 2>/dev/null || true)

%.d: %.o
	@echo >/dev/null	#do nothing, specify dependency only

-include $($(OBJS:.o=.d))

tags: $(TAGFILES)
	@(cat $(TAGFILES) 2>/dev/null || true) | sort | uniq | xargs etags
