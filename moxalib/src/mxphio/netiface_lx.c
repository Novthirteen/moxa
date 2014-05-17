/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    netiface_lx.c

    Routines of processing network interfaces.

    2009-01-15	CF Lin
		new release
*/
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "netiface.h"
#include "os-support.h"

#define MXIFACE_FILENAME "/etc/network/interfaces"
#define DNS_FILE		"/etc/resolv.conf"
#define DNS_TEMP		"/etc/tmp_resolv.conf"

#if 0
#define MXIFACE_FLAG_ADDRESS	1
#define MXIFACE_FLAG_BROADCAST (1<<1)
#define MXIFACE_FLAG_GATEWAY (1<<2)
#define MXIFACE_FLAG_NETMASK (1<<3)
#define MXIFACE_FLAG_NETWORK (1<<4)
#endif

#define MXIFACE_NAME_ADDRESS	"address"
#define MXIFACE_NAME_BROADCAST	"broadcast"
#define MXIFACE_NAME_GATEWAY	"gateway"
#define MXIFACE_NAME_NETMASK	"netmask"
#define MXIFACE_NAME_NETWORK	"network"

static int
mxiface_socket(void)
{
    int skfd;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (skfd < 0)
    {
        dbgprintf("socket: %s", strerror(errno));
		return -1;
    }
    return skfd;
}

/*  get interface settings
    Inputs:
        <ifname> point to the name of an interface, a value of NULL
		means getting all interfaces
    Outputs:
	<ifaces> the array of interfaces
    Returns:
        the number of interfaces, or zero value on error
*/
int
mxiface_get_info(char *ifname, MXIFACE *ifaces, int max)
{
    char buffer[1024];
    struct ifconf ifc;
    struct ifreq *ifr;
    int skfd;
    int n;
	int num;

    skfd = mxiface_socket();
    if ( skfd < 0 )
        return 0;

    ifc.ifc_len = sizeof(buffer);
    ifc.ifc_buf = buffer;
    n = ioctl(skfd, SIOCGIFCONF, &ifc);
    if (n < 0)
    {
        dbgprintf("SIOCGIFCONF: %s", strerror(errno));
        n = 0;
    }
    else
    {
        int i;
        struct sockaddr_in *sin;

        ifr = ifc.ifc_req;
        num = ifc.ifc_len / sizeof(struct ifreq);
        n = 0;
        for (i = 0; i < num && n < max; i++, ifr++)
        {
            if (strcmp(ifr->ifr_name, "lo")==0)
                continue;
    	    memset(&ifaces[n], 0, sizeof(MXIFACE));
            /* copy the interface name */
            strcpy( ifaces[n].ifname, ifr->ifr_name);
            ifr->ifr_addr.sa_family = AF_INET;
            /* get IP addresses */
            if (ioctl(skfd, SIOCGIFADDR, ifr) < 0)
                break;
            sin = (struct sockaddr_in*) &ifr->ifr_addr;
            strcpy( ifaces[n].ipaddr, inet_ntoa(sin->sin_addr));
            /* get net mask */
            if (ioctl(skfd, SIOCGIFNETMASK, ifr) < 0)
                break;
            sin = (struct sockaddr_in*) &ifr->ifr_netmask;
            strcpy( ifaces[n].netmask, inet_ntoa(sin->sin_addr));
            /* get broadcast address */
            if (ioctl(skfd, SIOCGIFBRDADDR, ifr) < 0)
	            break;
            sin = (struct sockaddr_in*) &ifr->ifr_broadaddr;
            strcpy( ifaces[n].broadaddr, inet_ntoa(sin->sin_addr));
            if (ioctl(skfd, SIOCGIFHWADDR, ifr) < 0)
                break;
            memcpy(ifaces[n].macaddr, ifr->ifr_hwaddr.sa_data, 6);
            if (!ifname)
                n++;
            else if (strcmp(ifname, ifr->ifr_name)==0)
            {
                n = 1;
                break;
            }
        }
    }
    close(skfd);
    return n;
}

int
mxiface_set_mac_address(char *ifname, unsigned char *mac)
{
    int skfd;
    struct ifreq ifr;

    skfd = mxiface_socket();
    if (skfd < 0)
        return -1;
    strcpy(ifr.ifr_name, ifname);
    ifr.ifr_addr.sa_family = AF_INET;
    memcpy(ifr.ifr_hwaddr.sa_data, mac, 6);
    if (ioctl(skfd, SIOCSIFHWADDR, &ifr) < 0)
    {
        dbgprintf("ioctl: %s", strerror(errno));
		close(skfd);
		return -2;
    }
    close(skfd);
    return 0;
}

int
mxiface_get_mac_address(char *ifname, unsigned char *mac)
{
    int skfd;
    struct ifreq ifr;

	if (ifname == NULL)
	{
		MXIFACE ifaces;

		if (mxiface_get_info(ifname, &ifaces, 1))
		{
			memcpy(mac, ifaces.macaddr, 6);
			return 0;
		}
		else
			return -3;
	}
    skfd = mxiface_socket();
    if (skfd < 0)
		return -1;
    strcpy(ifr.ifr_name, ifname);
    ifr.ifr_addr.sa_family = AF_INET;
    if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
    {
        dbgprintf("ioctl: %s", strerror(errno));
		close(skfd);
		return -2;
    }
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    close(skfd);
    return 0;
}

/* Get the interface configuration from the kernel.
    Inputs:
        <ifname> the name of the interface
    Outputs:
        <ipaddr> IP address
    Returns:
        0 on success, otherwise on failure
*/
int
mxiface_update_info(MXIFACE *iface)
{
    int skfd;
    struct ifreq ifr;
    struct sockaddr_in *sin;

    skfd = mxiface_socket();
    if (skfd < 0)
	return -1;

    strcpy( ifr.ifr_name, iface->ifname );
    ifr.ifr_addr.sa_family = AF_INET;
    sin = (struct sockaddr_in*) &ifr.ifr_addr;
    sin->sin_addr.s_addr = inet_addr(iface->ipaddr);
    if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0)
    {
		dbgprintf("fail to change IP %s", iface->ipaddr);
        goto err;
    }
    sin = (struct sockaddr_in*) &ifr.ifr_netmask;
    sin->sin_addr.s_addr = inet_addr(iface->netmask);
    if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0)
    {
		dbgprintf("fail to change netmask %s", iface->ipaddr);
        goto err;
    }
    sin = (struct sockaddr_in*) &ifr.ifr_broadaddr;
    sin->sin_addr.s_addr = inet_addr(iface->broadaddr);
    if (ioctl(skfd, SIOCSIFBRDADDR, &ifr) < 0)
    {
		dbgprintf("fail to change broadcast %s", iface->ipaddr);
        goto err;
    }
    close(skfd);
    return 0;
err:
    dbgprintf("ioctl: %s", strerror(errno));
    close(skfd);
    return -1;
}

static char*
mxiface_field_lookup(MXIFACE *iface, char *name)
{
	if (strcmp(name, MXIFACE_NAME_ADDRESS)==0)
		return iface->ipaddr;
	else if (strcmp(name, MXIFACE_NAME_NETMASK)==0)
		return iface->netmask;
	else if (strcmp(name, MXIFACE_NAME_GATEWAY)==0)
		return iface->gateway;
	else if (strcmp(name, MXIFACE_NAME_BROADCAST)==0)
		return iface->broadaddr;
	else if (strcmp(name, MXIFACE_NAME_NETWORK)==0)
		return iface->network;
	else
		return NULL;
}

static int
mxiface_split_line(char **av, int max, char *str)
{
    int num = 0;

    while(num < max)
    {
        while(*str && isspace(*str)) str++;
	if (*str==0)
	    break;
	av[num++] = str;
        while(*str && !isspace(*str)) str++;
	if (*str==0)
	    break;
	*str = 0;
	str++;
    }
    return num;
}
// Carl : TODO remove path. doesn't get all info ?. when the if is first ?.
/*  get network settings of an interface from the file
    Inputs:
        <ifname> the name of the interface
	<iface> setting data structure
*/
int
mxiface_get_info_from_file(char *ifname, MXIFACE *ifaces, int max)
{
#ifdef UC711X
	return 0;
#else
    FILE *fp;
    char buffer[256], tmpbuf[256];
    char *av[2];
    int index=-1;

    memset(ifaces, 0, max*sizeof(MXIFACE));

    if( (fp=fopen(MXIFACE_FILENAME, "r"))==NULL )
    {
        dbgprintf("fail to open interfaces file");
        return 0;
    }

    while(fgets(buffer, 256, fp))
    {
		strcpy(tmpbuf, buffer);

        if (mxiface_split_line(av, 2, tmpbuf) < 2)
			continue;

        if (strcmp(av[0], "iface") == 0 && strcmp(av[1], "lo"))
        {
            if (!ifname)
			{
                index++;
				if (index==(max-1))
					break;
			}
            else if (index==0 && strcmp(ifaces[0].ifname, ifname)==0)
                break; /* find it */
            else
                index=0;
			/* for specific ifname clear previous interface data */
			memset(&ifaces[index], 0, sizeof(MXIFACE));
            strcpy(ifaces[index].ifname, av[1]);
			if( strstr(buffer, "dhcp") != 0 )
				ifaces[index].enable_dhcp = 1;
        }
        else if (index>=0)
        {
			char *name;
			if(!ifaces[index].enable_dhcp)
			{
				name = mxiface_field_lookup(&ifaces[index], av[0]);
				if (name) strcpy(name, av[1]);
			}
		}
    }
    fclose(fp);
    return index+1;
#endif
}

static unsigned int
mxiface_update_an_interface(MXIFACE *ifr, FILE *wfp)
{
	/* write down the changes of members */
	if ( !ifr->ipaddr[0] || !ifr->netmask[0] )
	{
		return -1;
	}
	else
	{
		fprintf(wfp, "\t%s %s\n", MXIFACE_NAME_ADDRESS, ifr->ipaddr);
		fprintf(wfp, "\t%s %s\n", MXIFACE_NAME_NETMASK, ifr->netmask);
	}
	if (ifr->network[0])
		fprintf(wfp, "\t%s %s\n", MXIFACE_NAME_NETWORK, ifr->network);
	if (ifr->gateway[0] && strcmp(ifr->gateway, "0.0.0.0"))
		fprintf(wfp, "\t%s %s\n", MXIFACE_NAME_GATEWAY, ifr->gateway);
	if (ifr->broadaddr[0])
		fprintf(wfp, "\t%s %s\n", MXIFACE_NAME_BROADCAST, ifr->broadaddr);

	return 0;
}

// Carl : TODO remove ifrpath, add dhcp, check mkstemp
/*  change the settings of an interface onto file
    Inputs:
        <ifr> the interface
*/
int
mxiface_update_interface_file(MXIFACE *ifr)
{
#ifdef UC711X
	return 0;
#else
    FILE *rfp, *wfp;
    char buffer[256], tmpbuf[256];
    char tmppath[256] = "/etc/network/moxatmpXXXXXX";
    char *av[2];
    int target_if = 0, update = 0, in_auto = 0;

 	if (mkstemp(tmppath) < 0)
		return -1;

	/* copy the interface file to a tmp file */
	if (rename(MXIFACE_FILENAME, tmppath) == -1)
        return -4;

    if( (rfp=fopen(tmppath,"r"))==NULL )
    {
		return (rename(tmppath, MXIFACE_FILENAME)==-1) ? -5 : -2;
    }
    if( (wfp=fopen(MXIFACE_FILENAME, "w"))==NULL )
    {
        fclose(rfp);
        return (rename(tmppath, MXIFACE_FILENAME)==-1) ? -6 : -3;
    }

	while(fgets(buffer, sizeof(buffer), rfp))
	{
		if (buffer[0] == '#') {
			fprintf(wfp, "%s", buffer);
			continue;
		}
		/* copy one tmp buffer for data processing */
		strcpy(tmpbuf, buffer);
		if (mxiface_split_line(av, 2, tmpbuf) < 2)
		{
			/* not target entry */
			fprintf(wfp, "%s", buffer);
		}
		/* find the line with "iface" which leads a round of configuration */
		else if (strcmp(av[0], "iface") == 0)
		{
			if (strcmp(av[1], ifr->ifname)) {
			    fprintf(wfp, "%s", buffer); /* write down the header line for this interface */
				target_if = 0;
				continue;
			} else {
			    fprintf(wfp, "iface %s inet %s\n", ifr->ifname, ifr->enable_dhcp ? "dhcp":"static");
			    if( !ifr->enable_dhcp )
			    {
                    /* write down the changes of members */
                    if( mxiface_update_an_interface(ifr, wfp) < 0 )
						dbgprintf("fail to update interface %s", ifr->ifname);
			    }
                update = target_if = 1;
			}
		}
		else if (strcmp(av[0], "auto") == 0)
		{
		    fprintf(wfp, "%s", buffer);
		    if( strstr(buffer,ifr->ifname) != 0 )
                in_auto = 1;
		}
		else if (target_if == 0) /* copy to the new file */
		{
			/* not iface line or not matched iface line */
			fprintf(wfp, "%s", buffer);
		}
	}

	if( update == 0 )
	{
		/* append new interface */
		/* TODO: also update auto entry */
		if( in_auto == 0 )
            fprintf(wfp, "auto %s\n", ifr->ifname);
        fprintf(wfp, "iface %s inet %s\n", ifr->ifname, ifr->enable_dhcp ? "dhcp":"static");
        if( !ifr->enable_dhcp )
		{
			if( mxiface_update_an_interface(ifr, wfp) < 0 )
				dbgprintf("fail to update interface %s", ifr->ifname);
		}
	}
    fclose(wfp);
    fclose(rfp);
    unlink(tmppath);

    return 0;
#endif
}

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
mxhost_get_dns(char *ifname, char *dns_list, int size)
{
	FILE *fp;
	char buf[128];
	char *p, *q;
	int num = 0;
	int len;
	int unused = size - 1;

	if (dns_list == NULL || size <= 0) return -1;
	if ((fp = fopen(DNS_FILE, "r")) == NULL) return -2;

	dns_list[0] = 0;
	while (fgets(buf, sizeof(buf), fp)) {
		if (buf[0] == '#' || buf[0] == 0x20) continue;
		buf[strlen(buf)-1] = 0;
		p = buf;
		if (!strncmp(p, "nameserver", 10)) {
			p += 10; q = p;
			while (*p && *p == 0x20) p++;
			if (p == q) continue;
			q = p; /* value */
			while (*p && *p != 0x20) p++;
			*p = 0;
			len = strlen(q);
			if (len < 16) {
				if (num != 0) {
					if (unused >= len+1) {
						strcat(dns_list, " ");
						strcat(dns_list, q);
					}
				} else {
					if (unused >= len) {
						strcpy(dns_list, q);
						num = 1;
					}
				}
			}
		}
	}

	fclose(fp);
	return 0;
}

/*  Update the list of DNS servers onto a networking interface
    Inputs:
        <ifname> the name of the updated interface
		<dns_list> the list of DNS servers separated by a space
    Returns:
	    0 on success
*/
int
mxhost_update_dns(char *ifname, char *dns_list)
{
	FILE *rfp, *wfp;
	char buf[128];
	char *p, *q;

	if (dns_list == NULL) return -1;
	if ((rfp = fopen(DNS_FILE, "r")) == NULL) return -2;
	if ((wfp = fopen(DNS_TEMP, "w")) == NULL) {
		fclose(rfp);
		return -3;
	}

	while (fgets(buf, sizeof(buf), rfp)) {
		if (strncmp(buf, "nameserver", 10)) {
			fprintf(wfp, "%s", buf);
			continue;
		}
	}
	for (q = p = dns_list; *p; q = p) {
		while (*p && *p != 0x20) p++;
		if (!*p) break;
		*p = 0;
		fprintf(wfp, "nameserver %s\n", q);
		p++;
	}
	if (*q) {
		fprintf(wfp, "nameserver %s\n", q);
	}

	fclose(rfp);
	fclose(wfp);
	rename(DNS_TEMP, DNS_FILE);
	return 0;
}

/* added by Carl for network restart, 2009-09-07 */
int ifupdown(char* ifname, int up)
{
	struct ifconf ifc;
	struct ifreq *ifr;
	int idx;
	int skfd;
	int num;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	ifc.ifc_len = 0;
	ifc.ifc_buf = 0;
	/* get the length of ifconf */
	if (ioctl(skfd, SIOCGIFCONF, &ifc) < 0) {
		close(skfd);
		return -1;
	}

	ifc.ifc_buf = calloc(ifc.ifc_len, 1);
	/* get the content of ifs */
	if (ioctl(skfd, SIOCGIFCONF, &ifc) < 0) {
		close(skfd);
		return -1;
	}

	ifr = ifc.ifc_req;
	num = ifc.ifc_len / sizeof(struct ifreq);
	for( idx=0; idx<num; idx++,ifr++)
	{
		if ( (strcmp(ifr->ifr_name, "lo")==0) || (strchr(ifr->ifr_name,':')!=NULL))
			continue;

		if ( (ifname && strcmp(ifr->ifr_name, ifname)==0) || !ifname)
		{
			if( up )
				ifr->ifr_flags |= (IFF_UP | IFF_MULTICAST);
			else
				ifr->ifr_flags &= ~(IFF_UP);

			if (ioctl(skfd, SIOCSIFFLAGS, ifr) < 0) {
				close(skfd);
				return -1;
			}
		}
	}
	close(skfd);

	return 0;
}

int
mxiface_restart(void)
{
	ifupdown(NULL, 0);
	ifupdown(NULL, 1);
	return 0;
}
