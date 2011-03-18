#ifndef PCI_UTILS_H_
#define PCI_UTILS_H_

#include "int_types.h"

// Port I/O locations
#define PCI_CONFIG_ADDR_PORT 0xCF8
#define PCI_CONFIG_DATA_PORT 0xCFC

// struct for the first 8 bytes (first two registers) readable
// from the data port
typedef struct _pci_tiny_header_t {
    uint16_t device_id; // the id of the device
    uint16_t vendor_id; // the id of the vendor
    uint16_t status;    // status of the device 
    uint16_t command;   // command code of the device
    uint8_t class_code;
    uint8_t subclass;
} pci_tiny_header_t;

// functions
extern uint32_t _get_pci_bus_information( uint32_t, uint32_t, uint32_t, uint32_t );
extern void _scan_all_pci_devices(void);

#endif
