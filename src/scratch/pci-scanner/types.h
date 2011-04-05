#ifndef TYPES_H
#define TYPES_H

#define NULL 0x00
#define true 0
#define false 1


typedef signed char		int8_t;
typedef signed short	int16_t;
typedef long			int32_t;
typedef long long		int64_t;

typedef	unsigned char		uint8_t;
typedef	unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef uint32_t		uint_t;

/*
** hardware-sized types
*/

typedef uint8_t		byte_t;
typedef uint16_t	word_t;
typedef uint32_t	dword_t;
typedef uint64_t	qword_t;

/*
** miscellaneous types
*/

typedef uint8_t		boolean_t;

typedef int32_t	status_t;

#define E_TIMEOUT -3
#define E_TOO_MANY_TRIES -2
#define E_ERROR -1
#define E_SUCCESS	0
#define E_BAD_PARAM	1
#define E_NOT_IMPLEMENTED 2
#define E_NOT_FOUND	3
#define E__INSUFFICIENT_RESOURCES 4

typedef struct link_node{
	struct link_node_t* prev;
	struct link_node_t* next;
	void* data;
} link_node_t;

#endif	//TYPES_H
