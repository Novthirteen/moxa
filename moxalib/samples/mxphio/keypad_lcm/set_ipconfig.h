/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _IPCONFIG_H
#define _IPCONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32_WCE
#include <arpa/inet.h>
#endif
#include "netiface.h"
#include "keypad_lcm.h"

enum
{
	INDEX_IPADDR,
	INDEX_NETMASK,
#ifndef _WIN32_WCE
	INDEX_NETWORK,
	INDEX_BROADCAST,
#endif
	INDEX_GATEWAY,
};

/* intialize a menu operation
 * Inputs:
 *     <hndl> the handler to LCM/keypad 
 *     <index> which ip is taken case
 * Returns:
 *     0 on success
 */
int
ipconfig_menu_init(LCMKPD *hndl, unsigned int index);

#endif
