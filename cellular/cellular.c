/*
  Copyright (C) MOXA Inc. All rights reserved.
  This software is distributed under the terms of the
  MOXA License.  See the file COPYING-MOXA for details.
*/

/******************************************************************************
File Name : buzzer.c

Description :
	The program demo how to code buzzer program on UC.

Usage :
	1.Compile this file and execute on UC7000 series.
	2.Choose the number of menu, you can hear a beep sound.

History :
	Versoin		Author		Date		Comment
	1.0		Hank		01-15-2004	Wrote it
*******************************************************************************/
//#include "buzzer.h"
#include "cellular_modem.h"

#define OK 			0
#define ERROR       -1
int main(int argc, char * argv[])
{
	int 	i;
	int    result = OK;
	unsigned int hd_cellular_modem;
	int message_count = 0;
	SMSMSG  msg_snd;


	hd_cellular_modem = cellular_modem_open();

	if(hd_cellular_modem == 0)
	{
		result = ERROR;
	}


	if(result== OK)
	{
		//cellular_modem_sms_get_message_count(hd_cellular_modem, &message_count);
		//printf("The current message count is : %d.\n", message_count);

		msg_snd.been_read = 0;
		memcpy(msg_snd.msg_date, "2014-4-1", 9);
		memcpy(msg_snd.msg_time, "11:12", 6);
		memcpy(msg_snd.phone_number,"18502154999", 12);
		msg_snd.msg_length = 7;
		memcpy(msg_snd.msg_text, "Warning!",7);
		result = cellular_modem_sms_send_message(hd_cellular_modem, 0, &msg_snd);
		if(result != 0)
		{
			printf("Message send error.\n", message_count);
			result = ERROR;
		}
	}


	//cellular_modem_close(hd_cellular_modem);

	//BuzzerInit();
	while( 1 ) {
		printf("\nSelect a number of menu, other key to exit.	\n\
	1.one short beep.						\n\
	2.two short beep.						\n\
	3.long beep.							\n\
	4.quit this program.						\n\
Choose : ");
		scanf("%d", &i);
		if ( i == 1 ) {
			//BuzzerSound(250);
		} else if ( i == 2 ) {
			//BuzzerSound(250);
			//BuzzerSound(250);
		} else if ( i == 3 ) {
			//BuzzerSound(1500);
		} else if ( i == 4 ) {
			if(hd_cellular_modem)
			{
				cellular_modem_close(hd_cellular_modem);
			}
			
			break;
		} else {
			printf("Select error, please select again !\n");
		}
		
	}

	return 0;
}
