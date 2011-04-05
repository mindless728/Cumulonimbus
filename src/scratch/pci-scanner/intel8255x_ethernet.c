#include "c_io.h"
#include "pci.h"
#include "kalloc.h"
#include "types.h"
#include "utils.h"
#include "support.h"
#include "interrupts.h"
#include "intel8255x_ethernet.h"

intel_ethernet_t _i8255x_device;
//struct intel_csr global_csr = { 0, 0, 0,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};

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

	c_printf("================ TEST 1 ================\n");

	//Setup RU and CU base
	c_printf("Load CU Base\n");
	i8255x_write_cu_cmd(SCB_CMD_CUC_LD_CU_BASE, 0x00000000);

	c_printf("Load RU Base\n");
	i8255x_write_ru_cmd(SCB_CMD_RUC_LD_RU_BASE, 0x00000000);
	c_printf("\n");


	/*c_printf("0x%x", _i8255x_device.csr_bar);

	while(1);*/

	//c_printf("0x%d", sizeof(struct intel_csr));

	//Setup general command base
	_i8255x_device.command_base = (intel_action_hdr_t*)(((uint32_t)_i8255x_device.csr_bar) + (uint32_t)sizeof(struct intel_csr));



	void* old_base_ptr = 0; //kalloc_set_base(_i8255x_device.command_base);

	c_printf("csr_bar = 0x%x\n", _i8255x_device.csr_bar);
	//Make sure command base is aligned
	_i8255x_device.command_base = kalloc_aligned(sizeof(intel_action_hdr_t), Align_QWord);

	c_printf("command_base = 0x%x\n", &_i8255x_device.command_base[0]);
	c_printf("command_base = 0x%x\n", _i8255x_device.command_base);

	_i8255x_device.csr_bar->command = 0x00;

	//Setup a NOP command
	_i8255x_device.command_base[0].command = CU_ACTION_NOP | ACTION_HDR_CMD_EL;
	_i8255x_device.command_base[0].status = 0x0000;
	_i8255x_device.command_base[0].link_addr = 0x00000000;

	_i8255x_device.cu_transition = false;
	i8255x_write_cu_cmd(SCB_CMD_CUC_START, (uint32_t)_i8255x_device.command_base);

	c_printf(".");

	//_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_TRIGGER_SI;

	int j=0;
	for(j=0; j<3; j++){
		if(_i8255x_device.cu_transition == true){
			c_printf("\nCU Transitioned\n");
			break;
		}
		c_printf("|");
		__delay_ms(5000);
	}


	c_printf("Command: 0x%x\n", _i8255x_device.csr_bar->command);
	c_printf("Status:  0x%x\n", _i8255x_device.csr_bar->status);

	//Restore allocator base_ptr
	//kalloc_set_base(old_base_ptr);

	return E_SUCCESS;


	//Allocate Recieve frame area(RFA)
	//Initialize RFA and all Receive Frame Descriptors(RFDs)

	//Tell Device about RFA

	//Check for

	return E_SUCCESS;
}




void i8255x_write_ru_cmd(uint8_t cmd, uint32_t generel_ptr){
	_i8255x_device.csr_bar->general_ptr = generel_ptr;
	//uint16_t cmdTemp = _i8255x_device.csr_bar->command;
	//cmdTemp &= ~INTEL_ETH_SCB_CMD_RUC_MASK;
	//cmdTemp |= (INTEL_ETH_SCB_CMD_RUC_MASK & cmd) << INTEL_ETH_SCB_CMD_RUC_SHIFT;
	//_i8255x_device.csr_bar->command = cmdTemp;

	c_printf("DEBUG: i8255x_write_ru_cmd - pre  0x%x, 0x%x\n", _i8255x_device.csr_bar->command, _i8255x_device.csr_bar->status);

	_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_RUC_MASK & (cmd << INTEL_ETH_SCB_CMD_RUC_SHIFT);

	c_printf("DEBUG: i8255x_write_ru_cmd - mid  0x%x, 0x%x\n", _i8255x_device.csr_bar->command, _i8255x_device.csr_bar->status);


	while((_i8255x_device.csr_bar->command & INTEL_ETH_SCB_CMD_RUC_MASK) != 0x0){
		//Wait for command acceptance
		c_printf("r");
	}
	c_printf("DEBUG: i8255x_write_ru_cmd - post 0x%x, 0x%x\n", _i8255x_device.csr_bar->command, _i8255x_device.csr_bar->status);
}


void i8255x_write_cu_cmd(uint8_t cmd, uint32_t generel_ptr){
	_i8255x_device.csr_bar->general_ptr = generel_ptr;
	/*uint16_t cmdTemp = _i8255x_device.csr_bar->command;
	cmdTemp &= ~INTEL_ETH_SCB_CMD_CUC_MASK;
	cmdTemp |= (INTEL_ETH_SCB_CMD_CUC_MASK & cmd) << INTEL_ETH_SCB_CMD_CUC_SHIFT;

	_i8255x_device.csr_bar->command = cmdTemp;*/

	c_printf("DEBUG: i8255x_write_cu_cmd - setting cu = 0x%x\n", (INTEL_ETH_SCB_CMD_CUC_MASK & (cmd << INTEL_ETH_SCB_CMD_CUC_SHIFT)));

	c_printf("DEBUG: i8255x_write_cu_cmd - pre  0x%x, 0x%x\n", _i8255x_device.csr_bar->command, _i8255x_device.csr_bar->status);

	_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_CUC_MASK & (cmd << INTEL_ETH_SCB_CMD_CUC_SHIFT);

	//c_printf("DEBUG: i8255x_write_cu_cmd - mid  0x%x, 0x%x\n", _i8255x_device.csr_bar->command, _i8255x_device.csr_bar->status);

	while((_i8255x_device.csr_bar->command & INTEL_ETH_SCB_CMD_CUC_MASK) != 0x0){
		//Wait for command acceptance
		c_printf("c");
	}
	c_printf("DEBUG: i8255x_write_cu_cmd - post 0x%x, 0x%x\n", _i8255x_device.csr_bar->command, _i8255x_device.csr_bar->status);
}



void i8255x_driver_isr(int vector, int code){
	//c_printf("_i8255x_driver_isr - Status:  0x%x\n", _i8255x_device.csr_bar->status);

	if(_i8255x_device.csr_bar->status == 0){
		c_printf("ERROR: _i8255x_driver_isr - WRONG IRQ, expected %x got %x!\n", _i8255x_device.irq_vector, vector);
		_i8255x_device.wrong_irq = true;
		//__panic("ERROR: _i8255x_driver_isr - WRONG IRQ!\n");
	}
	else{
		boolean_t handled = false;

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_SWI) != 0){
			c_printf("INFO: _i8255x_driver_isr - SWI\n");
			//_i8255x_device.csr_bar->status |= INTEL_ETH_SCB_STATUS_SWI;
			if(_i8255x_device.irq_vector == -1){
				_i8255x_device.irq_vector = vector;
				//c_printf("_i8255x_driver_isr - IRQ Vector:  0x%x\n", _i8255x_device.irq_vector);
			}
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_CX) != 0){
			c_printf("INFO: _i8255x_driver_isr - CX\n");
			_i8255x_device.cu_transition = true;
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_FR) != 0){
			c_printf("INFO: _i8255x_driver_isr - FR\n");
			//TODO: Wake up sleeping process thats waiting for packet info
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_CNA) != 0){
			c_printf("INFO: _i8255x_driver_isr - CNA\n");
			_i8255x_device.cu_transition = true;
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_RNR) != 0){
			c_printf("INFO: _i8255x_driver_isr - RNR\n");
			_i8255x_device.ru_transition = true;
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_MDI) != 0){
			c_printf("INFO: _i8255x_driver_isr - MDI\n");
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_FCP) != 0){
			c_printf("INFO: _i8255x_driver_isr - FCP\n");
			handled=true;
			//TODO: Punish last sending process
		}

		if(handled != true){
			c_printf("ERROR: _i8255x_driver_isr - Unexpected status 0x%x\n", _i8255x_device.csr_bar->status);
		}


		//Acknowledge all interrupts
		_i8255x_device.csr_bar->status |= INTEL_ETH_SCB_STATUS_ACK_MASK;

		c_printf("_i8255x_driver_isr - Command: 0x%x\n", _i8255x_device.csr_bar->command);
		c_printf("_i8255x_driver_isr - Status:  0x%x\n", _i8255x_device.csr_bar->status);
	}
}


status_t i8255x_driver_setup_irq(void){
	status_t status;
	int i = 0;
	for(; i < MAX_IRQ_FIND_TRIES; i++){
		c_printf("INFO: i8255x_driver_setup_irq - Try %d\n", i);


		//Initialize IRQ related variables
		_i8255x_device.irq_vector = -1;
		_i8255x_device.wrong_irq = false;

		asm("cli");	//Turn off maskable interrupts

		_i8255x_device.csr_bar->status |= INTEL_ETH_SCB_STATUS_ACK_MASK;

		//Trigger software interrupt
		_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_TRIGGER_SI;
		status = _interrupt_wait(1000, &i8255x_driver_isr);

		if(status != E_SUCCESS){
			c_printf("ERROR: i8255x_driver_setup_irq - Wait was inconclusive\n");
			//i=0;
			__delay_ms(1000);
			continue;
		}

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
		__delay_ms(100);
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
