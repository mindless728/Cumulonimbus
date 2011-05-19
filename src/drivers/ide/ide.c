#include "ide.h"
#include <kernel/includes.h>
#include <kernel/irqs.h>
#include <kernel/system.h>
#include <kernel/syscall.h>

//#define IDE_DEBUG_MSG

unsigned int ide_num_devices;
unsigned int ide_num_channels;
unsigned int ide_num_controllers;

ide_controller_t ide_controllers[ATA_MAX_IDE_CONTROLLERS];
ide_channel_t ide_channels[ATA_MAX_IDE_CHANNELS];
ide_device_t ide_devices[ATA_MAX_IDE_DEVICES];

void ide_controller_init(pci_device_t * device);
void ide_channel_init(void);
void ide_device_init(void);

void copy_device_to_controller(ide_controller_t * controller, pci_device_t * device);
void set_base_io_register(pci_device_t * device, uint8_t channel, uint16_t base_port);
void set_base_control_register(pci_device_t * device, uint8_t channel, uint16_t base_port);
void set_bus_master_base(pci_device_t * device, uint16_t base_port);
status_t ide_polling(ide_device_t * device, uint32_t advanced_check);

void _ide_isr(int vector, int code);

status_t _ide_init(void) {
	pci_device_list_t * list = &_pci_devices;
	//@TODO panic here!!!!!
	if(!list)
		return E_SUCCESS;

	pci_device_t * device = list->first;
	ide_controller_t * ide_controller;
	ide_channel_t * ide_channel = 0;
	ide_device_t * ide_device = 0;
	ide_num_controllers = ide_num_channels = ide_num_devices = 0;
	int i,j,k;
	uint8_t ide_buf[2048] = {0};
	unsigned char device_added = 0,
				  			channel_added = 0,
				  			register_read = 1,
				  			ide_type = 0;

	//__asm__("cli");
	
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
#ifdef IDE_DEBUG_MSG
			c_printf("- IDE Device, VEN: 0x%x, DEV: 0x%x\n", ide_controller->VID, ide_controller->DID);
#endif

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
#ifdef IDE_DEBUG_MSG
				c_printf(" - Channel: %d, CMD_BAR: 0x%x, CNL_BAR: 0x%x, BM_BASE: 0x%x\n",i,ide_channel->base_io_register,ide_channel->base_control_register,ide_channel->bus_master_base);
#endif
				
				for(j = 0; j < 2; ++j) { //cycle through the possible ide devices on the channel
					//get the device pointer
					ide_device = &(ide_devices[ide_num_devices]);
					
					//get some default values
					ide_device->ide_channel = ide_channel;
					ide_device->drive = j;
					register_read = 1;
					
					//select the drive
					ide_register_write(ide_device, ATA_REG_HDDEVSEL, 0xa0 | (j << 4));
					__delay(1);
					
					//send the identify command over
					ide_register_write(ide_device, ATA_REG_CONTROL, 2);
					ide_register_write(ide_device, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
					__delay(1);
					
					//read status
					while(register_read) {
						ide_register_read(ide_device, ATA_REG_STATUS, &register_read);
						if(register_read & ATA_SR_ERR) {register_read = 0xff; break;}
						if(!(register_read & ATA_SR_BSY) && (register_read & ATA_SR_DRQ)) break;
					}

					//debug output @TODO remove
#ifdef IDE_DEBUG_MSG
					c_printf("  - Drive: %d, Status: 0x%x\n", ide_device->drive, register_read);
#endif
					//if not a valid driver, don't do any more
					if(!register_read || register_read == 0xff || register_read == 0x6c)
						continue;

					++ide_num_devices;
					device_added = 1;

					//get the identification space of the drive
					ide_read_ident_space(ide_device, ide_buf);

					//set the data for the ide_device_t
					ide_device->valid = 1;
					ide_device->type = 0;
					ide_device->signature = *((uint16_t *)(ide_buf+ATA_IDENT_DEVICETYPE));
					ide_device->capabilities = *((uint16_t *)(ide_buf+ATA_IDENT_CAPABILITIES));
					ide_device->command_sets = *((uint32_t *)(ide_buf+ATA_IDENT_COMMANDSETS));
					if(ide_device->command_sets & (1 << 26))
						ide_device->size = *((uint32_t *)(ide_buf+ATA_IDENT_MAX_LBA_EXT));
					else
						ide_device->size = *((uint32_t *)(ide_buf+ATA_IDENT_MAX_LBA));
					for(k = 0; k < 40; ++k)
						ide_device->model[k] = ide_buf[ATA_IDENT_MODEL+2*(k/2)+1-k%2];
					ide_device->model[40] = 0;

					//display some more information about the drive
					//c_printf("   - Signature: 0x%x\n", ide_device->signature);
					//c_printf("   - Model: %s\n", ide_device->model);
					//c_printf("   - Size: %d Sectors\n", ide_device->size);
				}
				if(device_added) {
					++ide_num_channels;
					channel_added = 1;
					device_added = 0;
				}
			}
			if(channel_added) {
				++ide_num_controllers;
				channel_added = 0;
			}
		}
		device = device->next;
		
		//debug output @TODO remove
		//c_printf("\n");
	}
	//c_printf("before\n");
	//__asm__("sti");
	//c_printf("after\n");

	//loop through the drives found and print information
	c_printf("IDE Initialized\n");
	for(i = 0; i < ide_num_devices; ++i) {
		c_printf("Device: %d - %s\n", i, ide_devices[i].model);
		c_printf("  - Size: %d Sectors\n", ide_devices[i].size);
#ifdef IDE_DEBUG_MSG
		c_printf("  - Device on Channel: %x\n", ide_devices[i].drive);
#endif
	}

	//_interrupt_add_isr(_ide_isr, 0x23);

	//register system calls
	_syscall_install(_ide_pio_lba_read, IDE_SYSCALL_READ);
	_syscall_install(_ide_pio_lba_write, IDE_SYSCALL_WRITE);
	
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

status_t ide_read_ident_space(ide_device_t * device, uint8_t * buf) {
	uint32_t * _buf = (uint32_t *)buf;
	uint32_t i;

	for(i = 0; i < 128; ++i)
		_buf[i] = __inl(device->ide_channel->base_io_register);
	return E_SUCCESS;
}

status_t ide_pio_lba_read(ide_device_t * device, uint32_t sector, uint8_t * buf) {
	uint8_t drive_status;
	uint16_t * _buf = (uint16_t *)buf;
	uint32_t i;
	status_t status;

	//tell device not to use interrupts for this
	ide_register_write(device, ATA_REG_CONTROL, 0x2);

	//while the drive is busy, wait
	ide_register_read(device, ATA_REG_STATUS, &drive_status);
	while(drive_status & ATA_SR_BSY) ide_register_read(device, ATA_REG_STATUS, &drive_status);

	//select the drive
	ide_register_write(device, ATA_REG_HDDEVSEL, 0xe0 | (device->drive << 4));

	//write LBA data to registers, upper parts first then lower
	ide_register_write(device, ATA_REG_SECCOUNT1, 0);
	ide_register_write(device, ATA_REG_LBA3, (sector & 0xFF000000) >> 24);
	ide_register_write(device, ATA_REG_LBA4, 0);
	ide_register_write(device, ATA_REG_LBA5, 0);
	//now lower parts
	ide_register_write(device, ATA_REG_SECCOUNT0, 1);	
	ide_register_write(device, ATA_REG_LBA0, sector & 0xFF);
	ide_register_write(device, ATA_REG_LBA1, (sector & 0xFF00) >> 8);
	ide_register_write(device, ATA_REG_LBA2, (sector & 0xFF0000) >> 16);

	//send the command to the device
	ide_register_write(device, ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);

	//read in the data
	if((status = ide_polling(device, 1)))
		return status;

	for(i = 0; i < 256; ++i) {
		_buf[i] = __inw(device->ide_channel->base_io_register);
	}

	return E_SUCCESS;
}

status_t ide_pio_lba_write(ide_device_t * device, uint32_t sector, uint8_t * buf) {
	uint8_t drive_status;
	uint16_t * _buf = (uint16_t *)buf;
	uint32_t i;
	status_t status;

	//tell device not to use interrupts for this
	ide_register_write(device, ATA_REG_CONTROL, 0x2);

	//while the drive is busy, wait
	ide_register_read(device, ATA_REG_STATUS, &drive_status);
	while(drive_status & ATA_SR_BSY) ide_register_read(device, ATA_REG_STATUS, &drive_status);

	//select the drive
	ide_register_write(device, ATA_REG_HDDEVSEL, 0xe0 | (device->drive << 4));

	//write LBA data to registers, upper parts first then lower
	ide_register_write(device, ATA_REG_SECCOUNT1, 0);
	ide_register_write(device, ATA_REG_LBA3, (sector & 0xFF000000) >> 24);
	ide_register_write(device, ATA_REG_LBA4, 0);
	ide_register_write(device, ATA_REG_LBA5, 0);
	//now lower parts
	ide_register_write(device, ATA_REG_SECCOUNT0, 1);	
	ide_register_write(device, ATA_REG_LBA0, sector & 0xFF);
	ide_register_write(device, ATA_REG_LBA1, (sector & 0xFF00) >> 8);
	ide_register_write(device, ATA_REG_LBA2, (sector & 0xFF0000) >> 16);

	//send the command to the device
	ide_register_write(device, ATA_REG_COMMAND, ATA_CMD_WRITE_PIO_EXT);

	//read in the data
	if((status = ide_polling(device, 0)))
		return status;

	for(i = 0; i < 256; ++i)
		__outw(device->ide_channel->base_io_register, _buf[i]);

	//flush the cache
	ide_register_write(device, ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH_EXT);

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

status_t ide_polling(ide_device_t * device, uint32_t advanced_check) {
	int i = 0;
	uint8_t temp;
	//kill off 400ns by reading alternate status port
	for(i = 0; i < 4; ++i)
		ide_register_read(device, ATA_REG_ALTSTATUS, &temp);

	//wait for the device to not be busy
	ide_register_read(device, ATA_REG_STATUS, &temp);
	while(temp & ATA_SR_BSY) ide_register_read(device, ATA_REG_STATUS, &temp);

	if(advanced_check) {
		ide_register_read(device, ATA_REG_STATUS, &temp);

		if(temp & ATA_SR_ERR)
			return 2;
		if(temp & ATA_SR_DF)
			return 1;
		//if(temp & ATA_SR_DRQ)
			//return 3;
	}

	return E_SUCCESS;
}

status_t ide_read(uint32_t sector, uint8_t * buf);/* {
	//system call
	__asm__(
		"movl	$0x70, %eax\n"
		"int	$0x80"
	);
}*/

status_t ide_write(uint32_t sector, uint8_t * buf);/* {
	//system call
	__asm__(
		"movl	$0x71, %eax\n"
		"int	$0x80"
	);
}*/

void _ide_pio_lba_read(context_t * context) {
	uint32_t sector = ARG(context,1);
	uint8_t * buf = ARG(context,2);

#ifdef IDE_DEBUG_MSG
	c_printf("Reading from Sector: %x, to buf: %x\n",sector, buf);
#endif

	ide_pio_lba_read(&(ide_devices[1]), sector, buf);
	context->eax = E_SUCCESS;
}

void _ide_pio_lba_write(context_t * context) {
	uint32_t sector = ARG(context,1);
	uint8_t * buf = ARG(context,2);

#ifdef IDE_DEBUG_MSG
	c_printf("Writing to Sector: %x\n",sector);
#endif

	ide_pio_lba_write(&(ide_devices[1]), sector, buf);
	context->eax = E_SUCCESS;
}

void _ide_isr(int vector, int code) {
	uint32_t i = 0;
	uint8_t ret = 0;
	ide_device_t * ide_device = 0;
	
	//loop through all devices
	for(i = 0; i < ide_num_devices; ++i) {
		//grab the pointer to the device
		ide_device = &(ide_devices[i]);

		//read in teh status register
		ide_register_read(ide_device, ATA_REG_STATUS, &ret);

		if(ret & ATA_SR_ERR) { //if there is an error
			//read in error register
			ide_register_read(ide_device, ATA_REG_ERROR, &ret);
#ifdef IDE_DEBUG_MSG
			c_printf_at(0,7+i,"ide_device[%x] error: %x",i, ret);
#endif
		}
	}
}
