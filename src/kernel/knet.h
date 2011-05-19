#ifndef K_NET_H
#define K_NET_H

#include <types.h>
#include "headers.h"

#define NUM_MESSAGES 20

typedef struct message_table{
	boolean_t avail;
	message_t mesg;
} message_table_t;

extern message_table_t _messages[];

//extern queue_t _messages_avail;
//extern queue_t _messages_pending;

status_t _knet_init(void);

status_t _knet_process_frame(ethframe_t* frame);

/**
  *	Sends the provided message out of the network
  *
  *	@param	msg	Message to transmit on the network
  */
status_t _knet_send_msg(message_t* msg);

/**
  *	If there are pending messages for dest then it is copied
  *	into the supplied message buffer
  *
  *	@param	dest	Prcoess to check for pending messages
  *	@param	msg_buf	Buffer to copy message into
  *	@return	Returns a relevent status code
  */

status_t _knet_getmsg(pid_t dest, message_t* msg_buf);

/**
  *	Checks to see if there are any messages in the queue for the specified
  *	process.
  *
  *	@param	dest	Process to check for pending messages
  *	@return	Returns TRUE or FALSE to reflect if there are pending messages
  */
boolean_t _knet_has_msg(pid_t dest);

void _knet_syscall_sendmsg(context_t* context);

#endif	//K_NET_H

