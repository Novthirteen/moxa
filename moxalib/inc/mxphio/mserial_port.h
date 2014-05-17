/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef MSERIAL_PORT_H
#define MSERIAL_PORT_H

#include "os-support.h"

#if !defined(_WIN32_WCE) && !defined(WIN32) 
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termio.h>
#include <sys/termios.h>
#include <sys/ioctl.h>

#define COM_PORT_SELECTABLE 1

/*
	Modem Status Flags
*/
#define MS_CTS_ON			0x0010
#define MS_DSR_ON			0x0020
#define MS_RING_ON			0x0040
#define MS_RLSD_ON			0x0080

#define mxsp_close	close

#else
#include <windows.h>

#define COM_PORT_SELECTABLE 0

#endif

#define MOXA_SET_OP_MODE                (0x400+66)
#define MOXA_GET_OP_MODE                (0x400+67)
#define UC_SET_SPECIAL_BAUD_RATE        (0x400+68)
#define UC_GET_SPECIAL_BAUD_RATE        (0x400+69)
#define MOXA_SET_SPECIAL_BAUD_RATE      (0x400+100)
#define MOXA_GET_SPECIAL_BAUD_RATE      (0x400+101)

enum {
	MSP_PURGE_RX,
	MSP_PURGE_TX,
	MSP_PURGE_RXTX,
};

/*	serial device interface		*/
enum 
{
	MSP_RS232_MODE,
	MSP_RS485_2WIRE_MODE,
	MSP_RS422_MODE,
	MSP_RS485_4WIRE_MODE
};

enum {
	MSP_PARITY_NONE,
	MSP_PARITY_ODD,
	MSP_PARITY_EVEN,
	MSP_PARITY_SPACE,
	MSP_PARITY_MARK
};

enum {
	MSP_FLOWCTRL_NONE,
	MSP_FLOWCTRL_SW,
	MSP_FLOWCTRL_HW
};

#ifdef __cplusplus
extern "C" {
#endif

int mxsp_read(unsigned int fd, char *, int, void *);
int mxsp_write(unsigned int fd, char *, int, void *);

unsigned int mxsp_open(int);
#ifdef WIN32
int mxsp_close(unsigned int fd);
#endif

int mxsp_purge_buffer(unsigned int fd, int);

/* set communication settings */
int mxsp_set_baudrate(unsigned int fd, int );
int mxsp_set_databits(unsigned int fd, int );
int mxsp_set_stopbits(unsigned int fd, int );
int mxsp_set_parity(unsigned int fd, int );
int mxsp_set_interface(unsigned int fd, int );
int mxsp_set_flow_control(unsigned int fd, int );
/* get communication settings */
int mxsp_get_baudrate(unsigned int fd);
int mxsp_get_databits(unsigned int fd);
int mxsp_get_stopbits(unsigned int fd);
int mxsp_get_parity(unsigned int fd);
int mxsp_get_interface(unsigned int fd);
int mxsp_get_flow_control(unsigned int fd);
int mxsp_get_max_port(void);

int	mxsp_inqueue(unsigned int fd);
int mxsp_outqueue(unsigned int fd);

int mxsp_set_flow_ctrl(unsigned int fd ,int cts, int rts, int stx, int srx);
int mxsp_set_io_lctrl(unsigned int fd, unsigned int dwFunc);
int mxsp_get_lstatus(unsigned int fd, unsigned int *lstat);
int mxsp_set_xonxoff(unsigned int fd ,char xon, char xoff);
int mxsp_set_break(unsigned int fd);
int mxsp_clear_break(unsigned int fd);
int mxsp_get_errors(unsigned int fd, unsigned int *err);

#ifdef __cplusplus
}
#endif

#endif /* MSERIAL_PORT_H */
