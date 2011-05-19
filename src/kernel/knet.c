#include <kernel/includes.h>
#include <kernel/hosts.h>
#include <kernel/knet.h>
#include <kernel/system.h>
#include <kernel/syscall.h>
#include "headers.h"
#include "utils.h"
#include "process.h"


status_t _messages_avail;

message_t _message_temp;
message_table_t _messages[NUM_MESSAGES];
uint32_t _next_message_idx = 0;


status_t _knet_init(void){
	int i;

	for(i=0; i<NUM_MESSAGES; i++){
		_messages[i].avail = TRUE;
	}

	_syscall_install(_knet_syscall_sendmsg, SYS_sendmsg);

	return E_SUCCESS;
}


message_t* _knet_get_avail_msg(void){
	int idx = _next_message_idx;

	do{
		if(idx >= NUM_MESSAGES){
			idx = 0;
		}

		if(_messages[idx].avail != FALSE){
			//Claim message buffer
			_messages[idx].avail = FALSE;
			return &_messages[idx].mesg;
		}

		idx++;

	} while(idx != _next_message_idx);

	return NULL;
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
				message_t* buf = _knet_get_avail_msg();
				if(buf != NULL){
					memcpy(buf, temp, sizeof(message_t));
				}
				else{
					c_printf("WARNING: knet - Packet dropped, buffers full\n");
				}
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


void _knet_syscall_sendmsg(context_t* context){
	message_t* msg = ARG(context, 1);

	_pid_cpy(&msg->sender, &_current->pid);
	context->eax = _knet_send_msg(msg);
}


status_t _knet_send_msg(message_t* msg){
	ethframe_t frame;

	if(msg == NULL){
		return E_BAD_PARAM;
	}

	//Set sender address
	memcpy(&frame.header.source, &_local_mac, 6);

	//Set dest address
	memcpy(&frame.header.dest, &msg->dest.host, 6);

	//Set proto
	frame.header.proto = PROTO_CTP;

	((ctp_header_t*) &frame.data[0])->type = CTP_IPC;

	memcpy( &frame.data[(sizeof(ctp_header_t))], msg, sizeof(message_t));

	return i8255x_driver_transmit(&frame, sizeof(ethframe_t), TRUE);
}
