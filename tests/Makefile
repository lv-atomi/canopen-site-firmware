PROJ_NAME=test_$(test)

DEBUG=1
FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))

CC=arm-none-eabi-gcc --specs=nosys.specs --specs=nano.specs -Werror -Wno-unused-variable
#GDB=arm-none-eabi-gdb
GDB=gdb-multiarch
OBJCOPY=arm-none-eabi-objcopy
OBJDIR = build

LIBROOT := ../Libraries
CMSISROOT := $(LIBROOT)/cmsis/cm4
DRIVERROOT := $(LIBROOT)/Drivers
CANOPENROOT := ../canopen
SRCROOT := ../user

OPENOCD_CMSIS := ../OpenOCD/bin/openocd -s ../OpenOCD/scripts -f interface/cmsis-dap.cfg -f target/at32f403axx.cfg --command "adapter driver cmsis-dap; cmsis_dap_backend hid"
OPENOCD_ATLINK := ../OpenOCD/bin/openocd -s ../OpenOCD/scripts -f interface/atlink.cfg -f target/at32f403axx.cfg
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
LDFLAGS = -Wl,-Map,$(OBJDIR)/$(PROJ_NAME).map -g -T$(CMSISROOT)/devicesupport/gcc/linker/AT32F403AxG_FLASH.ld
DEFINES =  -DAT32F403ACGU7 -DAT_START_F403A_V1	# device & board selection, use APPLICATION
export DEFINES

LITTLEFSINC = -I$(SRCROOT)/littlefs
CANOPENINC = -I$(CANOPENROOT) -I$(SRCROOT)/canopen -I$(SRCROOT)/canopen/driver -I$(SRCROOT)/canopen/application -I$(SRCROOT)/canopen/module
CFLAGS += $(DEFINES) -I$(SRCROOT) -I$(SRCROOT)/subsystem -I$(CMSISROOT)/coresupport -I$(CMSISROOT)/devicesupport -I$(DRIVERROOT)/inc $(CANOPENINC) $(LITTLEFSINC)

CANOPENSRC = $(CANOPENROOT)/301/CO_ODinterface.c \
	$(CANOPENROOT)/301/CO_NMT_Heartbeat.c \
	$(CANOPENROOT)/301/CO_HBconsumer.c \
	$(CANOPENROOT)/301/CO_Emergency.c \
	$(CANOPENROOT)/301/CO_SDOserver.c \
	$(CANOPENROOT)/301/CO_TIME.c \
	$(CANOPENROOT)/301/CO_SYNC.c \
	$(CANOPENROOT)/301/CO_PDO.c \
	$(CANOPENROOT)/303/CO_LEDs.c \
	$(CANOPENROOT)/305/CO_LSSslave.c \
	$(CANOPENROOT)/storage/CO_storage.c \
	$(CANOPENROOT)/CANopen.c \
	$(wildcard $(SRCROOT)/canopen/driver/*.c) \
	$(wildcard $(SRCROOT)/canopen/application/*.c) \
	$(wildcard $(SRCROOT)/canopen/module/*.c)

SRCS = $(call FILTER_OUT,main.c,$(wildcard $(SRCROOT)/*.c)) \
	$(CANOPENSRC) \
	$(wildcard $(SRCROOT)/subsystem/*.c) \
	$(wildcard $(SRCROOT)/littlefs/*.c) \
	$(wildcard $(DRIVERROOT)/src/*.c) \
	$(CMSISROOT)/devicesupport/system_at32f403a_407.c \
	$(CMSISROOT)/devicesupport/gcc/startup_at32f403a_407.s

TEST_OBJS = $(PROJ_NAME).o
TEST2_OBJS = $(addprefix $(OBJDIR)/,$(TEST_OBJS))

OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)
OBJS := $(addprefix $(OBJDIR)/lib/,$(OBJS))
OBJS := $(OBJS) $(TEST2_OBJS)

ALLTESTS = $(wildcard *.c)
ALLTESTS2 = $(ALLTESTS:.c=)
ALLTESTS3 = $(subst test_,,$(ALLTESTS2))

ifeq ($(test),)
$(info usage: make test=[$(ALLTESTS3)])
$(info Supply one of the test target in lists.)
all:
else
all: proj
endif

proj: $(OBJDIR)/$(PROJ_NAME).elf $(OBJDIR)/$(PROJ_NAME).hex $(OBJDIR)/$(PROJ_NAME).bin

$(OBJDIR)/%.elf: $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[LD] $<"

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJDIR)/lib/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $^
	@echo "[CC] $<"

$(OBJDIR)/lib/%.o: %.s
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $^
	@echo "[AS] $<"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $^
	@echo "[CC] $<"

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $^
	@echo "[AS] $<"

$(OBJDIR):
	@mkdir -p $@

clean:
	rm -f $(OBJDIR)/$(PROJ_NAME).elf
	rm -f $(OBJDIR)/$(PROJ_NAME).hex
	rm -f $(OBJDIR)/$(PROJ_NAME).bin
	rm -f $(OBJDIR)/$(PROJ_NAME).map
	find $(OBJDIR) -type f -name '*.[odt]' -print0 | xargs -0 -r rm


flash: $(OBJDIR)/$(PROJ_NAME).elf
	@cp $< $(OBJDIR)/main.elf
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
