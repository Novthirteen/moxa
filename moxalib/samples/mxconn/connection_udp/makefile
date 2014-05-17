
MXDIR = ../../..
INCDIR = -I$(MXDIR)/inc/mxconn -I$(MXDIR)/inc/mxphio
LIBS = -L$(MXDIR)/lib -lmxconn -L$(MXDIR)/lib -lmxphio -lpthread

include $(MXDIR)/rules.make

DEFS = $(INCDIR)
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

SRC1 = udp_connection_client.c
SRC2 = udp_connection_server.c
SRCS = $(SRC1) $(SRC2)

OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJS = $(OBJ1) $(OBJ2)

EXEC = udp_connection_client udp_connection_server

all: $(EXEC)

udp_connection_client: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $(OBJ1) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

udp_connection_server: $(OBJ2)
	$(CC) $(LDFLAGS) -o $@ $(OBJ2) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

clean:
	$(RM) $(OBJS) $(EXEC) *.gdb
