/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef KEYPAD_H
#define KEYPAD_H

#if !defined(_WIN32_WCE) && !defined(WIN32)
#ifndef MAX_KEYPADS
#define MAX_KEYPADS 4
#endif
#else
#ifdef UC74XX /* UC-74xx */
#define MAX_KEYPADS 5
#else /* DA-66X */
#define MAX_KEYPADS 4
#endif
#endif

enum 
{
    KEY_MENU,
    KEY_SEL,
    KEY_DOWN,
    KEY_UP,
#ifdef UC74XX /* UC-74xx */
	KEY_F5
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

/*	open a handle to the keypad device
	Inputs:
		None
	Returns:
		on success, a positive value for the handle
		otherwise, a negative value
*/
int	keypad_open(void);

/*	close a handle to the keypad device
	Inputs:
		<fd> the opend handle
	Returns:
		None
*/
void keypad_close(int fd);

/*	obtain which key has been pressed
	Inputs:
		<fd> the opend handle
	Returns:
		key number starting from 1, or 0 meaning no key been pressed
*/
int	keypad_get_pressed_key(int fd);

#ifdef __cplusplus
}
#endif

#endif /* KEYPAD_H */
