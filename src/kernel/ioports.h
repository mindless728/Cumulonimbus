#ifndef IOPORTS_H
#define IOPORTS_H

#include <types.h>

void _ioports_init(void);
uint16_t allocate_ports(uint16_t amount);
uint16_t allocate_ports_align(uint16_t amount, uint16_t align);

#endif
