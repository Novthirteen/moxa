
MXDIR = ../..
INCDIR = -I$(MXDIR)/inc/mxphio
LIBDIR = $(MXDIR)/lib

include $(MXDIR)/rules.make

SRCS = \
       keypad_callback.c \
       keypad_lcm.c \
       mserial_port_lx.c \
       mxbuzzer_lx.c \
       mxdgio_lx.c \
       mxgpio_lx.c \
       mxkeypad_lx.c \
       mxlcm_lx.c \
       mxrtc_lx.c \
       mxwdg_lx.c \
       netiface_lx.c \
       network.c

OBJS = $(SRCS:.c=.o)

ALL = $(LIBDIR)/libmxphio.a

DEFS = $(INCDIR)
ifeq ($(ARCH),uc74xx)
# comment for DA-660-LX
DEFS += #-DMAX_LCM_ROWS=8
DEFS += #-DMAX_KEYPADS=5
endif
ifdef DEBUG
DEFS += -DDEBUG
endif

CFLAGS += -Wall -O2 $(DEFS)

all: $(ALL)

$(LIBDIR)/libmxphio.a: $(OBJS)
	@$(RM) $@
	$(AR) $@ $(OBJS)

clean:
	@$(RM) $(OBJS) $(ALL)

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) > make.dep

include make.dep
