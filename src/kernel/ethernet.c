#include "ethernet.h"
#include "c_io.h"
//#include "defines.h"

//pci_device_t _eth0;

/*int init_ethernet(unsigned int vendor, unsigned int device){
	_eth0.config.vendorId = vendor;
	_eth0.config.deviceId = device;

	if(_pci_find_device(&_eth0.config, &_eth0.addr) == 0){
		//SUCCESS

		_eth0.memory_space = pciGetBARSize(_eth0.pciAddr, &_eth0.config);

		c_printf("Found device 0x%x at %d:%d:%d\n", device, _eth0.pciAddr.bus, _eth0.pciAddr.slot, _eth0.pciAddr.func);
		c_printf("BAR0 starts at 0x%x and is of size %d bytes\n", _eth0.config.headers.type0.bar0, _eth0.memory_space);
		c_printf("IntPin = 0x%x IntLine = 0x%x\n", _eth0.config.headers.type0.irqPin, _eth0.config.headers.type0.irqLine);

		return 0;
	}
	return -1;
}
*/

mac_address_t _local_mac;

uint32_t htonl(uint32_t hostlong){
	#ifdef LITTLE_ENDIAN
	uint16_t temp = ((uint16_t*)&hostlong)[1];
	hostlong = hostlong<<sizeof(uint16_t) | temp;
	#endif

	return hostlong;
}

uint16_t htons(uint16_t hostshort){
	#ifdef LITTLE_ENDIAN
	uint8_t temp = ((uint8_t*)&hostshort)[1];
	hostshort = hostshort<<sizeof(uint8_t) | temp;
	#endif

	return hostshort;
}

uint32_t ntohl(uint32_t netlong){
	#ifdef LITTLE_ENDIAN
	uint16_t temp = ((uint16_t*)&netlong)[1];
	netlong = netlong<<sizeof(uint16_t) | temp;
	#endif

	return netlong;
}


uint16_t ntohs(uint16_t netshort){
	#ifdef LITTLE_ENDIAN
	uint8_t temp = ((uint8_t*)&netshort)[1];
	netshort = netshort<<sizeof(uint8_t) | temp;
	#endif

	return netshort;
}
