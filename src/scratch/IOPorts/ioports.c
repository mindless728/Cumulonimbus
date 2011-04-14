#include "ioports.h"

uint16_t current_port;
const uint16_t last_port = 0xF000;

void ioports_init() {
	current_port = 0x1000;
}

uint16_t allocate_ports(uint16_t amount) {
	return allocate_ports_align(amount,1);
}

uint16_t allocate_ports_align(uint16_t amount, uint16_t alignment) {
	uint16_t new_current_port = current_port + amount;
	uint16_t align_offset = alignment - new_current_port % alignment;
	uint16_t ret = 0;

	if(align_offset == alignment)
		align_offset = 0;
	new_current_port += align_offset;

	if(new_current_port <= last_port) {
		ret = current_port+align_offset;
		current_port = new_current_port;
	}

	return ret;
}
