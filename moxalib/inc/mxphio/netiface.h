/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _NET_IFACE_H
#define _NET_IFACE_H

#if defined(_WIN32_WCE) || defined(WIN32)
#include <windows.h>
#endif

#define MAX_IFACE 16

typedef struct _MXIFACE
{
	char ifname[32];
	unsigned char macaddr[8];
	char ipaddr[16];
	char gateway[16];
	char netmask[16];
	char network[16];
	char broadaddr[16];
	char dhcpsrv[16];
	unsigned int enable_dhcp;
} MXIFACE;

#ifdef __cplusplus
extern "C" {
#endif

/*  Get the interface configuration from the kernel/file. 
    Inputs:
        <ifname> the name of the interface, NULL for all interfaces
	<max> the maximum interfaces
    Outputs:
        <ifaces> the buffer containing interface information
    Returns:
        the number of interfaces
*/
int
mxiface_get_info(char *ifname, MXIFACE *ifaces, int max);
int
#ifdef _WIN32_WCE
mxiface_get_info_static(char *ifname, MXIFACE *ifaces, int max);
#else
mxiface_get_info_from_file(char *ifname, MXIFACE *ifaces, int max);
#endif

/*  get the mac address of an interface
    Inputs:
        <ifname> the name of the interface
    Outputs:
        <mac> mac address
    Returns:
        0 on success
*/
int
mxiface_get_mac_address(char *ifname, unsigned char *mac);

#ifndef _WIN32_WCE
/*  Update the configuration of an interface.
    Inputs:
        <iface> info of the interface
    Returns:
        0 on success, otherwise on failure
*/
int
mxiface_update_info(MXIFACE *iface);
#endif

/*  Update the configuration of an interface onto the file.
    Inputs:
        <iface> info of the interface
*/
int
mxiface_update_interface_file(MXIFACE *ifr);

/*  Get the list of DNS servers from a networking interface
    Inputs:
        <ifname> the name of an interface
		<dns_list> buffer for the list of DNS servers
		<size> size of buffer for the list of DNS servers
	Outputs:
		<dns_list> the list of DNS servers separated by a space
    Returns:
	    0 on success
*/
int
mxhost_get_dns(char *ifname, char *dns_list, int size);

/*  Update the list of DNS servers onto a networking interface
    Inputs:
        <ifname> the name of the updated interface
		<dns_list> the list of DNS servers separated by a space
    Returns:
	    0 on success
*/
int
mxhost_update_dns(char *ifname, char *dns_list);

/*  restart the network interfaces 
    Returns:
        0 on success, otherwise on failure
*/
int
mxiface_restart(void);


#ifdef __cplusplus
}
#endif

#endif
