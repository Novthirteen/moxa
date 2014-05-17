
ARCHS = uc74xx da66x ia24x uc711x uc84xx x86

################ UC74XX series ####################################
ifeq ($(ARCH),uc74xx)
# UC-74XX-LX and DA-660-LX
CROSS_COMPILE=mxscaleb
CFLAGS=-DWORDS_BIGENDIAN -DUC74XX
endif
################ DA66X series ####################################
ifeq ($(ARCH),da66x)
# DA-66X-LX
CROSS_COMPILE=xscale_be
CFLAGS=-DWORDS_BIGENDIAN -DDA66X
endif
################ UC711X series ####################################
ifeq ($(ARCH),uc711x)
# UC-7101-LX, UC-7110-LX, UC-7112-LX
CROSS_COMPILE=arm-elf
CFLAGS=-DUC711X
LDFLAGS += -Wl,-elf2flt="-z -r -s 131072"
endif
################# IA24X series ###################################
ifeq ($(ARCH),ia24x)
# IA24X-LX, W3XX-LX, IA26X-LX
CROSS_COMPILE=arm-linux
CFLAGS=-DIA24X
endif
################# UC84XX series ###################################
ifeq ($(ARCH),uc84xx)
# UC-841X-LX
CROSS_COMPILE=xscale-linux
CFLAGS=-DWORDS_BIGENDIAN -DUC84XX
endif
####################################################

ifeq ($(ARCH),x86)
# DA-68X-LX
                        CC = gcc
                     STRIP = strip -s
                        AR = ar rcs
else
                        CC = $(CROSS_COMPILE)-gcc
                     STRIP = $(CROSS_COMPILE)-strip -s
                        AR = $(CROSS_COMPILE)-ar rcs
endif
                        RM = rm -f

