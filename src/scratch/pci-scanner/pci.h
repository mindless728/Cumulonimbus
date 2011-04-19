/**
  *	@file	pci.h
  *	@author	Alan Meekins
  *
  *	Description:	Declarations for functions implemented in pci.c
  *
  *
  */

#ifndef PCI_H
#define PCI_H

#include "types.h"
#include "pci_defines.h"



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


inline uint32_t _pci_base_addr(uint32_t bar_value);

inline status_t _pci_mask_inta(pci_device_t* device);

#endif	//PCI_H
