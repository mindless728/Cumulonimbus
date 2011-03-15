#include "ethernet.h"
#include "pci.h"
#include "c_io.h"

struct ethernet_t _eth0;

int init_ethernet(unsigned int vendor, unsigned int device){
	_eth0.config.vendorId = vendor;
	_eth0.config.deviceId = device;

	if(pciFindDevice(&_eth0.config, &_eth0.pciAddr) == 0){
		//SUCCESS

		_eth0.memory_space = pciGetBARSize(_eth0.pciAddr, &_eth0.config);

		c_printf("Found device 0x%x at %d:%d:%d\n", device, _eth0.pciAddr.bus, _eth0.pciAddr.slot, _eth0.pciAddr.func);
		c_printf("BAR0 starts at 0x%x and is of size %d bytes\n", _eth0.config.headers.type0.bar0, _eth0.memory_space);
		c_printf("IntPin = 0x%x IntLine = 0x%x\n", _eth0.config.headers.type0.irqPin, _eth0.config.headers.type0.irqLine);

		return 0;
	}
	return -1;
}
