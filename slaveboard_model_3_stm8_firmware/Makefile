## Select one of these
DEVICE=stm8s003f3
#DEVICE=stm8s103f3

## A directory for common include files
LIBRARY=spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver

## Get program name from enclosing directory name
PROGRAM = $(lastword $(subst /, ,$(CURDIR)))

SOURCES=$(wildcard *.c $(LIBRARY)/src/*.c)
OBJECTS=$(SOURCES:.c=.rel)
HEADERS=$(wildcard *.h $(LIBRARY)/inc/*.h)

CC = sdcc --opt-code-size
PROGRAMMER = stlinkv2


DEFINES= -DNO_STDPERIPH_DRIVER
## Set MCU-type DEFINE
ifeq ($(DEVICE),stm8s003f3)
    DEFINES += -DSTM8S003
endif
ifeq ($(DEVICE),stm8s103f3)
    DEFINES += -DSTM8S103
endif

CPPFLAGS = -I. -I$(LIBRARY)/inc
CFLAGS = --Werror --disable-warning 190 --disable-warning 85 --std-sdcc99 -mstm8 $(DEFINES)
LDFLAGS = -lstm8 -mstm8 --out-fmt-ihx

.PHONY: all clean flash

$(PROGRAM).ihx: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.rel : %.c $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

CCOMPILEDFILES=$(SOURCES:.c=.asm) $(SOURCES:.c=.lst) $(SOURCES:.c=.rel) \
               $(SOURCES:.c=.rst) $(SOURCES:.c=.sym)
clean:
	rm -f $(PROGRAM).ihx $(PROGRAM).cdb $(PROGRAM).lk $(PROGRAM).map $(CCOMPILEDFILES)

flash: $(PROGRAM).ihx
	stm8flash -c $(PROGRAMMER) -p $(DEVICE) -w $(PROGRAM).ihx
