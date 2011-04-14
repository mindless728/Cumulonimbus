#include "ide.h"
#include "c_io.h"
#include "startup.h"
#include "../IOPorts/ioports.h"

unsigned int ide_num_devices;
unsigned int ide_num_channels;
unsigned int ide_num_controllers;

ide_controller_t ide_controllers[ATA_MAX_IDE_CONTROLLERS];
ide_channel_t ide_channels[ATA_MAX_IDE_CHANNELS];
ide_device_t ide_devices[ATA_MAX_IDE_DEVICES];

void ide_controller_init(pci_device_t * device);
void ide_channel_init();
void ide_device_init();

void copy_device_to_controller(ide_controller_t * controller, pci_device_t * device);
void set_base_io_register(pci_device_t * device, uint8_t channel, uint16_t base_port);
void set_base_control_register(pci_device_t * device, uint8_t channel, uint16_t base_port);
void set_bus_master_base(pci_device_t * device, uint16_t base_port);

status_t ide_init(pci_device_list_t * list) {
	//@TODO panic here!!!!!
	if(!list)
		return E_SUCCESS;

	pci_device_t * device = list->first;
	ide_controller_t * ide_controller;
	ide_channel_t * ide_channel = 0;
	ide_device_t * ide_device = 0;
	ide_num_controllers = ide_num_channels = ide_num_devices = 0;
	int i,j;
	unsigned char //device_added = 0,
				  //channel_added = 0,
				  register_read = 1,
				  ide_type = 0;
	
	while(device) {
		//@TODO break up into separate init functions
		if(device->config.classCode == 1 && device->config.subClass == 1) {
			//get controller pointer
			ide_controller = &(ide_controllers[ide_num_controllers]);
			
			//check the bus master base register for the device, allocate if needed
			if(device->config.headers.type0.bar[4] & 0x1) {
				set_bus_master_base(device, allocate_ports_align(16,16));
			}
			
			//copy over the data from the pci device config to the ide controller struct
			copy_device_to_controller(ide_controller, device);
			
			//debug output @TODO remove
			c_printf("- IDE Device, VEN: 0x%x, DEV: 0x%x\n", ide_controller->VID, ide_controller->DID);
			
			
			//set type to not assumed
			ide_type = 0;
			for(i = 0; i < 2; ++i) { //cycle through the channels on the device
				//get channel pointer
				ide_channel = &(ide_channels[ide_num_channels]);
				ide_channel->primary = i;
				
				//get some default values
				ide_channel->bus_master_base = device->config.headers.type0.bar[4]+i*8;
				if(!ide_type) { //do this if the device is not parallel or not assumed yet
					ide_channel->base_io_register = device->config.headers.type0.bar[2*i];
					ide_channel->base_control_register = device->config.headers.type0.bar[2*i+1];
					
					//check if type should be set to parallel
					if(ide_channel->base_io_register == 0 || ide_channel->base_io_register == 1) {
						ide_type = 1;
						ide_channel->base_io_register = 0;
					}

					//check to see if the device is not parallel and assign ports
					if(!ide_type) {
						ide_channel->base_io_register = allocate_ports_align(8,8);
						set_base_io_register(device, i, ide_channel->base_io_register);
						if(!i)
							ide_controller->PCMD_BAR = ide_channel->base_io_register;
						else
							ide_controller->SCMD_BAR = ide_channel->base_io_register;

						ide_channel->base_control_register = allocate_ports_align(4,4);
						set_base_control_register(device, i, ide_channel->base_control_register);
						if(!i)
							ide_controller->PCNL_BAR = ide_channel->base_control_register;
						else
							ide_controller->SCNL_BAR = ide_channel->base_control_register;
					}
				}
				if(ide_type) { //do this if the device is parallel
					ide_channel->base_io_register = 0x1f0-i*0x80;
					ide_channel->base_control_register = 0x3f4-i*0x80;
					ide_channel->bus_master_base = device->config.headers.type0.bar[4]+i*8;
				}
				
				//debug output @TODO remove
				c_printf(" - Channel: %d, CMD_BAR: 0x%x, CNL_BAR: 0x%x, BM_BASE: 0x%x\n",i,ide_channel->base_io_register,ide_channel->base_control_register,ide_channel->bus_master_base);
				
				for(j = 0; j < 2; ++j) { //cycle through the possible ide devices on the channel
					//get the device pointer
					ide_device = &(ide_devices[ide_num_devices]);
					
					//get some default values
					ide_device->ide_channel = ide_channel;
					ide_device->drive = j;
					
					//turn off interrupts on the channel
					ide_register_write(ide_device, ATA_REG_CONTROL, 2);
					
					//select the drive
					ide_register_write(ide_device, ATA_REG_HDDEVSEL, 0xa0 | (j << 4));
					__delay(1);
					
					//send the identify command over
					ide_register_write(ide_device, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
					__delay(1);
					
					//read status
					while(register_read) {
						ide_register_read(ide_device, ATA_REG_STATUS, &register_read);
						if(register_read & ATA_SR_ERR) {register_read = 0xff; break;}
						if(!(register_read & ATA_SR_BSY) && (register_read & ATA_SR_DRQ)) break;
					}
					
					//debug output @TODO remove
					c_printf("  - Drive: %d, Status: 0x%x\n", ide_device->drive, register_read);
				}
			}
		}
		device = device->next;
		
		//debug output @TODO remove
		c_printf("\n");
	}
	
	return E_SUCCESS;
}

status_t ide_register_read(ide_device_t * device, unsigned char reg, unsigned char * ret) {
	if(reg > 0x7 && reg < 0xc)
		ide_register_write(device, ATA_REG_CONTROL, 0x82);
	if (reg < 0x08)
		*ret = __inb(device->ide_channel->base_io_register + reg - 0x00);
	else if (reg < 0x0C)
		*ret = __inb(device->ide_channel->base_io_register  + reg - 0x06);
	else if (reg < 0x0E)
		*ret = __inb(device->ide_channel->base_control_register  + reg - 0x0A);
	else if (reg < 0x16)
		*ret = __inb(device->ide_channel->bus_master_base + reg - 0x0E);
	if(reg > 0x7 && reg < 0xc)
		ide_register_write(device, ATA_REG_CONTROL, 0x2);
	return E_SUCCESS;
}

status_t ide_register_write(ide_device_t * device, unsigned char reg, unsigned char out) {
	if(reg > 0x7 && reg < 0xc)
		ide_register_write(device, ATA_REG_CONTROL, 0x82);
	if (reg < 0x08)
		__outb(device->ide_channel->base_io_register + reg - 0x00, out);
	else if (reg < 0x0C)
		__outb(device->ide_channel->base_io_register  + reg - 0x06, out);
	else if (reg < 0x0E)
		__outb(device->ide_channel->base_control_register  + reg - 0x0A, out);
	else if (reg < 0x16)
		__outb(device->ide_channel->bus_master_base + reg - 0x0E, out);
	if(reg > 0x7 && reg < 0xc)
		ide_register_write(device, ATA_REG_CONTROL, 0x2);
	return E_SUCCESS;
}

void copy_device_to_controller(ide_controller_t * controller, pci_device_t * device) {
	//grab pointer to the device config
	pci_config_t * config = &(device->config);
	
	//copy over data already there
	controller->VID = config->vendorId;
	controller->DID = config->deviceId;
	controller->PCICMD = config->command;
	controller->PCISTS = config->status;
	controller->RID = config->revisionId;
	controller->PI = config->progIF;
	controller->SCC = config->subClass;
	controller->BCC = config->classCode;
	controller->CLS = config->cacheLineSize;
	controller->PMLT = config->latencyTimer;
	controller->PCMD_BAR = config->headers.type0.bar[0];
	controller->PCNL_BAR = config->headers.type0.bar[1];
	controller->SCMD_BAR = config->headers.type0.bar[2];
	controller->SCNL_BAR = config->headers.type0.bar[3];
	controller->BM_BASE = config->headers.type0.bar[4];
	controller->IDE_SVID = config->headers.type0.systemVendorId;
	controller->IDE_SID = config->headers.type0.systemId;
	controller->INTR_LN = config->headers.type0.irqLine;
	controller->INTR_PN = config->headers.type0.irqPin;
	
	//read data from the device that is still needed
}

void set_base_io_register(pci_device_t * device, uint8_t channel, uint16_t base_port) {
	pci_addr_t addr;
	addr.bus = device->address.bus;
	addr.slot = device->address.slot;
	addr.func = device->address.func;
	addr.offset = 4+2*channel;
	
	_pci_write_long(1, addr, base_port);
	
	device->config.headers.type0.bar[2*channel] = base_port;
}

void set_base_control_register(pci_device_t * device, uint8_t channel, uint16_t base_port) {
	pci_addr_t addr;
	addr.bus = device->address.bus;
	addr.slot = device->address.slot;
	addr.func = device->address.func;
	addr.offset = 5+2*channel;
	
	_pci_write_long(1, addr, base_port);
	
	device->config.headers.type0.bar[2*channel] = base_port;
}

void set_bus_master_base(pci_device_t * device, uint16_t base_port) {
	//status_t _pci_write_long(boolean_t configAddr, pci_addr_t addr, uint32_t value);
	pci_addr_t addr;
	addr.bus = device->address.bus;
	addr.slot = device->address.slot;
	addr.func = device->address.func;
	addr.offset = 8;
	
	_pci_write_long(1, addr, base_port);
	
	device->config.headers.type0.bar[4] = base_port;
}
