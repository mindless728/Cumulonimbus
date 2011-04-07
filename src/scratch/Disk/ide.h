#ifndef IDE_H
#define IDE_H

#include "types.h"
#include "../pci-scanner/pci.h"

#include "ide_defines.h"

typedef struct _ide_channel_t ide_channel_t;
typedef struct _ide_device_t ide_device_t;

struct _ide_channel_t {
	pci_device_t * controller;
	unsigned short base_io_register;
	unsigned short base_control_register;
	unsigned short bus_master_base;
};

struct _ide_device_t {
	ide_channel_t *	ide_channel; // channel the device sits on
	unsigned char	valid;		 // 1 - valid, 0 - invalid
	unsigned char	drive;       // 0 (Master Drive) or 1 (Slave Drive).
	unsigned short	type;        // 0: ATA, 1:ATAPI.
	unsigned short	signature;   // Drive Signature
	unsigned short	capabilities;// Features.
	unsigned int	command_sets; // Command Sets Supported.
	unsigned int	size;        // Size in Sectors.
	unsigned char	model[41];   // Model in string.
};

extern unsigned int num_devices;
extern unsigned int num_channels;

extern ide_channel_t ide_channels[ATA_MAX_IDE_CHANNELS];
extern ide_device_t ide_devices[ATA_MAX_IDE_DEVICES];

status_t ide_init(pci_device_list_t * list); //initializes the ide driver
status_t ide_register_read(ide_device_t * device, unsigned char reg, unsigned char * ret); //reads an ide register
status_t ide_register_write(ide_device_t * device, unsigned char reg, unsigned char out); //writes an ide register

#endif