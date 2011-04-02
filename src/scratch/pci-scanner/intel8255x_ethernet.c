#include "c_io.h"
#include "pci.h"
#include "kalloc.h"
#include "types.h"
#include "support.h"
#include "intel8255x_ethernet.h"

intel_ethernet_t _i8255x_device;

void i8255x_driver_init(pci_device_list_t* list){
	status_t status = _pci_get_device(list, &_i8255x_device.pci, PCI_VENDOR_ID_INTEL, INTEL8255x_DEVICE_ID);

	if(status == E_BAD_PARAM){
		c_printf("ERROR: i8255x_driver_init failed with E_BAD_PARAM!\n");
		return;
	}
	else if(status == E_NOT_FOUND){
		c_printf("ERROR: i8255x_driver_init failed with E_NOT_FOUND!\n");
		return;
	}
	else if(_i8255x_device.pci== NULL){
		c_printf("ERROR: i8255x_driver_init got a null device pointer!\n");
		return;
	}

	_i8255x_device.csr_bar = (intel_ethernet_t*) _pci_base_addr(_i8255x_device.pci->config.headers.type0.bar[0]);


	_pci_print_config(_i8255x_device.pci);

	//Trigger interrupt
	//_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_TRIGGER_SI;

	c_printf("Command: 0x%x\n", _i8255x_device.csr_bar->command);
	c_printf("Status:  0x%x\n", _i8255x_device.csr_bar->status);
}
