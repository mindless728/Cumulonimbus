#include "c_io.h"
#include "pci.h"
#include "kalloc.h"
#include "types.h"
#include "support.h"
#include "interrupts.h"
#include "intel8255x_ethernet.h"

intel_ethernet_t _i8255x_device;

status_t i8255x_driver_init(pci_device_list_t* list){
	status_t status = _pci_get_device(list, &_i8255x_device.pci, PCI_VENDOR_ID_INTEL, INTEL8255x_DEVICE_ID);

	if(status == E_BAD_PARAM){
		c_printf("ERROR: i8255x_driver_init failed with E_BAD_PARAM!\n");
		return status;
	}
	else if(status == E_NOT_FOUND){
		c_printf("ERROR: i8255x_driver_init failed with E_NOT_FOUND!\n");
		return status;
	}
	else if(_i8255x_device.pci== NULL){
		c_printf("ERROR: i8255x_driver_init got a null device pointer!\n");
		return E_NOT_FOUND;
	}

	_i8255x_device.csr_bar = (intel_csr_t*) _pci_base_addr(_i8255x_device.pci->config.headers.type0.bar[0]);


	_pci_print_config(_i8255x_device.pci);


	status = i8255x_driver_setup_irq();

	if(status != E_SUCCESS){
		//Failed to detect IRQ
		_i8255x_device.csr_bar->status |= INTEL_ETH_SCB_STATUS_SWI;
		c_printf("ERROR: i8255x_driver_init - Failed to determine IRQ\n");
		return E_NOT_FOUND;
	}

	c_printf("IRQ: 0x%x\n", _i8255x_device.irq_vector);
	c_printf("Command: 0x%x\n", _i8255x_device.csr_bar->command);
	c_printf("Status:  0x%x\n", _i8255x_device.csr_bar->status);


	//Allocate Recieve frame area(RFA)
	//Initialize RFA and all Receive Frame Descriptors(RFDs)

	//Tell Device about RFA

	//Check for

	return E_SUCCESS;
}


status_t i8255x_driver_setup_irq(){
	status_t status;
	int i = 0;
	for(; i < MAX_IRQ_FIND_TRIES; i++){
		c_printf("INFO: i8255x_driver_setup_irq - Try %d\n", i);

		asm("cli");	//Turn off maskable interrupts

		//Initialize IRQ related variables
		_i8255x_device.irq_vector = -1;
		_i8255x_device.wrong_irq = false;

		//Trigger software interrupt
		_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_TRIGGER_SI;
		status = _interrupt_wait(1000, &i8255x_driver_isr);
		//c_printf("Initial Status:  0x%x\n", _i8255x_device.csr_bar->status);

		//Setup our ISR
		//c_printf("Setup ISR\n");
		status = _interrupt_add_isr(&i8255x_driver_isr, _i8255x_device.irq_vector);

		if(status != E_SUCCESS){
			c_printf("ERROR: i8255x_driver_setup_irq - Failed to add ISR with status=0x%x\n", status);
			__panic("CRITICAL");
		}



		//Make sure we didn't get the wrong IRQ
		//We don't expect an interrupt since we didn't trigger one
		__delay_ms(10);
		status = _interrupt_wait_for_irq(1000, _i8255x_device.irq_vector);


		if(_i8255x_device.wrong_irq != false || status != E_TIMEOUT){
			//Remove installed ISR
			status = _interrupt_del_isr(&i8255x_driver_isr, _i8255x_device.irq_vector);

			if(status != E_SUCCESS){
				__panic("ERROR: i8255x_driver_setup_irq - Failed to remove ISR\n");
			}
			continue;
		}

		//Test out our ISR
		_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_TRIGGER_SI;
		__delay_ms(10);
		//c_printf("Final Status:  0x%x\n", _i8255x_device.csr_bar->status);
		if(_i8255x_device.csr_bar->status == 0x0){
			return E_SUCCESS;
		}
	}

	if(i == 3){
		return E_TOO_MANY_TRIES;
	}

	return status;
}


void i8255x_driver_isr(int vector, int code){
	//c_printf("_i8255x_driver_isr - Status:  0x%x\n", _i8255x_device.csr_bar->status);

	if(_i8255x_device.csr_bar->status == 0){
		c_printf("ERROR: _i8255x_driver_isr - WRONG IRQ!\n");
		_i8255x_device.wrong_irq = true;
		//__panic("ERROR: _i8255x_driver_isr - WRONG IRQ!\n");
	}
	else if((_i8255x_device.csr_bar->status && INTEL_ETH_SCB_STATUS_SWI) != 0){
		//Acknowledge SI interrupt
		_i8255x_device.csr_bar->status |= INTEL_ETH_SCB_STATUS_SWI;
		if(_i8255x_device.irq_vector == -1){
			_i8255x_device.irq_vector = vector;
			//c_printf("_i8255x_driver_isr - IRQ Vector:  0x%x\n", _i8255x_device.irq_vector);
		}
	}
}
