#ifndef	_CUMULONIMUBS_STATUS_H
#define _CUMULONIMUBS_STATUS_H

#define STATUS_GET_STATUS(x)	(x & (0x3<<30))
#define STATUS_GET_MOD_ID(x)	((x>>15) & 0x7fff)
#define STATUS_GET_CODE(x)	(x & 0x7fff)

#define MAKE_STATUS(sev, id, code)	(status_t) ()

#define	E_SUCCESS	(0)
#define E_WARNING	(1<<30)
#define E_INFO		(2<<30)
#define E_ERROR		(3<<30)

#define E_UNDEFINED			(0x00)
#define E_BAD_PARAM			(0x01)
#define E_EMPTY				(0x02)
#define E_NOT_FOUND			(0x03)
#define E_NOT_TIMEOUT			(0x04)
#define E_NOT_IMPLEMENTED		(0x05)
#define E_TIMEOUT			(0x06)
#define E_INSUFFICIENT_RESOURCES	(0x07)
#define E_NO_QNODES			(0x08)

#endif	//_CUMULONIMUBS_STATUS_H
