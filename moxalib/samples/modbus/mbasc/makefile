
MXDIR = ../../..
INCDIR = -I$(MXDIR)/inc/mxconn -I$(MXDIR)/inc/mxphio -I$(MXDIR)/inc/modbus
LIBS = -L$(MXDIR)/lib -lmodbus -lmxconn -lmxphio -lpthread

include $(MXDIR)/rules.make

DEFS = $(INCDIR)
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

SRC1 = mbasc_master.c
SRC2 = mbasc_slave.c
SRCS = $(SRC1) $(SRC2)

OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJS = $(OBJ1) $(OBJ2)

EXEC = mbasc_master mbasc_slave

all: $(EXEC)

mbasc_master: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $(OBJ1) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

mbasc_slave: $(OBJ2)
	$(CC) $(LDFLAGS) -o $@ $(OBJ2) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

clean:
	$(RM) $(OBJS) $(EXEC) *.gdb
