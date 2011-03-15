#ifndef ETHERNET_H
#define ETHERNET_H

#include "pci.h"

/*#define ETH_BASE_ADDRESS 0x100000
#define ETH_BAR_CONFIG 0x0*/

int init_ethernet(unsigned int vendor, unsigned int device);

#endif	//ETHERNET_H
