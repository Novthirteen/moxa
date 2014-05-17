/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
#include "set_ipconfig.h"

#define MAX_IFACES 8 /* assume a maximum # of interfaces */
#ifdef _WIN32_WCE
#define NUM_FIELD	3
#else
#define NUM_FIELD	5
#endif

#if 0
enum
{
	INDEX_IPADDR,
	INDEX_NETMASK,
	INDEX_NETWORK,
	INDEX_BROADCAST,
	INDEX_GATEWAY,
};
#endif

/* about a network interface */
typedef struct _IPADDR
{
    char ifname[16]; /* the name of the interface */
    unsigned char addr[4][3]; /* ip address in digits */
    int digit_i;	/* sub address 0-3 */
	int digit_j;	/* position in sub address 0-2 */
	unsigned int index; /* could be IP, maks, gateway, ... */
} IPADDR;

/* global structures */
static IPADDR gIPAddrs[NUM_FIELD][MAX_IFACES];

static char *gTitles[] = {
	"IPAddr",
	"Netmask",
#ifndef _WIN32_WCE
	"Network",
	"Broadcast",
#endif
	"Gateway",
};

/*  convert IP address to string style 
    Inputs:
        <addr> the underlying IP address
*   Outputs:
*       <text> address in string
*/
static void
ipconfig_ntoa(unsigned char addr[4][3], char *text)
{
    int i, len=0;

    for (i=0; i < 4; i++)
    {
       sprintf(text+len, "%d.", addr[i][0]*100+addr[i][1]*10+addr[i][2]);
       len = strlen(text);
    }
	text[len-1] = 0;
}

/* flush content to the LCM */
void
ipconfig_flush2lcm(LCMDAT *lcm, IPADDR *ip, char *msg)
{
#ifdef UC74XX /* UC-74xx */
	int j;
#endif

    sprintf(lcm->text[0], "%s: %s", ip->ifname, gTitles[ip->index]);
    if (msg)
        sprintf(lcm->text[1], "%s", msg);
    else
    {
		int i;

		/* convert IP to a string style shown on LCM */
		for (i=0; i < 4; i++)
			sprintf(lcm->text[1]+4*i, "%d%d%d.", 
				ip->addr[i][0],ip->addr[i][1],ip->addr[i][2]);
		lcm->text[1][15] = 0;
    }
#ifdef UC74XX /* UC-74xx */
	for (j = 2; j < MAX_LCM_ROWS; j++) {
		memset(lcm->text[j], ' ', MAX_LCM_COLS);
		lcm->text[j][MAX_LCM_COLS] = 0;
	}
#endif
    lcm->cursor_y = 1;
    lcm->cursor_x = ip->digit_i*4+ip->digit_j;
}

/* convert IP address in string style to number style */
static void
ipconfig_aton(char *text, unsigned char addr[4][3])
{
    unsigned char val, buf[32];
    char *p, *hdr;
    int i;

    strcpy(buf, text);
    p = hdr = buf;
    for (i=0; i < 4; i++)
    {
	p = strchr(hdr, '.');
	if (p) *p = 0;
	val = atoi(hdr);
	addr[i][2] = val%10;
	val /= 10;
	addr[i][1] = val%10;
	addr[i][0] = val/10;
	if(!p) break;
	hdr = p+1;
    }
}

static char *
ipconfig_get_ipaddr(unsigned int index, MXIFACE *iface)
{
		char *ipaddr;

		switch(index)
		{
		case INDEX_NETMASK:
			ipaddr = iface->netmask;
			break;
#ifndef _WIN32_WCE
		case INDEX_NETWORK:
			ipaddr = iface->network;
			break;
		case INDEX_BROADCAST:
			ipaddr = iface->broadaddr;
			break;
#endif
		case INDEX_GATEWAY:
			ipaddr = iface->gateway;
			break;
		default:
			ipaddr = iface->ipaddr;
			break;
		}
		return ipaddr;
}

/* KEY_MENU: get the IP address
 */
static void
ipconfig_get(LCMDAT *lcm, void *data)
{
    IPADDR *ip = (IPADDR*) data;
    MXIFACE iface;
	char *ipaddr;

    ip->digit_i = ip->digit_j = 0;
#ifdef _WIN32_WCE
    if (mxiface_get_info_static(ip->ifname, &iface, 1)>0)
#else
    if (mxiface_get_info_from_file(ip->ifname, &iface, 1)>0)
#endif
    {
		ipaddr = ipconfig_get_ipaddr(ip->index, &iface);
        ipconfig_aton(ipaddr, ip->addr);
        ipconfig_flush2lcm(lcm, ip, NULL);
    }
}

/* KEY_UP: increase the value of a digit
*/
static void
ipconfig_digit_up(LCMDAT *lcm, void *data)
{
    IPADDR *ip = (IPADDR*) data;
    unsigned int max; 
	
    if (ip->digit_j==0)
        max = 2;
    else
    {
        if (ip->addr[ip->digit_i][0]==2)
			max = 5;
		else
			max = 9;
    }
	if (ip->addr[ip->digit_i][ip->digit_j] == max)
		ip->addr[ip->digit_i][ip->digit_j] = 0;
	else
		ip->addr[ip->digit_i][ip->digit_j]++;

    ipconfig_flush2lcm(lcm, ip, NULL);
}

/* KEY_DOWN: shift the cursor to a digit
*/
static void
ipconfig_digit_shift(LCMDAT *lcm, void *data)
{
    IPADDR *ip = (IPADDR*) data;

	if (ip->digit_j==2)
	{
		ip->digit_i++;
		if (ip->digit_i == 4) ip->digit_i = 0;
		ip->digit_j = 0;
	}
	else
		ip->digit_j++;

    ipconfig_flush2lcm(lcm, ip, NULL);
}

/*  KEY_SEL: set the ip address  
*/
static void
ipconfig_set(LCMDAT *lcm, void *data)
{
    IPADDR *ip = (IPADDR*) data;
    MXIFACE iface;
    unsigned int i, val;

    ip->digit_i = ip->digit_j = 0;
    for (i=0; i < 4; i++)
    {
		val = ip->addr[i][0]*100 + ip->addr[i][1]*10 + ip->addr[i][2];
		if (val>255)
		{
			ipconfig_flush2lcm(lcm, ip, "value overflow");
			return;
		}
    }
#ifdef _WIN32_WCE
    if (mxiface_get_info_static(ip->ifname, &iface, 1)>0)
#else
    if (mxiface_get_info_from_file(ip->ifname, &iface, 1)>0)
#endif
    {
		char *ipaddr;

		ipaddr = ipconfig_get_ipaddr(ip->index, &iface);
       ipconfig_ntoa(ip->addr, ipaddr);
       /* write setting into the system */
#ifndef _WIN32_WCE
       mxiface_update_info(&iface);
#endif
       mxiface_update_interface_file(&iface);
    }
    ipconfig_flush2lcm(lcm, ip, NULL);
}

/* intialize a menu operation
 * Inputs:
 *     <hndl> the handler to LCM/keypad 
 *     <index> which ip is taken case
 * Returns:
 *     0 on success
 */
int
ipconfig_menu_init(LCMKPD *hndl, unsigned int index)
{
    int i, max;
	lcm_show_t funs[4];
    MXIFACE gIPIfaces[MAX_IFACES];

	if (index > 4)
		return -2;
    /* get network interfaces */
    memset(gIPIfaces, 0, MAX_IFACES*sizeof(MXIFACE));
#ifdef _WIN32_WCE
	max = mxiface_get_info_static(NULL, gIPIfaces, MAX_IFACES);
#else
    max = mxiface_get_info_from_file(NULL, gIPIfaces, MAX_IFACES);
#endif
    if (max < 0)
    {
        printf("fail to get interfaces\n");
        return -1;
    }

	funs[0] = ipconfig_get;
	funs[1] = ipconfig_set;
	funs[2] = ipconfig_digit_shift;
	funs[3] = ipconfig_digit_up;

    /* based on interfaces, each carries an IPADDR */
    for (i=0; i < max ; i++)
    {
        strcpy(gIPAddrs[index][i].ifname, gIPIfaces[i].ifname);
		if (keypad_lcm_menu_add(hndl, funs, &gIPAddrs[index][i]))
			return -1;
		gIPAddrs[index][i].index = index;
	}
	return 0;
}
