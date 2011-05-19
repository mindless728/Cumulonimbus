#include "c_io.h"
#include "pci.h"
//#include "net.h"
#include "kalloc.h"
#include "types.h"
#include "utils.h"
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
		c_printf("ERROR: i8255x_driver_init - Failed to auto determine IRQ\n");
		return E_NOT_FOUND;
	}


	c_printf("IRQ: 0x%x\n", _i8255x_device.irq_vector);
	c_printf("Command: 0x%x\n", _i8255x_device.csr_bar->command);
	c_printf("Status:  0x%x\n", _i8255x_device.csr_bar->status);

	//Setup CU and RU base
	i8255x_write_cu_cmd(SCB_CMD_CUC_LD_CU_BASE, 0x00000000);
	i8255x_write_ru_cmd(SCB_CMD_RUC_LD_RU_BASE, 0x00000000);

	//Setup dump comand
	intel_action_dump_t dump;
	dump.header.command = CU_ACTION_DUMP | ACTION_HDR_CMD_EL | ACTION_HDR_CMD_I;
	dump.header.status = 0x0000;
	dump.header.link_addr = 0x00000000;
	dump.offset = (uint32_t) dump.buffer;

	//Start command processing
	i8255x_write_cu_cmd(SCB_CMD_CUC_START, (uint32_t)&dump);

	int timeout = 15000;
	while(timeout > 0){
		if(_i8255x_device.cu_transition == true){
			c_printf("\nCU Transitioned\n");
			break;
		}
		timeout -= 500;
		__delay_ms(500);
	}

	int j=0;
	if(timeout > 0){
		if((dump.header.status & ACTION_HDR_STATUS_C) != 0){
			c_printf("\nCommand completed\n");

			if((dump.header.status & ACTION_HDR_STATUS_OK) != 0){
				for(j=0; j<6; j++){
					_i8255x_device.mac_addr[j] = dump.buffer[39+j];
				}
			}
			else{
				c_printf("ERROR: Failed to dump data!\n");
				return E_ERROR;
			}
		}
		else{
			c_printf("ERROR: Dump command failed to complete!\n");
			return E_ERROR;
		}
	}
	else{
		c_printf("ERROR: Waited too long for command to complete!\n");
		return E_TIMEOUT;
	}


	i8255x_setup_rfa();

	c_printf("Command: 0x%x\n", _i8255x_device.csr_bar->command);
	c_printf("Status:  0x%x\n", _i8255x_device.csr_bar->status);

	__delay_ms(150);

	c_clearscreen();

	c_printf("MAC Address = ");
	for(j=0; j<6; j++){

		if(j==5){
			c_printf("%x\n", _i8255x_device.mac_addr[j]);
			continue;
		}
		c_printf("%x:", _i8255x_device.mac_addr[j]);
	}
	c_printf("Receiving frames now... \n");

	i8255x_write_ru_cmd(SCB_CMD_RUC_START, (uint32_t)_i8255x_device.rx_buffer_base);

	uint32_t buffer_size = sizeof(intel_tx_buffer_t)+(ETH_DATA_LEN+ETH_HLEN);
	_i8255x_device.tx_buffer_base = kalloc_aligned(buffer_size, Align_DWord);

	//Allocate Recieve frame area(RFA)


	//Initialize RFA and all Receive Frame Descriptors(RFDs)

	//Tell Device about RFA


	return E_SUCCESS;
}

status_t i8255x_driver_transmit(uint8_t* frame, uint16_t size, boolean_t blocking){
	uint8_t* data = (uint8_t*) &(_i8255x_device.tx_buffer_base[1]);

	int i=0;
	for(; i<size; i++){
		data[i] = frame[i];
	}

	int index=6;
	for(i=0; i<6; i++){
		data[i+index] = _i8255x_device.mac_addr[i];
	}

	memset(_i8255x_device.tx_buffer_base, 0x00, sizeof(intel_tx_buffer_t));
	//_i8255x_device.tx_buffer_base->tbd_addr = 0xffffffff;
	_i8255x_device.tx_buffer_base->tbd_addr = 0x0;
	_i8255x_device.tx_buffer_base->header.command = ACTION_HDR_CMD_EL | ACTION_HDR_CMD_I | ACTION_HDR_CMD_S | CU_ACTION_TRANSMIT;

	_i8255x_device.tx_buffer_base->tcb_byte_count = size;

	i8255x_write_cu_cmd(SCB_CMD_CUC_START, (uint32_t)_i8255x_device.tx_buffer_base);

	return E_SUCCESS;
}

void i8255x_init_rxb(struct intel_rx_buffer* rx_buf){
	rx_buf->header.status = 0x0;
	rx_buf->header.command = 0x0;
	rx_buf->size = ETH_DATA_LEN;
	rx_buf->header.command = ACTION_HDR_CMD_EL | ACTION_HDR_CMD_S;
}

status_t i8255x_setup_rfa(void){
	int i = 0;
	status_t status = E_SUCCESS;
	intel_rx_buffer_t* first_buf = NULL;
	intel_rx_buffer_t* prev_buf = NULL;

	do{
		intel_rx_buffer_t* rx_buf = kalloc_aligned(sizeof(intel_rx_buffer_t), Align_DWord);

		memset(rx_buf, 0x00, sizeof(intel_rx_buffer_t));

		rx_buf->size = ETH_DATA_LEN;

		if(prev_buf != NULL){
			prev_buf->header.link_addr = (uint32_t)rx_buf;
		}

		if(first_buf == NULL){
			first_buf = rx_buf;
		}

		prev_buf = rx_buf;
		i++;
	}
	while(i<I8255X_RX_BUFFER_COUNT && status == E_SUCCESS);

	prev_buf->header.link_addr = (uint32_t) first_buf;
	prev_buf->header.command = ACTION_HDR_CMD_EL | ACTION_HDR_CMD_S;

	_i8255x_device.rx_buffer_base = first_buf;
	_i8255x_device.rx_buffer_ptr = first_buf;
	_i8255x_device.rx_buffer_end = prev_buf;

	c_printf("i8255x_setup_rfa - Allocated %d RFDs totalling %d bytes\n", i, I8255X_RX_BUFFER_SIZE);

	return status;
}


void i8255x_write_ru_cmd(uint8_t cmd, uint32_t generel_ptr){
	_i8255x_device.csr_bar->general_ptr = generel_ptr;

	_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_RUC_MASK & (cmd << INTEL_ETH_SCB_CMD_RUC_SHIFT);

	while((_i8255x_device.csr_bar->command & INTEL_ETH_SCB_CMD_RUC_MASK) != 0x0){
		//Wait for command acceptance
	}
}


void i8255x_write_cu_cmd(uint8_t cmd, uint32_t generel_ptr){
	_i8255x_device.csr_bar->general_ptr = generel_ptr;
	_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_CUC_MASK & (cmd << INTEL_ETH_SCB_CMD_CUC_SHIFT);

	while((_i8255x_device.csr_bar->command & INTEL_ETH_SCB_CMD_CUC_MASK) != 0x0){
		//Wait for command acceptance
	}
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
			//Software Interrupt
			c_printf("INFO: _i8255x_driver_isr - SWI\n");
			if(_i8255x_device.irq_vector == -1){
				_i8255x_device.irq_vector = vector;
			}
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_CX) != 0){
			//Command Unit Execution finished
			//c_printf("INFO: _i8255x_driver_isr - CX\n");
			_i8255x_device.cu_transition = true;
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_FR) != 0){
			//Recieve unit finished recieving frame
			//c_printf("INFO: _i8255x_driver_isr - FR\n");

			//TODO: Wake up sleeping process thats waiting for packet info

			intel_rx_buffer_t* rx_buf = _i8255x_device.rx_buffer_ptr;

			if(rx_buf->header.status == 0x00){
				c_printf("INFO: _i8255x_driver_isr - FR Nothing to do\n");
			}

			while(rx_buf->header.status != 0x00){

				//Did we get back to the start?
				if(rx_buf->header.link_addr == (uint32_t)_i8255x_device.rx_buffer_ptr){
					break;
				}

				_i8255x_device.rx_count++;

				//DELIVER FRAME HERE
				struct ethframe* frame = (ethframe_t*) &rx_buf->frame[0];
				if(frame->header.proto == htons(0xcafe)){
					c_printf("MSG: %s\n", frame->data);
				}


				/*c_printf("PACKET - status=0x%x EOF=%d F=%d actual_count=0x%x rx_count=%d\n",
							rx_buf->header.status,
							((rx_buf->actual_count>>15) & 1),
							((rx_buf->actual_count>>14) & 1),
							((~(1<<15) & ~(1<<14)) & rx_buf->actual_count),
							_i8255x_device.rx_count); */

				//Print ethernet header
				int i=0;
				for(; i<ETH_HLEN; i++){
					//c_printf("%x ", rx_buf->frame[i]);
				}
				//c_printf("\n");


				if((rx_buf->header.command & ACTION_HDR_CMD_EL) != 0x0){
					//RU stopped here
					i8255x_init_rxb(rx_buf);
					rx_buf = (intel_rx_buffer_t*) rx_buf->header.link_addr;
					break;
				}

				i8255x_init_rxb(rx_buf);
				rx_buf = (intel_rx_buffer_t*) rx_buf->header.link_addr;
			}

			_i8255x_device.rx_buffer_ptr = rx_buf;
			//__delay_ms(5000);
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_CNA) != 0){
			//CU not Active
			//c_printf("INFO: _i8255x_driver_isr - CNA\n");
			_i8255x_device.cu_transition = true;
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_RNR) != 0){
			//Receive unit Not Ready
			//c_printf("INFO: _i8255x_driver_isr - RNR\n");
			_i8255x_device.ru_transition = true;
			handled=true;
			_i8255x_device.rx_buffer_base = (intel_rx_buffer_t*) _i8255x_device.rx_buffer_base->header.link_addr;
			i8255x_write_ru_cmd(SCB_CMD_RUC_START, (uint32_t)_i8255x_device.rx_buffer_base);
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_MDI) != 0){
			//MDI Write complete
			c_printf("INFO: _i8255x_driver_isr - MDI\n");
			handled=true;
		}

		if((_i8255x_device.csr_bar->status & INTEL_ETH_SCB_STATUS_FCP) != 0){
			//Flow Control Pause
			c_printf("INFO: _i8255x_driver_isr - FCP\n");
			handled=true;
			//TODO: Punish last sending process
		}

		if(handled != true){
			c_printf("ERROR: _i8255x_driver_isr - Unexpected status 0x%x\n", _i8255x_device.csr_bar->status);
		}


		//Acknowledge all interrupts
		_i8255x_device.csr_bar->status |= INTEL_ETH_SCB_STATUS_ACK_MASK;

		//c_printf("_i8255x_driver_isr - Command: 0x%x\n", _i8255x_device.csr_bar->command);
		//c_printf("_i8255x_driver_isr - Status:  0x%x\n", _i8255x_device.csr_bar->status);
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
		//Try using the default IRQ
		_i8255x_device.irq_vector = I8255X_DEFAULT_IRQ;
		status = _interrupt_add_isr(&i8255x_driver_isr, I8255X_DEFAULT_IRQ);


		if(status != E_SUCCESS){
			c_printf("ERROR: i8255x_driver_setup_irq - Failed to add ISR with status=0x%x\n", status);
			__panic("CRITICAL");
		}

		//Test out our ISR
		_i8255x_device.csr_bar->command |= INTEL_ETH_SCB_CMD_TRIGGER_SI;
		__delay_ms(10);
		//c_printf("Final Status:  0x%x\n", _i8255x_device.csr_bar->status);
		if(_i8255x_device.csr_bar->status == 0x0){
			c_printf("INFO: i8255x_driver_setup_irq -  Using default IRQ\n");
			return E_SUCCESS;
		}

		_interrupt_del_isr(&i8255x_driver_isr, I8255X_DEFAULT_IRQ);

		return E_TOO_MANY_TRIES;
	}

	return status;
}
