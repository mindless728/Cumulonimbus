#ifndef MBR_H
#define MBR_H

#include "../../include/types.h"

typedef struct _MBR_t MBR_t;
typedef struct _MBR_partition_t MBR_partition_t;
typedef struct _CHS_address_t CHS_address_t;

#define MBR_SIG 0xAA55

struct _CHS_address_t {
	uint8_t head;
	uint8_t sector:6;
	uint8_t cyl_high:2;
	uint8_t cyl_low;
};

struct _MBR_partition_t {
	uint8_t status;
	CHS_address_t start_address;
	uint8_t partition_type;
	CHS_address_t last_address;
	uint32_t first_LBA;
	uint32_t num_sectors;
};

struct _MBR_t {
	uint8_t data[446];
	MBR_partition_t partition[4];
	uint16_t MBR_signature;
} __attribute__((__packed__));

#endif