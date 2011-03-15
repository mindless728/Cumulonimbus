#ifndef PCI_H
#define PCI_H


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

struct PCIAddr{
	unsigned char bus;		//8 bits
	unsigned char slot;		//6 bits
	unsigned char func;		//2 bits
	unsigned char offset;	//6 bits
};

struct PCIConfig{

	unsigned short vendorId;
	unsigned short deviceId;

	unsigned short command;
	unsigned short status;


	unsigned char revisionId;
	unsigned char progIF;
	unsigned char subClass;
	unsigned char classCode;

	unsigned char cacheLineSize;
	unsigned char latencyTimer;
	unsigned char headerType;
	unsigned char bist;

	union{
		unsigned int header[14];
		struct {	//48 bytes
			unsigned int bar0;
			unsigned int bar1;
			unsigned int bar2;
			unsigned int bar3;
			unsigned int bar4;
			unsigned int bar5;
			unsigned int cardbus_cis;

			unsigned short systemVendorId;
			unsigned short systemId;

			unsigned int romExpansionBar;

			unsigned char capabilitiesPtr;
			unsigned char reserved[7];

			unsigned char irqLine;
			unsigned char irqPin;
			unsigned char minGrant;
			unsigned char maxLatency;
		}type0;

		/*struct {
			unsigned int bar0;
			unsigned int bar1;
		} type1;*/
	} headers;


};

struct pci_device_t{
	struct PCIAddr pciAddr;
	struct PCIConfig config;
	unsigned int memory_space;
};


void pciScan(void);

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
int pciFindDevice(struct PCIConfig* config, struct PCIAddr* addr);

void printPciConfig(struct PCIConfig* config, struct PCIAddr* addr);
unsigned char pciReadByte(unsigned char configAddr, struct PCIAddr addr);
unsigned short pciReadShort(unsigned char configAddr, struct PCIAddr addr);
unsigned int pciReadLong(unsigned char configAddr, struct PCIAddr addr);

void pciWriteLong(unsigned char configAddr, struct PCIAddr addr, unsigned int value);

void pciReadConfig(struct PCIAddr addr, struct PCIConfig* config);

unsigned int pciGetBARSize(struct PCIAddr addr, struct PCIConfig* config);

void pciUpdateStatus(struct pci_device_t* device);

#endif	//PCI_H
