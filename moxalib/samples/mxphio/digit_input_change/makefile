
MXDIR = ../../..
INCDIR = -I$(MXDIR)/inc/mxphio
LIBS = -L$(MXDIR)/lib -lmxphio

include $(MXDIR)/rules.make

EXEC=digit_io_timer

SRCS=digit_io_timer.c main.c
OBJS=$(SRCS:.c=.o)

DEFS = $(INCDIR)
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

all: $(EXEC)

digit_io_timer: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

clean:
	$(RM) $(OBJS) $(EXEC) *.gdb
