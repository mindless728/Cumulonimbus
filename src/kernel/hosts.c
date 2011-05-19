#include <kernel/utils.h>
#include <kernel/hosts.h>


hostname_map_t _dsl_hosts[] = {
		{{{0x00,0x90,0x27,0x90,0x29,0x9d}}, "lestrade"},
		{{{0x00,0x90,0x27,0xbb,0xd9,0x6a}}, "addler"},
		{{{0x00,0x90,0x27,0xbb,0xd9,0x90}}, "baskerville"},
		{{{0x00,0x90,0x27,0xbb,0xd9,0xbd}}, "slaney"},
		{{{0x00,0x90,0x27,0xbb,0xd9,0xcd}}, "beppo"},
		{{{0x00,0x90,0x27,0xbb,0xd9,0xf8}}, "milverton"},
		{{{0x00,0x90,0x27,0xbb,0xda,0x28}}, "wilson"},
		{{{0x00,0x90,0x27,0xbb,0xda,0xc0}}, "moriarty"},
		{{{0x00,0x90,0x27,0xbb,0xda,0xd9}}, "sholto"},
		{{{0x00,0x00,0x00,0x00,0x00,0x00}}, "\0"}
	};


boolean_t _hosts_isknown(mac_address_t* mac){
	int index = 0;
	while(_dsl_hosts[index].name[0] != 0x00){

		if(memcmp(&mac->addr, &_dsl_hosts[index].address.addr, sizeof(mac_address_t)) == 0){
			return TRUE;
		}
		index++;
	}

	return FALSE;
}

mac_address_t* _hosts_get_address(char* name){
	int len = strlen(name);
	int index = 0;
	while(_dsl_hosts[index].name[0] != 0x00){
		if(strcmp(_dsl_hosts[index].name, name) == 0){
			return &_dsl_hosts[index].address;
		}
		index++;
	}
	return NULL;
}


char* _hosts_get_hostname(mac_address_t* mac){
	int index = 0;
	while(_dsl_hosts[index].name[0] != 0x00){

		if(memcmp(&mac->addr, &_dsl_hosts[index].address.addr, sizeof(mac_address_t)) == 0){
			return _dsl_hosts[index].name;
		}
		index++;
	}

	return NULL;
}

