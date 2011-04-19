#ifndef ETHERNET_H
#define ETHERNET_H

#include "pci.h"
#include "types.h"

#define ETH_ALEN        6               /* Octets in one ethernet addr   */
#define ETH_HLEN        14              /* Total octets in header. */
#define ETH_ZLEN        60              /* Min. octets in frame */
#define ETH_DATA_LEN    1500            /* Max. octets in payload */



#define IP_PROTO_ICMP		(0x01)
#define IP_PROTO_UDP		(0x11)
#define IP_PROTO_TCP		(0x06)

/*
 *	This is an Ethernet frame header.
 */

typedef struct ethhdr{
	uint8_t dest[ETH_ALEN];		//! Destination ethernet address
	uint8_t source[ETH_ALEN];	//! Source ethernet address
	uint16_t proto;				//! Packet type ID field
} __attribute__((__packed__)) ethhdr_t;


typedef struct ethframe{
	ethhdr_t header;
	uint8_t data[ETH_DATA_LEN];
} __attribute__((__packed__)) ethframe_t;


typedef struct ctp_header{
	uint16_t length;
	uint32_t type;
	uint32_t dest_handle;
	uint32_t src_handle;
} __attribute__((__packed__)) ctp_header_t;

typedef struct ipv4_header{
	//TODO
} __attribute__((__packed__)) ipv4_header_t;


typedef struct udp_header{
	ipv4_header_t ip_hdr;
	uint16_t dest_port;
	uint16_t source_port;

	uint16_t checksum;
	uint16_t length;

} __attribute__((__packed__)) udp_header_t;


uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);


#endif	//ETHERNET_H
