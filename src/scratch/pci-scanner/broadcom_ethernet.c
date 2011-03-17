#include "pci.h"
#include "ethernet.h"
#include "broadcom_ethernet.h"
#include "kalloc.h"

pci_device_t* bcm_driver_init(){
	pci_device_t* device = kalloc(sizeof(pci_device_t));

	if(_pci_find_device())

	return device;
}
