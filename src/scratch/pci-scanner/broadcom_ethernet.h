#ifndef BROADCOM_ETHERNET_H
#define BROADCOM_ETHERNET_H

#include "pci.h"
#include "ethernet.h"

#define BCM5788_PCI_DEVICE	0x1691
#define BCM5788_PCI_VENDOR	0x14e4

pci_device_t* bcm_driver_init();


#endif	//BROADCOM_ETHERNET_H
