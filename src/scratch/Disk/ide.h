#ifndef IDE_H
#define IDE_H

#include "../pci-scanner/types.h"
#include "../pci-scanner/pci.h"

#include "ide_defines.h"

typedef struct _ide_controller_t ide_controller_t;
typedef struct _ide_channel_t ide_channel_t;
typedef struct _ide_device_t ide_device_t;

struct _ide_controller_t {
	uint16_t VID;
	uint16_t DID;
	uint16_t PCICMD;
	uint16_t PCISTS;
	uint8_t  RID;
	uint8_t  PI;
	uint8_t  SCC;
	uint8_t  BCC;
	uint8_t  CLS;
	uint8_t  PMLT;
	uint32_t PCMD_BAR;
	uint32_t PCNL_BAR;
	uint32_t SCMD_BAR;
	uint32_t SCNL_BAR;
	uint32_t BM_BASE;
	uint16_t IDE_SVID;
	uint16_t IDE_SID;
	uint8_t  INTR_LN;
	uint8_t  INTR_PN;

	//beginning of IDE specific data
	uint16_t IDE_TIMP;
	uint16_t IDE_TIMS;
	uint8_t  SLV_IDETIME;
	uint8_t  SDMA_CNT;
	uint16_t SDMA_TIM;
	uint8_t  IDE_CONFIG;
	uint8_t  ATC;
	uint8_t  ATS;
};

struct _ide_channel_t {
	ide_controller_t * controller;
	unsigned short base_io_register;
	unsigned short base_control_register;
	unsigned short bus_master_base;
	unsigned char  primary;
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

extern unsigned int ide_num_devices;
extern unsigned int ide_num_channels;
extern unsigned int ide_num_controllers;

extern ide_controller_t ide_controllers[ATA_MAX_IDE_CONTROLLERS];
extern ide_channel_t ide_channels[ATA_MAX_IDE_CHANNELS];
extern ide_device_t ide_devices[ATA_MAX_IDE_DEVICES];

status_t ide_init(pci_device_list_t * list); //initializes the ide driver
status_t ide_register_read(ide_device_t * device, unsigned char reg, unsigned char * ret); //reads an ide register
status_t ide_register_write(ide_device_t * device, unsigned char reg, unsigned char out); //writes an ide register
status_t ide_read_ident_space(ide_device_t * device, uint8_t * buf);

status_t ide_pio_lba_read(ide_device_t * device, uint32_t sector, uint8_t * buf);
status_t ide_pio_lba_write(ide_device_t * device, uint32_t sector, uint8_t * buf);
status_t ide_pio_lba_reads(ide_device_t * device, uint32_t sector, uint8_t * buf, uint32_t num_sectors);
status_t ide_pio_lba_writes(ide_device_t * device, uint32_t sector, uint8_t * buf, uint32_t num_sectors);

#endif
