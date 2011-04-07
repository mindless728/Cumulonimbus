#include "ide.h"
#include "c_io.h"
#include "startup.h"

unsigned int num_devices;
unsigned int num_channels;

ide_channel_t ide_channels[ATA_MAX_IDE_CHANNELS];
ide_device_t ide_devices[ATA_MAX_IDE_DEVICES];

status_t ide_init(pci_device_list_t * list) {
	//@TODO panic here!!!!!
	if(!list)
		return E_SUCCESS;

	pci_device_t * device = list->first;
	num_channels = num_devices = 0;
	int i,j;
	unsigned char //device_added = 0,
				  //channel_added = 0,
				  register_read;
	
	while(device) {
		if(device->config.classCode == 1 && device->config.subClass == 1) {
			for(i = 0; i < 2; ++i) { //loop through the channels on the ide controller
				//grab base registers
				ide_channels[num_channels].base_io_register = device->config.headers.type0.bar[2*i];
				if(ide_channels[num_channels].base_io_register == 0 || ide_channels[num_channels].base_io_register == 1)
					ide_channels[num_channels].base_io_register = 0x1f0 - i*0x80;
				ide_channels[num_channels].base_control_register = device->config.headers.type0.bar[2*i+1];
				if(ide_channels[num_channels].base_control_register == 0 || ide_channels[num_channels].base_control_register == 1)
					ide_channels[num_channels].base_control_register = 0x3f4 - i*0x80;
				ide_channels[num_channels].bus_master_base = device->config.headers.type0.bar[4]+i*8;
				
				for(j = 0; j < 2; ++j) { //loop through the devices on the channels
					ide_devices[num_devices].ide_channel = &(ide_channels[num_channels]);
					ide_devices[num_devices].drive = j;
					
					//select device
					ide_register_write(&(ide_devices[num_devices]), ATA_REG_HDDEVSEL, 0xA0 | (j << 4));
					
					//send the identify command
					ide_register_write(&(ide_devices[num_devices]), ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
					__delay(10);
					
					//read the result back
					ide_register_read(&(ide_devices[num_devices]), ATA_REG_STATUS, &register_read);
					
					if(register_read) {
						c_printf("Found an actual device, i: %d, j: %d\n",i,j);
						c_printf("  - Ven:0x%x, Dev:0x%x\n",device->config.vendorId, device->config.deviceId);
						c_printf("  - Data:0x%x\n",register_read);
						c_printf("\n");
					}
				}
			}
		}
		device = device->next;
	}
	
	return E_SUCCESS;
}

status_t ide_register_read(ide_device_t * device, unsigned char reg, unsigned char * ret) {
	if (reg < 0x08)
		*ret = __inb(device->ide_channel->base_io_register + reg - 0x00);
	else if (reg < 0x0C)
		*ret = __inb(device->ide_channel->base_io_register  + reg - 0x06);
	else if (reg < 0x0E)
		*ret = __inb(device->ide_channel->base_control_register  + reg - 0x0A);
	else if (reg < 0x16)
		*ret = __inb(device->ide_channel->bus_master_base + reg - 0x0E);
	return E_SUCCESS;
}

status_t ide_register_write(ide_device_t * device, unsigned char reg, unsigned char out) {
	if (reg < 0x08)
		__outb(device->ide_channel->base_io_register + reg - 0x00, out);
	else if (reg < 0x0C)
		__outb(device->ide_channel->base_io_register  + reg - 0x06, out);
	else if (reg < 0x0E)
		__outb(device->ide_channel->base_control_register  + reg - 0x0A, out);
	else if (reg < 0x16)
		__outb(device->ide_channel->bus_master_base + reg - 0x0E, out);
	return E_SUCCESS;
}