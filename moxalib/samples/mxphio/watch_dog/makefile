
MXDIR = ../../..
INCDIR = -I$(MXDIR)/inc/mxphio
LIBS = -L$(MXDIR)/lib -lmxphio

include $(MXDIR)/rules.make

DEFS = $(INCDIR)
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

SRCS = wdg.c
OBJS = $(SRCS:.c=.o)

EXEC = wdg

all: $(EXEC)

wdg: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

clean:
	$(RM) $(OBJS) $(EXEC) *.gdb
