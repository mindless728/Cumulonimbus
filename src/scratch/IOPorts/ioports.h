#ifndef IOPORTS_H
#define IOPORTS_H

#include "../pci-scanner/types.h"

void ioports_init(void);
uint16_t allocate_ports(uint16_t amount);
uint16_t allocate_ports_align(uint16_t amount, uint16_t align);

#endif
