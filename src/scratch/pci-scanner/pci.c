#include "pci.h"
#include "startup.h"
#include "c_io.h"

void pciScan(){

	int slot = 0;
	int bus = 0;
	struct PCIAddr addr;
	struct PCIConfig config;

	addr.offset=0;
	addr.func=0;

	for(bus=0; bus<4; bus++){

		c_printf("Scanning PCI Bus %d\n", bus);

		addr.bus=bus;
		for(slot=0; slot<0x1f; slot++){
			addr.slot = slot;
			addr.func = 0;

			pciReadConfig(addr, &config);

			if(config.deviceId!=0xffff){
				c_printf("------------------------------\n");
				c_printf("Bus=%d Slot=%d Func=%d\n", addr.bus, addr.slot, addr.func);
				printPciConfig(&config, &addr);

				if(config.headerType & 0x80){
					c_printf("Has secondary functions\n");
					c_getchar();

					int func=1;
					for(func=1; func<3; func++){
						addr.func = func;
						pciReadConfig(addr, &config);
						if(config.deviceId != 0xffff){
							c_printf("Bus=%d Slot=%d Func=%d\n", addr.bus, addr.slot, addr.func);
							printPciConfig(&config, &addr);
							c_getchar();
						}
					}
				}
				else{
					c_getchar();
				}
			}

			/*unsigned long temp=0;
			temp = pciConfigReadWord(bus, slot, 0, 0x00);

			if(temp != 0xffffffff){
				c_printf("------------------------------\n");
				c_printf("Found device on bus=%d slot=%d\n", bus, slot);
				c_printf("    DeviceId:  0x%x\n", temp>>16);
				c_printf("    VendorId:  0x%x\n", temp&0xffff);

				temp = pciConfigReadWord(bus, slot, 0, 0x08);
				c_printf("    ClassCode: 0x%x\n", temp>>24);
				c_printf("    SubClass:  0x%x\n", (temp>>16)&0xff);
				c_printf("    Revision:  0x%x\n", temp&0xff);

				temp = pciConfigReadWord(bus, slot, 0, 0x0c);
				c_printf("    Header:    0x%x\n", (temp>>16)&0xff);

				c_getchar();
			}*/
		}
	}
}


int pciFindDevice(struct PCIConfig* config, struct PCIAddr* addr){
	int slot = 0;
	int bus = 0;
	struct PCIAddr tempAddr;

	tempAddr.offset=0;
	tempAddr.func=0;

	for(bus=0; bus<4; bus++){

		tempAddr.bus=bus;
		for(slot=0; slot<0x1f; slot++){
			tempAddr.slot = slot;
			tempAddr.func = 0;

			tempAddr.offset = 0x00;
			unsigned int vendor_dev_id = pciReadLong(1, tempAddr);
			if(vendor_dev_id == *((unsigned int*)config)){
				pciReadConfig(tempAddr, config);
				*addr = tempAddr;
				return 0;
			}
		}
	}
	return -1;
}

void printPciConfig(struct PCIConfig* config, struct PCIAddr* addr){
	c_printf("    DeviceId:  0x%x\n", config->deviceId);
	c_printf("    VendorId:  0x%x\n", config->vendorId);
	c_printf("    Status:    0x%x\n", config->status);
	c_printf("    Command:   0x%x\n", config->command);
	c_printf("    Class:     0x%x\n", config->classCode);
	c_printf("    SubClass:  0x%x\n", config->subClass);
	c_printf("    ProgIF:    0x%x\n", config->progIF);
	c_printf("    Revision:  0x%x\n", config->revisionId);
	c_printf("    BIST:      0x%x\n", config->bist);
	c_printf("    Header:    0x%x\n", config->headerType);
	c_printf("    Latency:   0x%x\n", config->latencyTimer);
	c_printf("    CacheSize: 0x%x\n", config->cacheLineSize);
	if(config->headerType == 0x00){
		c_printf("    BAR0:      0x%x\n", config->headers.type0.bar0);
		unsigned int bar0Size = pciGetBARSize(*addr, config);
		c_printf("    BAR0_Size: %d\n", bar0Size);
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

unsigned int pciReadLong(unsigned char configAddr, struct PCIAddr addr){
	unsigned int bus = ((unsigned int)addr.bus) << 16;
	unsigned int slot = ((unsigned int)addr.slot & 0x3f) << 11;
	unsigned int func = ((unsigned int)addr.func & 0x03) << 8;
	unsigned int offset = ((unsigned int)addr.offset & 0x3c);	//long aligned
	unsigned int address = bus | slot | func | (((unsigned int)configAddr)<<31) | offset;

	__outl(PCI_CONFIG_ADDRESS, address);

	return ((unsigned int)__inl(PCI_CONFIG_DATA));
}

void pciWriteLong(unsigned char configAddr, struct PCIAddr addr, unsigned int value){
	unsigned int bus = ((unsigned int)addr.bus) << 16;
	unsigned int slot = ((unsigned int)addr.slot & 0x3f) << 11;
	unsigned int func = ((unsigned int)addr.func & 0x03) << 8;
	unsigned int offset = ((unsigned int)addr.offset & 0x3c);	//int aligned
	unsigned int address = bus | slot | func | (((unsigned int)configAddr)<<31) | offset;

	__outl(PCI_CONFIG_ADDRESS, address);
	__outl(PCI_CONFIG_DATA, value);
}

void pciReadConfig(struct PCIAddr addr, struct PCIConfig* config){
	addr.offset=0;

	unsigned int* lptr = (unsigned int*)config;

	*lptr = pciReadLong(1, addr);
	lptr++;
	addr.offset = 0x04;

	*lptr = pciReadLong(1, addr);
	lptr++;
	addr.offset = 0x08;

	*lptr = pciReadLong(1, addr);
	lptr++;
	addr.offset = 0x0c;

	*lptr = pciReadLong(1, addr);
	lptr++;
	addr.offset = 0x10;

	if(config->headerType == 0x00){
		while(addr.offset < PCI_TYPE0_MAX_OFFSET){
			*lptr = pciReadLong(1, addr);
			lptr++;
			addr.offset += 0x04;
		}
	}
	else if(config->headerType == 0x01){
		while(addr.offset < PCI_TYPE1_MAX_OFFSET){
			*lptr = pciReadLong(1, addr);
			lptr++;
			addr.offset += 0x04;
		}
	}
	else if(config->headerType == 0x02){
		while(addr.offset < PCI_TYPE2_MAX_OFFSET){
			*lptr = pciReadLong(1, addr);
			lptr++;
			addr.offset += 0x04;
		}
	}
}

unsigned int pciGetBARSize(struct PCIAddr addr, struct PCIConfig* config){
	struct PCIAddr barAddr = addr;
	unsigned int size = 0;
	unsigned int original_bar = 0;

	if(config->headerType == 0x00){
		barAddr.offset = 0x10;
		original_bar = config->headers.type0.bar0;
	}
	else{
		//TODO: Support reading intial bar for all header types
	}

	size = 0xffffffff;
	pciWriteLong(1, barAddr, size);

	size = (~(pciReadLong(1, barAddr) & PCI_BAR_ADDR_MASK)) + 1;
	pciWriteLong(1, barAddr, original_bar);
	return size;
}

