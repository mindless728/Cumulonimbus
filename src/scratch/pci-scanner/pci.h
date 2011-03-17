#ifndef PCI_H
#define PCI_H

#include "types.h"

#define PCI_CONFIG_ADDRESS	0xCF8
#define PCI_CONFIG_DATA		0xCFC

#define PCI_TYPE0_MAX_OFFSET 0x3c
#define PCI_TYPE1_MAX_OFFSET 0x3c
#define PCI_TYPE2_MAX_OFFSET 0x44

#define PCI_BAR_INFO_MASK 0x0000000f
#define PCI_BAR_ADDR_MASK 0xfffffff0

//Command register bit masks
//Bits 11 through 15 are reserved
#define PCI_CMD_INTERRUPT_DISABLE			(1<<10)
#define PCI_CMD_FAST_BACK_TO_BACK_EN		(1<<9)
#define PCI_CMD_SERR_EN						(1<<8)
//Reserved bit 7
#define PCI_CMD_PARITY_ERROR_EN				(1<<6)
#define PCI_CMD_VGA_PALETTE_SNOOP			(1<<5)
#define PCI_CMD_MWI_EN						(1<<4)
#define PCI_CMD_SPECIAL_CYCLES				(1<<3)
#define PCI_CMD_BUS_MASTER					(1<<2)
#define PCI_CMD_MEMORY_SPACE				(1<<1)
#define PCI_CMD_IO_SPACE					(0x01)



//Status register bit masks
#define PCI_STATUS_PARITY_ERROR				(1<<15)
#define PCI_STATUS_SIGNALED_SYS_ERROR		(1<<14)
#define PCI_STATUS_RX_MASTER_ABORT			(1<<13)
#define PCI_STATUS_RX_TARGET_ABORT			(1<<12)
#define PCI_STATUS_SIGNALED_TARGET_ABORT	(1<<11)
#define PCI_STATUS_DEVSEL_TIMING			((1<<10) | (1<<9))
#define PCI_STATUS_MASTER_DATA_PARITY_ERROR	(1<<8)
#define PCI_STATUS_FAST_BACK_TO_BACK_CAPABLE	(1<<7)
//Bit six is reserved
#define PCI_STATUS_66MHZ_CAPABLE			(1<<5)
#define PCI_STATUS_CAPABILITIES_LIST		(1<<4)
#define PCI_STATUS_INTERRUPT_STATUS			(1<<3)
//Bits 0 through 2 are reserved

#define PCI_COMMAND_REG	0x04
#define PCI_STATUS_REG	0x06

typedef struct pci_addr{
	uint8_t bus;		//8 bits
	uint8_t slot;		//6 bits
	uint8_t func;		//2 bits
	uint8_t offset;	//6 bits
} pci_addr_t;

typedef struct pci_config{

	uint16_t vendorId;
	uint16_t deviceId;

	uint16_t command;
	uint16_t status;


	uint8_t revisionId;
	uint8_t progIF;
	uint8_t subClass;
	uint8_t classCode;

	uint8_t cacheLineSize;
	uint8_t latencyTimer;
	uint8_t headerType;
	uint8_t bist;

	union{
		uint32_t header[14];
		struct {	//48 bytes
			uint32_t bar0;
			uint32_t bar1;
			uint32_t bar2;
			uint32_t bar3;
			uint32_t bar4;
			uint32_t bar5;
			uint32_t cardbus_cis;

			uint16_t systemVendorId;
			uint16_t systemId;

			uint32_t romExpansionBar;

			uint8_t capabilitiesPtr;
			uint8_t reserved[7];

			uint8_t irqLine;
			uint8_t irqPin;
			uint8_t minGrant;
			uint8_t maxLatency;
		}type0;

		/*struct {
			uint32_t bar0;
			uint32_t bar1;
		} type1;*/
	} headers;


} pci_config_t;

typedef struct pci_device{
	pci_addr_t address;
	pci_config_t config;
	uint32_t memory_space;
	struct pci_device* next;
	struct pci_device* prev;
} pci_device_t;

typedef struct pci_device_list{
	uint32_t size;
	struct pci_device* first;
	struct pci_device* last;
} pci_device_list_t;

status_t _pci_alloc_device(pci_device_t** dev);
status_t _pci_alloc_device_list(pci_device_list_t** list);

status_t _pci_append_device(pci_device_list_t* list, pci_device_t* device);

status_t _pci_scan(pci_device_list_t* device_list);

void _pci_print_config(pci_device_t* device);

/**
  *	Searches the PCI bus looking for a device which has the same vendorId and
  *	deviceId as that provided in the config parameter. If the device is found
  *	zero is returned and the provided PCIConfig and PCIAddr structures will be
  *	properly filled in.
  *
  *	@param	config	PCIConfig structure to fill in.(input/output)
  *	@param	addr	PCI bus address(output)
  *
  *	@return	Returns 0 zero if the device was located, alse the configuration
  *			and address structures will be filled in. If the device could not
  *			be found using the vendor and device IDs then -1 is returned.
  */
//status_t _pci_find_device(pci_device_t* device);

//status_t _pci_read_byte(uint8_t configAddr, pci_addr_t addr, uint8_t* value);

//status_t _pci_read_short(uint8_t configAddr, pci_addr_t addr, uint16_t* value);

status_t _pci_read_long(boolean_t configAddr, pci_addr_t addr, uint32_t* value);

status_t _pci_write_long(boolean_t configAddr, pci_addr_t addr, uint32_t value);

status_t _pci_read_config(pci_addr_t addr, pci_config_t* config);

status_t _pci_read_bar_size(pci_device_t* device, uint32_t* size);

//status_t pciUpdateStatus(struct pci_device_t* device);

#endif	//PCI_H
