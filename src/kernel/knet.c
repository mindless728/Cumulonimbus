#include <kernel/hosts.h>
#include <kernel/knet.h>
#include "headers.h"
#include "utils.h"
#include "process.h"


status_t _messages_avail;

message_t _message_temp;
message_table_t _messages[NUM_MESSAGES];


status_t _knet_init(void){
	int i;

	for(i=0; i<NUM_MESSAGES; i++){
		_messages[i].avail = TRUE;
	}

	return E_SUCCESS;
}


status_t _knet_process_frame(ethframe_t* frame){
	if(frame->header.proto == PROTO_CTP){
		ctp_header_t* ctp_hdr = (ctp_header_t*) &frame->data[0];

		if(ctp_hdr->type == CTP_IPC && ctp_hdr->length >= MESSAGE_HDR_LEN){
			message_t* temp = (message_t*) &frame->data[(sizeof(ctp_header_t))];

			char* name = _hosts_get_hostname(&temp->sender.host);
			if(name != NULL){
				c_printf("DEBUG: knet - Got CTP packet from host: %s\n", name);
			}
			else{
				c_printf("DEBUG: knet - Received CTP packet\n");
			}


			if(memcmp(&temp->dest.host, &_local_mac, sizeof(mac_address_t)) == 0){
				c_printf("DEBUG: knet - MINE\n");
			}
			else{
				//Not for me
				c_printf("DEBUG: knet - NOT MINE\n");
			}
		}
	}
	//else{
		char* name = _hosts_get_hostname((mac_address_t*) &frame->header.source);
		if(name != NULL){
			c_printf("DEBUG: knet - Got eth frame from host: %s\n", name);
		}
		else{
			c_printf("DEBUG: knet - Received eth frame from mac: 0x");

			int i;
			for(i=0; i<6; i++){
				c_printf("%x ", frame->header.source[i]);
			}
			c_printf("\n");
		}

	//}
	
	return E_SUCCESS;
}
