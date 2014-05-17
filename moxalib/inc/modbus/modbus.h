/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    modbus.h

    MODBUS function declaration

    2009-05-08  CF Lin
        new release
*/

#ifndef _MODBUS_H
#define _MODBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#if WORDS_BIGENDIAN
#define BSWAP16(c) c
#define BSWAP32(c) c
#else
/* convert the bytes of a 16-bit integer to big endian */
#define BSWAP16(c) (((((c) & 0xff) << 8) | (((c) >> 8) & 0xff)))
/* convert the bytes of a 32-bit integer to big endian */
#define BSWAP32(c) ((((c)>>24)&0xff)|(((c)>>8)&0xff00)|(((c)<<8)&0xff0000)|((c)<<24))
#endif

#define MBRTU_SLAVE_ADDRESS(d) d[0]
#define MBRTU_FUNCTION_CODE(d) d[1]

typedef struct _MBTCPHDR 
{
	unsigned short	trans_identifier;
	unsigned short	proto_identifier;	/* 0 for MODBUS */
	unsigned short	length;
} MBTCPHDR;

/* 	digest a TCP ADU data packet into a TCP PDU structured buffer.
		ADU: <trans_identifier:2><proto_identifier:2><length:2><data>
	Inputs:
		<data> ADU data packet
		<plen> the length of data packet
	Outputs:
		<tcp> PDU header
		<plen> the # of bytes that have consumed from the data packet
	Returns:
		pointer to the data part of the TCP PDU inside the data packet
*/
unsigned char*
mbtcp_packet_digest(unsigned char *packet, unsigned int *plen, MBTCPHDR *tcp);

/* 	format data and a PDU header into an TCP ADU data packet.
		<trans_identifier:2><proto_identifier:2><length:2><data>
	Inputs:
		<tcp> PDU header
		<data> data
		<len> the length of data
	Outputs:
		<packet> ADU data packet
	Returns:
		the # of bytes of the ADU packet
*/
unsigned int
mbtcp_packet_format(MBTCPHDR *tcp, unsigned char *data, unsigned int len, unsigned char *packet);

/* 	digest a RTU ADU.
		<slave_address:1><function_code:1>[data]<crc16:2>
	Inputs:
		<packet> data packet
		<plen> the length of data packet
	Returns:
		the # of bytes in a packet excluding crc, 0 meaning incomplete/wrong crc packet
*/
unsigned int
mbrtu_packet_digest(unsigned char *packet, unsigned int plen);

/* 	format data into an ADU packet.
	Inputs:
		<data> data
		<dlen> the length of the data
	Outputs:
		<packet> ADU packet	<slave_address:1><function_code:1>[data]<crc16:2>
	Returns:
		the # of bytes of the ADU packet
*/
unsigned int
mbrtu_packet_format(unsigned char *data, unsigned int dlen, unsigned char *packet);

/* 	append a crc to data packet.
	Inputs:
		<packet> data packet <slave_address:1><function_code:1>[data]
		<dlen> the length of the data
	Outputs:
		<packet> ADU packet	<slave_address:1><function_code:1>[data]<crc16:2>
	Returns:
		the # of bytes of the ADU packet
*/
unsigned int
mbrtu_packet_append_crc(unsigned char *packet, unsigned int dlen);

/* 	digest a ASCII ADU.
		<ASCII_COMMA><hex_data><hex_LRC><ASCII_CR><ASCII_LF>
	Inputs:
		<frame> data packet
		<plen> the length of data packet
	Outputs:
		<data> PDU data <bin_data>
		<plen> the length of data that is consumed
	Returns:
		the # of bytes in a packet excluding crc, 0 meaning incomplete/wrong crc packet
*/
unsigned int
mbasc_packet_digest(unsigned char *frame, unsigned int *plen, unsigned char *data);

/* 	format an ASCII ADU.
	Inputs:
		<data> PDU data data>
		<dlen> the length of PDU data
	Outputs:
		<frame> ADU frame <ASCII_COMMA><hex_data><hex_LRC><ASCII_CR><ASCII_LF>
	Returns:
		the length of a ADU frame
*/
unsigned int
mbasc_packet_format(unsigned char *data, unsigned int dlen, unsigned char *frame);

#ifdef __cplusplus
}
#endif

#endif
