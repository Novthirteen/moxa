/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#include <string.h>
#include "netiface.h"

static void
mxiface_print(MXIFACE *iface)
{
    if (iface->ifname[0]) printf("Interface> %s\n",iface->ifname);
    if (iface->ipaddr[0]) printf("IP Addr >> %s\n", iface->ipaddr);
    if (iface->netmask[0]) printf("Netmask >> %s\n", iface->netmask);
    if (iface->gateway[0]) printf("Gateway >> %s\n", iface->gateway);
    if (iface->network[0]) printf("Network >> %s\n", iface->network);
    if (iface->broadaddr[0]) printf("Broadcast> %s\n", iface->broadaddr);
	if (iface->enable_dhcp) printf("DHCP    >> enabled\n");
}

static void
mxiface_print_mac_address(unsigned char *macaddr)
{
    printf("MAC %02X:%02X:%02X:%02X:%02X:%02X\n", 
		macaddr[0], macaddr[1], macaddr[2],
		macaddr[3], macaddr[4], macaddr[5]);
}

int
#if defined(_WIN32_WCE)
WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main(int argc, char *argv[])
#endif
{
	int i, max;
	MXIFACE ifaces[8];
	unsigned char mac[8];
	char dns_list[256];
#if defined(_WIN32_WCE)
	int	argc;
	char cmdline[256], *argv[32];

	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)lpCmdLine, 255, cmdline, 256, NULL, NULL);
	argc = split_line(argv+1, 32, cmdline)+1;
#endif

	/* get current interfaces */
	memset(ifaces, 0, 8*sizeof(MXIFACE));
	max = mxiface_get_info(NULL, ifaces, 8);
	printf("*** Network Interface Settings ***\n");
	for (i = 0; i < max; i++) 
	{
	    mxiface_print(&ifaces[i]);
	    mxiface_get_mac_address(ifaces[i].ifname, mac);
	    mxiface_print_mac_address(mac);
	}
	if (argc > 1)
	{
		printf("--- Network Interface Settings for %s ---\n", argv[1]);
		if (mxiface_get_info(argv[1], ifaces, 1) < 0)
		{
			printf("mxiface_get_info() error\n");
			return -1;
		}
	    mxiface_print(&ifaces[0]);
	    mxiface_get_mac_address(ifaces[0].ifname, mac);
	    mxiface_print_mac_address(mac);
	}

	/* get interfaces from file */
	memset(ifaces, 0, 8*sizeof(MXIFACE));
#ifdef _WIN32_WCE
	max = mxiface_get_info_static(NULL, ifaces, 8);
#else
	max = mxiface_get_info_from_file(NULL, ifaces, 8);
#endif
	printf("*** Network Interfaces Settings defined in file ***\n");
	for (i = 0; i < max; i++) 
	{
		mxiface_print(&ifaces[i]);
		if (!(mxhost_get_dns(ifaces[i].ifname, dns_list, sizeof(dns_list))))
			printf("DNS: %s\n", dns_list);
	}
	if (argc > 1)
	{
		printf("--- Network Interface Settings in file for %s ---\n", argv[1]);
#ifdef _WIN32_WCE
		if (mxiface_get_info_static(argv[1], ifaces, 1) < 0)
#else
		if (mxiface_get_info_from_file(argv[1], ifaces, 1) < 0)
#endif
		{
			printf("mxiface_get_info_from_file() error\n");
			return -1;
		}
		mxiface_print(&ifaces[0]);
	}

	return 0;
}
