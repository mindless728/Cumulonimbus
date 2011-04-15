#include "ide.h"
#include "startup.h"
#include "utils.h"
#include "../pci-scanner/pci.h"
#include "../IOPorts/ioports.h"
#include "../pci-scanner/types.h"
#include "../MBR/mbr.h"
#include "support.h"
#include "c_io.h"

pci_device_list_t * pci_devices = NULL;
int count = 0;

void dummy(int vector, int code);

int main(void) {
	status_t status;
	int i = 0, j = 0;
	uint8_t buf[512] = {0};
	ide_device_t * device;
	MBR_t * mbr = (MBR_t *)buf;
	
	asm("cli");
	__init_interrupts();
	__install_isr(0x23, dummy);
	__install_isr(0x27, dummy);
	__install_isr(0x2a, dummy);
	asm("sti");
	
	c_io_init();
	ioports_init();
	
	//allocate memory for pci list
	_pci_alloc_device_list(&pci_devices);
	
	//scan for pci devices
	status = _pci_scan(pci_devices);
	
	//clear the screen
	c_clearscreen();
	c_moveto(0,0);
	
	//initialize the ide system
	ide_init(pci_devices);
	
	//ide initialization done
	c_printf("IDE Initialized!\n");

	c_printf("Controllers: %d, Channels: %d, Devices: %d\n",ide_num_controllers, ide_num_channels, ide_num_devices);
		
	while(1) {}
	
	return 0;
}

void dummy(int vector, int code) {
}
