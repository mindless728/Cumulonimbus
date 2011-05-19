#ifndef HOSTS_H
#define HOSTS_H

#include <types.h>
#include <kernel/ethernet.h>

typedef struct hostname_map{
	mac_address_t	address;
	char* name;
} hostname_map_t;


/*
 *	Lestrade	00:90:27:90:29:9d
 *	Addler		00:90:27:bb:d9:6a
 *	Baskerville	00:90:27:bb:d9:90
 *	Slaney		00:90:27:bb:d9:bd
 *	Beppo		00:90:27:bb:d9:cd
 *	Milverton	00:90:27:bb:d9:f8
 *	Wilson		00:90:27:bb:da:28
 *	Moriarty	00:90:27:bb:da:c0
 *	Sholto		00:90:27:bb:da:d9
 */

extern hostname_map_t _dsl_hosts[];

char* _hosts_get_hostname(mac_address_t* mac);

boolean_t _hosts_isknown(mac_address_t* mac);


#endif	//HOSTS_H

