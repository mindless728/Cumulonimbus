#include "types.h"
#include "kalloc.h"
#include "utils.h"
#include "pci.h"
#include "startup.h"
#include "c_io.h"



status_t _pci_alloc_device(pci_device_t** dev){
	*dev = (pci_device_t*) kalloc(sizeof(pci_device_t));
	memset(*dev, 0x00, sizeof(pci_device_t));

	c_printf("_pci_alloc_device 0x%x\n", *dev);

	return E_SUCCESS;
}


status_t _pci_alloc_device_list(pci_device_list_t** list){
	*list = (pci_device_list_t*) kalloc(sizeof(pci_device_list_t));

	c_printf("_pci_alloc_device_list 0x%x\n", *list);

	memset(*list, 0x00, sizeof(pci_device_list_t));

	return E_SUCCESS;
}

status_t _pci_scan(pci_device_list_t* list){
	status_t status = E_SUCCESS;
	int slot = 0;
	int bus = 0;
	pci_addr_t addr;
	pci_config_t config;

	addr.offset=0;
	addr.func=0;

	list->first=NULL;
	list->last=NULL;

	for(bus=0; bus<4; bus++){

		//c_printf("Scanning PCI Bus %d\n", bus);

		addr.bus=bus;
		for(slot=0; slot<0x1f; slot++){
			addr.slot = slot;
			addr.func = 0;

			//pciReadConfig(addr, &config);
			status = _pci_read_config(addr, &config);

			if(status != E_SUCCESS){
				break;
			}
			else if(config.deviceId!=0xffff){
				//Found valid device
				//c_printf("Device found in slot %d\n", addr.slot);

				pci_device_t* device=NULL;
				_pci_alloc_device(&device);

				device->address = addr;
				device->config = config;
				if(device->config.headerType == 0x00){
					_pci_read_bar_size(device, &device->memory_space);
				}

				status = _pci_append_device(list, device);
				if(status != E_SUCCESS){
					return status;
				}

				if(config.headerType & 0x80){
					//Device may support secondary functions
					int func=1;

					//Scan for secondary functions
					for(func=1; func<3; func++){
						addr.func = func;
						status = _pci_read_config(addr, &config);

						if(status != E_SUCCESS){
							return status;
						}
						else if(config.deviceId != 0xffff){
							//Found valid device
							_pci_alloc_device(&device);

							device->address = addr;
							device->config = config;
							if(device->config.headerType == 0x00){
								_pci_read_bar_size(device, &device->memory_space);
							}

							status = _pci_append_device(list, device);
							if(status != E_SUCCESS){
								return status;
							}
						}
					}
				}
			}
		}
	}

	return status;
}


/*int pciFindDevice(pci_device_t* device){
	int slot = 0;
	int bus = 0;
	pci_device_t tempDev;

	tempDev.address.offset=0;
	tempDev.address.func=0;

	for(bus=0; bus<4; bus++){

		tempDev.address.bus=bus;
		for(slot=0; slot<0x1f; slot++){
			tempDev.address.slot = slot;
			tempDev.address.func = 0;

			tempDev.address.offset = 0x00;
			unsigned int vendor_dev_id = pciReadLong(1, tempDev.address);
			if(vendor_dev_id == *((unsigned int*)device->config)){
				pciReadConfig(tempAddr, config);
				*addr = tempAddr;
				return 0;
			}
		}
	}
	return -1;
}*/

void _pci_print_config(pci_device_t* device){
	c_printf("Bus=%d Slot=%d Func=%d\n", device->address.bus, device->address.slot, device->address.func);
	c_printf("    DeviceId:  0x%x\n", device->config.deviceId);
	c_printf("    VendorId:  0x%x\n", device->config.vendorId);
	c_printf("    Status:    0x%x\n", device->config.status);
	c_printf("    Command:   0x%x\n", device->config.command);
	c_printf("    Class:     0x%x\n", device->config.classCode);
	c_printf("    SubClass:  0x%x\n", device->config.subClass);
	c_printf("    ProgIF:    0x%x\n", device->config.progIF);
	c_printf("    Revision:  0x%x\n", device->config.revisionId);
	c_printf("    BIST:      0x%x\n", device->config.bist);
	c_printf("    Header:    0x%x\n", device->config.headerType);
	c_printf("    Latency:   0x%x\n", device->config.latencyTimer);
	c_printf("    CacheSize: 0x%x\n", device->config.cacheLineSize);
	if(device->config.headerType == 0x00){
		c_printf("    BAR0:      0x%x\n", device->config.headers.type0.bar0);
		c_printf("    BAR0_Size: %d\n", device->memory_space);
	}
}

/*unsigned char pciReadByte(unsigned char configAddr, struct PCIAddr addr){
	unsigned long bus = ((unsigned long)addr.bus) << 16;
	unsigned long slot = ((unsigned long)addr.slot & 0x3f) << 11;
	unsigned long func = ((unsigned long)addr.func & 0x03) << 8;
	unsigned long offset = ((unsigned long)addr.offset & 0x3c);
	unsigned long address = bus | slot | func | (((unsigned long)configAddr)<<31) | offset;

	__outl(CONFIG_ADDRESS, address);

	return (unsigned char)(__inw(CONFIG_DATA) >> ((3-(addr.offset & 0x03))*8) );
}

unsigned short pciReadShort(unsigned char configAddr, struct PCIAddr addr){
	unsigned long bus = ((unsigned long)addr.bus) << 16;
	unsigned long slot = ((unsigned long)addr.slot & 0x3f) << 11;
	unsigned long func = ((unsigned long)addr.func & 0x03) << 8;
	unsigned long offset = ((unsigned long)addr.offset & 0x3e);	//word aligned
	unsigned long address = bus | slot | func | (((unsigned long)configAddr)<<31) | offset;

	__outl(CONFIG_ADDRESS, address);

	return (__inw(CONFIG_DATA) >> (addr.offset & 2)*8);
}*/


status_t _pci_read_long(unsigned char configAddr, pci_addr_t addr, uint32_t* value){

	if(value == NULL){
		return E_BAD_PARAM;
	}

	unsigned int bus = ((unsigned int)addr.bus) << 16;
	unsigned int slot = ((unsigned int)addr.slot & 0x3f) << 11;
	unsigned int func = ((unsigned int)addr.func & 0x03) << 8;
	unsigned int offset = ((unsigned int)addr.offset & 0x3c);	//long aligned
	unsigned int address = bus | slot | func | (((unsigned int)configAddr)<<31) | offset;

	__outl(PCI_CONFIG_ADDRESS, address);

	*value = ((unsigned int)__inl(PCI_CONFIG_DATA));

	return E_SUCCESS;
}

status_t _pci_write_long(unsigned char configAddr, pci_addr_t addr, uint32_t value){
	unsigned int bus = ((unsigned int)addr.bus) << 16;
	unsigned int slot = ((unsigned int)addr.slot & 0x3f) << 11;
	unsigned int func = ((unsigned int)addr.func & 0x03) << 8;
	unsigned int offset = ((unsigned int)addr.offset & 0x3c);	//int aligned
	unsigned int address = bus | slot | func | (((unsigned int)configAddr)<<31) | offset;

	__outl(PCI_CONFIG_ADDRESS, address);
	__outl(PCI_CONFIG_DATA, value);
	return E_SUCCESS;
}


status_t _pci_read_config(pci_addr_t addr, pci_config_t* config){
	addr.offset=0;

	status_t status = E_SUCCESS;
	uint32_t* lptr = (uint32_t*)config;

	while(addr.offset < 0x10){
		status = _pci_read_long(1, addr, lptr);
		if(status != E_SUCCESS){
			return status;
		}

		lptr++;
		addr.offset += 0x04;
	}

	if(config->headerType == 0x00){
		while(addr.offset < PCI_TYPE0_MAX_OFFSET){
			status = _pci_read_long(1, addr, lptr);
			if(status != E_SUCCESS){
				return status;
			}

			lptr++;
			addr.offset += 0x04;
		}
	}
	else if(config->headerType == 0x01){
		while(addr.offset < PCI_TYPE1_MAX_OFFSET){
			status = _pci_read_long(1, addr, lptr);
			if(status != E_SUCCESS){
				return status;
			}

			lptr++;
			addr.offset += 0x04;
		}
	}
	else if(config->headerType == 0x02){
		while(addr.offset < PCI_TYPE2_MAX_OFFSET){
			status = _pci_read_long(1, addr, lptr);
			if(status != E_SUCCESS){
				return status;
			}

			lptr++;
			addr.offset += 0x04;
		}
	}

	return status;
}

//unsigned int pciGetBARSize(struct PCIAddr addr, struct PCIConfig* config){
status_t _pci_read_bar_size(pci_device_t* device, uint32_t* size){
	pci_addr_t barAddr = device->address;
	uint32_t original_bar = 0;

	if(size == NULL || device == NULL){
		return E_BAD_PARAM;
	}

	if(device->config.headerType == 0x00){
		barAddr.offset = 0x10;
		original_bar = device->config.headers.type0.bar0;
	}
	else{
		//TODO: Support reading intial bar for all header types
		return E_NOT_IMPLEMENTED;
	}

	_pci_write_long(1, barAddr, 0xffffffff);

	_pci_read_long(1, barAddr, size);
	*size = (~((*size) & PCI_BAR_ADDR_MASK)) + 1;
	return _pci_write_long(1, barAddr, original_bar);
}


status_t _pci_append_device(pci_device_list_t* list, pci_device_t* device){

	if(list == NULL || device == NULL){
		c_printf("Passed list=0x%x device=0x%x\n", list, device);
		return E_BAD_PARAM;
	}

	if(list->last == NULL){
		list->last = device;
		list->first = device;

		list->last->next = NULL;
		list->first->prev = NULL;
	}
	else{
		list->last->next = device;
		device->prev = list->last;
		list->last = device;
	}

	list->size++;
	return E_SUCCESS;
}

