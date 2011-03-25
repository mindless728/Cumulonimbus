#ifndef PCI_H
#define PCI_H

#include "types.h"

#define PCI_MAX_BUS_COUNT	0x100
#define PCI_MAX_SLOT_COUNT	0x20
#define PCI_MAX_FUNC_COUNT	0x08

#define PCI_REG_ADDR(x) (x >> 2)
#define PCI_REG_OFFSET(x) (x & 0x02)

#define PCI_CONFIG_ADDRESS	0xCF8
#define PCI_CONFIG_DATA		0xCFC

#define PCI_TYPE0_MAX_OFFSET 0x3c
#define PCI_TYPE1_MAX_OFFSET 0x3c
#define PCI_TYPE2_MAX_OFFSET 0x44

#define PCI_HEADER_1_BAR_START 4

#define PCI_HEADER_1_BAR_COUNT 6

#define PCI_BAR_INFO_MASK 0x0000000f
#define PCI_BAR_ADDR_MASK 0xfffffff0



//Register definitions

//The following are all 16 bit registers
#define PCI_VENDOR_ID_REG			PCI_REG_ADDR(0x00)
#define PCI_DEVICE_ID_REG			PCI_REG_ADDR(0x02)

#define PCI_VENDOR_ID_REG_OFFSET	PCI_REG_OFFSET(0x00)
#define PCI_DEVICE_ID_REG_OFFSET	PCI_REG_OFFSET(0x02)


#define PCI_COMMAND_REG			PCI_REG_ADDR(0x04)
#define PCI_STATUS_REG			PCI_REG_ADDR(0x06)

#define PCI_COMMAND_REG_OFFSET	PCI_REG_OFFSET(0x04)
#define PCI_STATUS_REG_OFFSET	PCI_REG_OFFSET(0x06)


//The following are all 8 bit registers
#define PCI_CLASS_CODE_REG		PCI_REG_ADDR(0x0b)
#define PCI_SUBCLASS_REG		PCI_REG_ADDR(0x0a)
#define PCI_PROG_IF_REG			PCI_REG_ADDR(0x09)
#define PCI_REV_ID_REG			PCI_REG_ADDR(0x08)

#define PCI_CLASS_CODE_REG_OFFSET	PCI_REG_OFFSET(0x0b)
#define PCI_SUBCLASS_REG_OFFSET		PCI_REG_OFFSET(0x0a)
#define PCI_PROG_IF_REG_OFFSET		PCI_REG_OFFSET(0x09)
#define PCI_REV_ID_REG_OFFSET		PCI_REG_OFFSET(0x08)


#define PCI_BIST_REG				PCI_REG_ADDR(0x0f)
#define PCI_HEADER_TYPE_REG			PCI_REG_ADDR(0x0e)
#define PCI_LATENCY_TIMER_REG		PCI_REG_ADDR(0x0d)
#define PCI_CACHE_LINE_SIZE_REG		PCI_REG_ADDR(0x0c)

#define PCI_BIST_REG_OFFSET				PCI_REG_OFFSET(0x0f)
#define PCI_HEADER_TYPE_REG_OFFSET		PCI_REG_OFFSET(0x0e)
#define PCI_LATENCY_TIMER_REG_OFFSET	PCI_REG_OFFSET(0x0d)
#define PCI_CACHE_LINE_SIZE_REG_OFFSET	PCI_REG_OFFSET(0x0c)


#define PCI_INT_PIN_REG		PCI_REG_ADDR(0x3d)
#define PCI_INT_LINE_REG	PCI_REG_ADDR(0x3c)

#define PCI_INT_PIN_REG_OFFSET	PCI_REG_OFFSET(0x3d)
#define PCI_INT_LINE_REG_OFFSET	PCI_REG_OFFSET(0x3c)

/*

 *	Register bit masks and useful values
 */

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
#define PCI_STATUS_PARITY_ERROR					(1<<15)
#define PCI_STATUS_SIGNALED_SYS_ERROR			(1<<14)
#define PCI_STATUS_RX_MASTER_ABORT				(1<<13)
#define PCI_STATUS_RX_TARGET_ABORT				(1<<12)
#define PCI_STATUS_SIGNALED_TARGET_ABORT		(1<<11)
#define PCI_STATUS_DEVSEL_TIMING				((1<<10) | (1<<9))
#define PCI_STATUS_MASTER_DATA_PARITY_ERROR		(1<<8)
#define PCI_STATUS_FAST_BACK_TO_BACK_CAPABLE	(1<<7)
//Bit six is reserved
#define PCI_STATUS_66MHZ_CAPABLE				(1<<5)
#define PCI_STATUS_CAPABILITIES_LIST			(1<<4)
#define PCI_STATUS_INTERRUPT_STATUS				(1<<3)
//Bits 0 through 2 are reserved


//Header type bit Masks
#define PCI_HDR_TYPE_MULTI_FUNC_MASK		0x80
#define PCI_HDR_TYPE_MASK					0x7f
//Header Type Values
#define PCI_HDR_TYPE_STD			0x00
#define PCI_HDR_TYPE_PCI_BRIDGE		0x01
#define PCI_HDR_TYPE_CARD_BRIDGE	0x02

//BIST bit masks
#define PCI_BIST_CAPABLE_MASK		(1<<7)
#define PCI_BIST_START_MASK			(1<<6)
//Bits 4 and 5 are reserved
#define PCI_BIST_COMPLETE_CODE_MASK	(0x0f)




typedef struct pci_addr{
	uint8_t bus;		//8 bits
	uint8_t slot;		//5 bits
	uint8_t func;		//2 bits
	//! Offset in 4byte chunks
	uint8_t offset;		//6 bits
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
			uint32_t bar[6];
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



/**
  *	Searches the provided pci device list for a device with a matching vendor
  *	and device ID number.
  *
  *	@param	list		List of detected pci devices
  *	@param	device		Pointer to a device structure pointer to store the found device
  *	@param	vendor_id	Vendor ID of the desired device
  *	@param	device_id	Device ID of the desired device
  *
  *	@return	Returns E_SUCCESS if the device was found and the device pointer
  *			could be correctly updated. If the provided device pointer is not
  *			valid then E_BAD_PARAM is returned. If the device was not found then
  *			E_NOT_FOUND will be return.
  */
status_t _pci_get_device(pci_device_list_t* list, pci_device_t** device, uint16_t vendor_id, uint16_t device_id);


/**
  *	Searches the provided pci device list for a device at the specified
  *	PCI bus address.
  *
  *	@param	list		List of detected pci devices
  *	@param	device		Pointer to a device structure pointer to store the found device
  *	@param	addr		Physical PCI address of desired.
  *
  *	@return	Returns E_SUCCESS if the device was found and the device pointer
  *			could be correctly updated. If the provided device pointer is not
  *			valid then E_BAD_PARAM is returned. If the device was not found then
  *			E_NOT_FOUND will be return.
  */
status_t _pci_get_device_by_address(pci_device_list_t* list, pci_device_t** device, pci_addr_t addr);

/**
  *	Reads a 32 bit register located at the specified PCI address.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci device to read from
  *	@param	value		Buffer to read the register value into.
  *
  *	@return	Returns E_SUCCESS if the value was read. Returns E_BAD_PARAM if
  *			value points to NULL.
  */
status_t _pci_read_long(boolean_t configAddr, pci_addr_t addr, uint32_t* value);


/**
  *	Reads an 8 bit register at the given PCI bus address with the specified byte
  *	offset.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci device to read from
  *	@param	offset		Number of bytes to offset into the register specified by
  *						addr, must be less than or equal to 2.
  *	@param	value		Buffer to read the register value into.
  *
  *	@return	Returns E_BAD_PARAM if the byte_offset is not in the specified range
  *			or if the value pointer is not valid.
  */
status_t _pci_read_short(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint16_t* value);


/**
  *	Reads an 8 bit register at the given PCI bus address with the specified byte
  *	offset.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci device to read from
  *	@param	offset		Number of bytes to offset into the register specified by
  *						addr, must be less than or equal to 3.
  *	@param	value		Buffer to read the register value into.
  *
  *	@return	Returns E_BAD_PARAM if the byte_offset is not in the specified range
  *			or if the value pointer is not valid.
  */
status_t _pci_read_byte(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint8_t* value);


/**
  *	Writea a 32 bit register located at the specified PCI address.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci device to write to
  *	@param	value		Value to write into register
  *
  *	@return	Returns E_SUCCESS
  */
status_t _pci_write_long(boolean_t configAddr, pci_addr_t addr, uint32_t value);


/**
  *	Writes a 16 bit register at the given PCI bus address with the specified
  *	byte offset.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci device to read from
  *	@param	offset		Number of bytes to offset into the register specified by
  *						addr, must be less than or equal to 3.
  *	@param	value		Value to write into register
  *
  *	@return	Returns E_BAD_PARAM if the byte_offset is not in the specified range.
  *			NOTE: This function causes a _pci_read_long and a _pci_write_long
  *			 as PCI does not directly support writing shorts. If either of these
  *			functions encounter an error the return value will be set to match.
  */
status_t _pci_write_short(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint16_t value);


/**
  *	Writes an 8 bit register at the given PCI bus address with the specified
  *	byte offset.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci device to read from
  *	@param	offset		Number of bytes to offset into the register specified by
  *						addr, must be less than or equal to 3.
  *	@param	value		Value to write into register
  *
  *	@return	Returns E_BAD_PARAM if the byte_offset is not in the specified range.
  *			NOTE: This function causes a _pci_read_long and a _pci_write_long
  *			 as PCI does not directly support writing bytes. If either of these
  *			functions encounter an error the return value will be set to match.
  */
status_t _pci_write_byte(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint8_t value);


/**
  *	Sets the selected mask bits of the addressed register to the desired state.
  *
  *	@param	configAddr	Wether the read should be completed using the
  *						configuration address space.
  *	@param	addr		Bus address of the pci register to modify
  *	@param	mask		Mask of bits to modify
  *	@param	state		State to set the masked bits to
  *
  *	@return	Returns the status of the underlying read/write calls.
  */
status_t _pci_set_bits(boolean_t configAddr, pci_addr_t addr, uint32_t mask, boolean_t state);

status_t _pci_read_config(pci_addr_t addr, pci_config_t* config);

status_t _pci_read_bar_size(pci_device_t* device, uint8_t bar_index, uint32_t* size);

#endif	//PCI_H
