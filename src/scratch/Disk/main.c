#include "ide.h"
#include "startup.h"
#include "utils.h"
#include "pci.h"
#include "types.h"
#include "support.h"
#include "c_io.h"

pci_device_list_t* pci_devices = NULL;

int main(void) {
	pci_device_t * controller;
	status_t status;
	int i;
	
	asm("cli");
	
	//allocate memory for pci list
	_pci_alloc_device_list(&pci_devices);
	
	//scan for pci devices
	status = _pci_scan(pci_devices);
	
	//clear the screen
	c_clearscreen();
	
	//get the ide controller
	status = _pci_get_device(pci_devices, &controller, 0x8086, 0x27c0);
	if(status != E_SUCCESS) {
		c_printf("ERROR: IDE device not found");
		while(1) {}
	}
	
	//print that the device was found
	c_printf("IDE device found\n");
	for(i = 0; i < 6; ++i)
		c_printf("BAR%d - 0x%x\n",i,controller->config.headers.type0.bar[i]);
	c_printf("IRQLine: 0x%x\n",controller->config.headers.type0.irqLine);
	c_printf("IRQPin: 0x%x\n",controller->config.headers.type0.irqPin);
	
	c_printf("IDE test execution done");
	/*c_printf("\nwaiting for 10s to run more tests");
	__delay(100);
	
	c_clearscreen();*/
		
	while(1) {}
	
	return 0;
}