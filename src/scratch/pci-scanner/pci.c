#include "types.h"
#include "kalloc.h"
#include "utils.h"
#include "pci.h"
#include "startup.h"
#include "support.h"
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

	for(bus=0; bus < PCI_MAX_BUS_COUNT; bus++){

		//c_printf("Scanning PCI Bus %d\n", bus);

		addr.bus=bus;
		for(slot=0; slot < PCI_MAX_SLOT_COUNT; slot++){
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
					_pci_read_bar_size(device, 0, &device->memory_space);
				}

				status = _pci_append_device(list, device);
				if(status != E_SUCCESS){
					return status;
				}

				if(config.headerType & 0x80){
					//Device may support secondary functions
					int func=1;

					//Scan for secondary functions
					for(func=1; func < PCI_MAX_FUNC_COUNT; func++){
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
								_pci_read_bar_size(device, 0, &device->memory_space);
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


status_t _pci_get_device(pci_device_list_t* list, pci_device_t** device, uint16_t vendor_id, uint16_t device_id){
	if(device == NULL){
		return E_BAD_PARAM;
	}

	pci_device_t* tempPtr = list->first;

	while(tempPtr != NULL){
		if(tempPtr->config.vendorId == vendor_id && tempPtr->config.deviceId == device_id){
			*device = tempPtr;
			return E_SUCCESS;
		}
		tempPtr = tempPtr->next;
	}

	*device = NULL;
	return E_NOT_FOUND;
}


status_t _pci_get_device_by_address(pci_device_list_t* list, pci_device_t** device, pci_addr_t addr){
	if(device == NULL
	   || addr.bus >= PCI_MAX_BUS_COUNT
	   || addr.slot >= PCI_MAX_SLOT_COUNT
	   || addr.func >= PCI_MAX_FUNC_COUNT){

		return E_BAD_PARAM;
	}

	pci_device_t* tempPtr = list->first;

	while(tempPtr != NULL){
		if(tempPtr->address.bus == addr.bus
		   && tempPtr->address.slot == addr.slot
		   && tempPtr->address.func == addr.func){

			*device = tempPtr;
			return E_SUCCESS;
		}
		tempPtr = tempPtr->next;
	}

	*device = NULL;
	return E_NOT_FOUND;
}


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
		//c_printf("----BARS----\n");

		uint32_t* bars = device->config.headers.type0.bar;

		int i=0;
		for(; i < PCI_HEADER_1_BAR_COUNT; i++){
			uint32_t bar = bars[i];

			if(bar == 0x0){
				continue;
			}

			uint32_t addr = _pci_base_addr(bar);

			if(PCI_IS_IOBARR(bar) == 1){
				c_printf("    BAR%d: 0x%x iospace\n", i, addr);
			}
			else if(PCI_GET_BAR_TYPE(bar) == PCI_32b_WIDE_BAR){
				c_printf("    BAR%d:      0x%x 32bit\n", i, addr);
				_pci_read_bar_size(device, i, &addr);
				c_printf("    BAR%d_Size: %d bytes\n", i, addr);
			}
			else if(PCI_GET_BAR_TYPE(bar) == PCI_64b_WIDE_BAR){
				c_printf("    BAR%d:      0x%x%x 64bit\n", i, bars[i+1], addr);
				_pci_read_bar_size(device, i, &addr);
				c_printf("    BAR%d_Size: %d bytes\n", i, addr);
				i++;
			}
		}

		//c_printf("    BAR0:      0x%x\n", device->config.headers.type0.bar[0]);
		//c_printf("    BAR0_Size: %d\n", device->memory_space);
	}
}




status_t _pci_read_long(unsigned char configAddr, pci_addr_t addr, uint32_t* value){

	if(value == NULL){
		return E_BAD_PARAM;
	}

	unsigned int bus = ((unsigned int)addr.bus & 0xff) << 16;
	unsigned int slot = ((unsigned int)addr.slot & 0x1f) << 11;
	unsigned int func = ((unsigned int)addr.func & 0x07) << 8;
	unsigned int offset = ((unsigned int)(addr.offset & 0x3f)) << 2;	//int aligned
	unsigned int address = bus | slot | func | (((unsigned int)configAddr)<<31) | offset;

	__outl(PCI_CONFIG_ADDRESS, address);

	*value = ((unsigned int)__inl(PCI_CONFIG_DATA));

	return E_SUCCESS;
}


status_t _pci_read_short(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint16_t* value){
	uint32_t buffer = 0;
	status_t read_status = E_SUCCESS;

	if(byte_offset > 2){
		return E_BAD_PARAM;
	}

	read_status = _pci_read_long(configAddr, addr, &buffer);

	if(read_status == E_SUCCESS){
		*value = (uint16_t)((buffer>>(byte_offset*8)) & 0xffff);
		return read_status;
	}
	return read_status;
}

status_t _pci_read_byte(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint8_t* value){
	uint32_t buffer = 0;
	status_t read_status = E_SUCCESS;

	if(byte_offset > 3){
		return E_BAD_PARAM;
	}

	read_status = _pci_read_long(configAddr, addr, &buffer);

	if(read_status == E_SUCCESS){
		*value = (uint8_t)((buffer>>(byte_offset*8)) & 0xff);
		return read_status;
	}
	return read_status;
}




status_t _pci_write_long(unsigned char configAddr, pci_addr_t addr, uint32_t value){
	unsigned int bus = ((unsigned int)addr.bus & 0xff) << 16;
	unsigned int slot = ((unsigned int)addr.slot & 0x1f) << 11;
	unsigned int func = ((unsigned int)addr.func & 0x07) << 8;
	unsigned int offset = ((unsigned int)(addr.offset & 0x3f)) << 2;	//int aligned
	unsigned int address = bus | slot | func | (((unsigned int)configAddr)<<31) | offset;

	__outl(PCI_CONFIG_ADDRESS, address);
	__outl(PCI_CONFIG_DATA, value);
	return E_SUCCESS;
}


status_t _pci_write_short(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint16_t value){
	uint32_t buffer = 0;
	status_t read_status;

	if(byte_offset > 2){
		return E_BAD_PARAM;
	}

	read_status = _pci_read_long(configAddr, addr, &buffer);
	if(read_status != E_SUCCESS){
		return read_status;
	}

	//Shift the value to the correct offset
	buffer = buffer | ((value & 0xffff) << (byte_offset*8));

	return _pci_write_long(configAddr, addr, buffer);
}

status_t _pci_write_byte(boolean_t configAddr, pci_addr_t addr, uint8_t byte_offset, uint8_t value){
	uint32_t buffer = 0;
	status_t read_status;

	if(byte_offset > 3){
		return E_BAD_PARAM;
	}

	read_status = _pci_read_long(configAddr, addr, &buffer);
	if(read_status != E_SUCCESS){
		return read_status;
	}

	//Shift the value to the correct offset
	buffer = buffer | ((value & 0xff) << (byte_offset*8));

	return _pci_write_long(configAddr, addr, buffer);
}


status_t _pci_read_config(pci_addr_t addr, pci_config_t* config){
	addr.offset=0;

	status_t status = E_SUCCESS;
	uint32_t* lptr = (uint32_t*)config;

	while(addr.offset*4 < 0x10){
		status = _pci_read_long(1, addr, lptr);
		if(status != E_SUCCESS){
			return status;
		}

		lptr++;
		addr.offset += 0x01;
	}

	if(config->headerType == 0x00){
		while(addr.offset*4 < PCI_TYPE0_MAX_OFFSET){
			status = _pci_read_long(1, addr, lptr);
			if(status != E_SUCCESS){
				return status;
			}

			lptr++;
			addr.offset += 0x01;
		}
	}
	else if(config->headerType == 0x01){
		while(addr.offset*4 < PCI_TYPE1_MAX_OFFSET){
			status = _pci_read_long(1, addr, lptr);
			if(status != E_SUCCESS){
				return status;
			}

			lptr++;
			addr.offset += 0x01;
		}
	}
	else if(config->headerType == 0x02){
		while(addr.offset*4 < PCI_TYPE2_MAX_OFFSET){
			status = _pci_read_long(1, addr, lptr);
			if(status != E_SUCCESS){
				return status;
			}

			lptr++;
			addr.offset += 0x01;
		}
	}

	return status;
}


status_t _pci_set_bits(boolean_t configAddr, pci_addr_t addr, uint32_t mask, boolean_t state){
	uint32_t value;
	status_t status = _pci_read_long(configAddr, addr, &value);

	if(status != E_SUCCESS){
		return status;
	}

	if(state == 0){
		value &= ~mask;
	}
	else{
		value |= mask;
	}

	return _pci_write_long(configAddr, addr, value);
}

uint32_t _pci_base_addr(uint32_t bar_value){
	if(PCI_IS_IOBARR(bar_value) == 1){
		//c_printf("INFO: _pci_base_addr - IO Space BAR\n");
		return bar_value & PCI_BAR_IOSPACE_MASK;
	}
	else if(PCI_GET_BAR_TYPE(bar_value) == PCI_32b_WIDE_BAR){
		//c_printf("INFO: _pci_base_addr - 32bit BAR\n");
		return bar_value & PCI_BAR_ADDR_MASK;
	}
	else if(PCI_GET_BAR_TYPE(bar_value) == PCI_64b_WIDE_BAR){
		//c_printf("WARNING: _pci_base_addr - 64bit bars NOT IMPLEMENTED!\n");
		return bar_value & PCI_BAR_ADDR_MASK;
	}

	c_printf("type=0x%x\n", PCI_GET_BAR_TYPE(bar_value));
	__panic("NOT_SUPPORTED: _pci_base_addr - Unsupported PCI BAR");

	return 0x0;
}

status_t _pci_read_bar_size(pci_device_t* device, uint8_t bar_index, uint32_t* size){
	pci_addr_t barAddr = device->address;
	uint32_t original_bar = 0;

	if(size == NULL || device == NULL){
		return E_BAD_PARAM;
	}

	if(device->config.headerType == 0x00){
		if(bar_index > PCI_HEADER_1_BAR_COUNT-1){
			//BAR index out of allowed range
			return E_BAD_PARAM;
		}
		barAddr.offset = PCI_HEADER_1_BAR_START + bar_index;
		original_bar = device->config.headers.type0.bar[0];
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

