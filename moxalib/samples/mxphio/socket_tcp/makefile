
MXDIR = ../../..
INCDIR = -I$(MXDIR)/inc/mxphio
LIBS = -L$(MXDIR)/lib -lmxphio

include $(MXDIR)/rules.make

DEFS = $(INCDIR)
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

SRC1 = tcp_client.c
SRC2 = tcp_server.c
SRCS = $(SRC1) $(SRC2)

OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJS = $(OBJ1) $(OBJ2)

EXEC = tcp_client tcp_server

all: $(EXEC)

tcp_client: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $(OBJ1) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

tcp_server: $(OBJ2)
	$(CC) $(LDFLAGS) -o $@ $(OBJ2) $(LIBS)
ifneq ($(ARCH),uc711x)
	$(STRIP) $@
endif

clean:
	$(RM) $(OBJS) $(EXEC) *.gdb
