/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    cellular_modem.h

	Header files for SMS/GPRS modem functions

    2009-09-15	Freddy Liao
		new release
*/
#ifndef _CELLULAR_MODEM_H
#define _CELLULAR_MODEM_H

#if defined _WCE_WIN32
#include <windows.h>
#else
#endif

#define MAX_SMS_BYTES	512

/* define GPRS errors, of which each stands on one of a 32-bit number */
#define GPRS_ERROR_BAUDRATE_COM3		(1<<0)
#define GPRS_ERROR_BAUDRATE_COM4		(1<<1)
#define GPRS_ERROR_FLOWCONTROL			(1<<2)
#define GPRS_ERROR_PINCODE			(1<<3)
#define GPRS_ERROR_TEMPERATURE			(1<<4)
#define GPRS_ERROR_SIGNAL_STRENGTH		(1<<5)
#define GPRS_ERROR_RADIOBAND			(1<<6)
#define GPRS_ERROR_MODULE			(1<<7)

typedef struct _SMSMSG
{
	unsigned int	been_read;
	char	msg_date[12];
	char	msg_time[20];
	char	phone_number[20];
	unsigned int msg_length;
	char	msg_text[MAX_SMS_BYTES];
} SMSMSG, *PSMSMSG;

/*	open a cellular modem handle for later uses
	Inputs:
		none
	Returns:
		pointer to a cellular modem handle. Return 0 on failure.
*/
unsigned int cellular_modem_open(void);

/*	close a cellular modem handle
	Inputs:
		<fd> the handle
	Returns:
		none
*/
void cellular_modem_close(unsigned int fd);

/*	send an AT command to a cellular modem and wait for a reply
	Inputs:
		<fd>	the cellular modem
		<at_cmd> the AT command
		<recv_size> maximum size of the buffer that stores replied data
		<timeout> timeout in milliseconds when there is no reply
	Outpus:
		<recv> point to buffer that stores the reply
	Returns:
		0 on success, otherwise the function fails
		
*/
int cellular_modem_send_cmd(unsigned int fd, char *at_cmd, char *recv, int recv_size, int timeout);

/********************* GPRS *************************/
/*	get the signal strength of the GPRS modem. 
	Note: It is suggested to call this function periodically.
	Inputs:
		<fd> the cellular modem
	Returns:
		1 to 99 on success, otherwise the function fails
*/
int cellular_modem_gprs_get_signal_strength(unsigned int fd);

/*	establish a GPRS connection to the ISP service provider
	Inputs:
		<fd> the cellular modem
		<user> point to the user id, can be null.
		<password> point to the user password, can be null.
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_gprs_establish_connection(unsigned int fd, char *user, char *password);

/*	abort a GPRS connection
	Inputs:
		<fd> the cellular modem
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_gprs_abort_connection(unsigned int fd);

/*	check the status of a GPRS connection.
	Inputs:
		<fd> the cellular modem
	Returns:
		0 indictes the connection is on. otherwise, it is disconnected
*/
int cellular_modem_gprs_check_connection_status(unsigned int fd);

/*	diagnose the status of a GPRS connection.
	Inputs:
		<fd> the cellular modem
	Returns:
		0 indictes no error. otherwise, a 32-bit number indicating a combination of errors
*/
unsigned int cellular_modem_gprs_diagnose_status(unsigned int fd);

/********************* SMS *************************/
/*	set the storage base of SIM messages
	Inputs:
		<fd> the cellular modem
		<mode> 0: on SIM card, 1: on modem module, 2: on both
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sms_set_storage_base(unsigned int fd, int mode);

/*	get the storage base of SIM messages
	Inputs:
		<fd> the cellular modem
	Returns:
		0: on SIM card, 1: on modem module, 2: on both, otherwise, the function fails
*/
int cellular_modem_sms_get_storage_base(unsigned int fd);

/*	get the number of stored messeages out of the maximum space
	Inputs:
		<fd> the cellular modem
	Outputs:
		<maximum> the maximum number of messages allowed
	Returns:
		the number of stored messeages. otherwise, a negative value indicates a failure
*/
int cellular_modem_sms_get_message_count(unsigned int fd, int *maximum);

/*	when the SIM card status is set to PIN (2), use this function to retrieve 
	available PIN code attempt count.
	if the SIM card status is set to PUK (3), use this function to get the available 
	PUK code attempt count.
	Inputs:
		<fd> the cellular modem
	Returns:
		the attempted count left of PIN/PUK code authentication. 
		Otherwise, a negative value indicates a failure
*/
int cellular_modem_sim_get_pin_attempt_count(unsigned int fd);

/*	send a SMS message to a specific phone number
	Inputs:
		<fd> the cellular modem
		<msg_mode> 0: message in text, 1: message in PDU
		<psms> point to the message
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sms_send_message(unsigned int fd, unsigned int msg_mode, SMSMSG *psms);

/*	receive an indexed SMS message
	Inputs:
		<fd> the cellular modem
		<index> the index to the message pool
		<msg_mode> 0: message in text, 1: message in PDU
		<psms> point to the message
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sms_recv_message(unsigned int fd, int index, unsigned int msg_mode, SMSMSG *psms);

/*	delete an indexed SMS message
	Inputs:
		<fd> the cellular modem
		<index> the index to the message pool
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sms_delete_message(unsigned int fd, int index);

/********************* SIM card *************************/
/*	get the status of the SIM card
	Inputs:
		<fd> the cellular modem
	Returns:
		0 : ready, okay to use
		1 : no sim card, (or loose)
		2 : PIN, wait for the pin code for authentication
		3 : PUK, three times of wrong pin codes
		otherwise, the function fails
*/
int cellular_modem_sim_get_sim_card_status(unsigned int fd);

/*	when the SIM card status is set to PIN (2), use this function to authenticate a PIN code.
	then the status will be back to ready (0).

	Inputs:
		<fd> the cellular modem
		<pin_code> point to the PIN code
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sim_authenticate_pin_code(unsigned int fd, char *pin_code);

/*	when the SIM card status is PUK (3), use this function to change the status to PIN (2). 
	if it fails, the SIM card is out of use.
	Inputs:
		<fd> the cellular modem
		<passwd> point to the PUK passwd code
		<new_pin_code> point to a new PIN code
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sim_unlock_pin_code(unsigned int fd, char *passwd, char *new_pin_code);


/********************* SIM card *************************/
/*	get the PIN code enable status of the SIM card
	Inputs:
		<fd> the cellular modem
	Returns:
		0 : PIN code disabled
		1 : PIN code enabled
		otherwise, the function fails
*/
int cellular_modem_sim_get_pin_enable_status(unsigned int fd);

/*	when the SIM card status is ready (0) and the PIN code is enabled, assign a PIN code to the SIM card
	Inputs:
		<fd> the cellular modem
		<old_pin_code> point to the old PIN code
		<new_pin_code> point to the new PIN code
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sim_assign_pin_code(unsigned int fd, char *old_pin_code, char *new_pin_code);

/*	when the SIM card status is ready (0), remove the current PIN code
	Inputs:
		<fd> the cellular modem
		<pin_code> point to the PIN code password
		<enable> 1: enable PIN code, 0: disable PIN code
	Returns:
		0 on success, otherwise, the function fails
*/
int cellular_modem_sim_enable_pin_code(unsigned int fd, char *pin_code, int enable);



#endif
