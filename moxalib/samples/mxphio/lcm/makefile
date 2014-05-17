
MXDIR = ../../..
INCDIR = -I$(MXDIR)/inc/mxphio
LIBS = -L$(MXDIR)/lib -lmxphio

include $(MXDIR)/rules.make

EXEC=lcm_clock

SRCS=lcm_clock.c
OBJS=$(SRCS:.c=.o)

DEFS = $(INCDIR)
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

all: $(EXEC)

lcm_clock: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

clean:
	$(RM) $(OBJS) $(EXEC) *.gdb
