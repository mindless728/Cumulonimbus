#include "c_io.h"
#include "pci.h"
#include "ethernet.h"
#include "broadcom_ethernet.h"
#include "kalloc.h"
#include "types.h"
#include "support.h"


#define PRINT_ERROR(x) c_printf("ERROR: bcm_driver_init - code=0x%x\n", x)
#define ERROR_CHECK(x) if(x != E_SUCCESS){PRINT_ERROR(x);return;}



bcm_ethernet_t _bcm_device;



void bcm_driver_init(pci_device_list_t* list){

	status_t status = _pci_get_device(list, &_bcm_device.pci_device, BCM5788_PCI_VENDOR, BCM5788_PCI_DEVICE);

	if(status == E_BAD_PARAM){
		c_printf("ERROR: bcm_driver_init failed with E_BAD_PARAM!\n");
		return;
	}
	else if(status == E_NOT_FOUND){
		c_printf("ERROR: bcm_driver_init failed with E_NOT_FOUND!\n");
		return;
	}
	else if(_bcm_device.pci_device == NULL){
		c_printf("ERROR: bcm_driver_init got a null device pointer!\n");
		return;
	}

	_bcm_device.base_address = _pci_base_addr(&_bcm_device.pci_device->config.headers.type0.bar[0]) & 0xffff0000;

	c_printf("bcm_driver_init - Initializing network card...\n");

	__delay(100);

	uint8_t temp8=0;
	uint16_t temp16=0;
	uint32_t temp32=0;
	pci_addr_t tempAddr = _bcm_device.pci_device->address;
	uint8_t byte_offset = 0;

	//STEP1: Enable memory_space and bus_master bits of command register
	//Update command register
	tempAddr.offset = PCI_COMMAND_REG;
	status = _pci_set_bits(1, tempAddr, PCI_CMD_BUS_MASTER | PCI_CMD_MEMORY_SPACE, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step1 complete\n");

	//STEP2: Disable interrupts
	//Update Miscellaneous Host Control register(p.325)
	tempAddr.offset = PCI_REG_ADDR( BCM_MISC_HOST_CTL_REG );



	_pci_read_long(1, tempAddr, &temp32);
	c_printf("DEBUG: Step2 - BCM_MISC_HOST_CTL_REG = 0x%x\n", temp32);


	status = _pci_set_bits(1, tempAddr, BCM_MISC_HOST_CTL_MASK_PCI_INT |
										BCM_MISC_HOST_CTL_CLEAR_INTA |
										BCM_MISC_HOST_CTL_ENDIAN_WORD_SWAP |
										/*BCM_MISC_HOST_CTL_ENDIAN_BYTE_SWAP |*/
										BCM_MISC_HOST_CTL_INDIRECT_ACCESS, 1);
	ERROR_CHECK(status);

	__delay(1);

	_pci_read_long(1, tempAddr, &temp32);
	//bcm_indirect_reg_read(&_bcm_device, BCM_MISC_HOST_CTL_REG, &temp32);
	c_printf("DEBUG: Step2 - BCM_MISC_HOST_CTL_REG = 0x%x\n", temp32);


	//STEP3: Save the PCI Cache Line Size register(p.305) and PCI Subsystem Vendor ID registers(p.309)
	//uint8_t cacheLineSize = _bcm_device.pci_device->config.cacheLineSize;
	//uint16_t subsysVendorId = _bcm_device.pci_device->config.headers.type0.systemVendorId;

	//STEP4: Acquire the NVRAM lock(p.555)
	status = bcm_indirect_reg_read(&_bcm_device, BCM_NVM_SOFT_ARB_REG, &temp32);
	ERROR_CHECK(status);

	do{
		c_printf("DEBUG: Step4 - BCM_NVM_SOFT_ARB_REG = 0x%x\n", temp32);
		temp32 |= BCM_NVM_SOFT_ARB_REQ_SET(1);
		status = bcm_indirect_reg_write(&_bcm_device, BCM_NVM_SOFT_ARB_REG, temp32);
		ERROR_CHECK(status);

		//__delay(10);

		status = bcm_indirect_reg_read(&_bcm_device, BCM_NVM_SOFT_ARB_REG, &temp32);
		ERROR_CHECK(status);
	}
	while((temp32 & BCM_NVM_SOFT_ARB_WON(1)) == 0);

	c_printf("DEBUG: Step4 - BCM_NVM_SOFT_ARB_REG = 0x%x\n", temp32);

	c_printf("INFO: Step 4 complete\n");


	//STEP5(a): Set the Enable bit in the Memory Arbiter Mode register p460.
	status = bcm_indirect_reg_set_bits(&_bcm_device, BCM_MEM_ARB_MODE_REG, BCM_MEM_ARB_MODE_EA, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step 5a complete\n");


	//STEP5(b): Set the Enable_Indirect_Access bit of the Miscellaneous Host Control register (p.325)
	tempAddr.offset = PCI_REG_ADDR( BCM_MISC_HOST_CTL_REG );
	status = _pci_set_bits(1, tempAddr, BCM_MISC_HOST_CTL_INDIRECT_ACCESS, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step 5b-d complete\n");

	//STEP6: Write the T3_MAGIC_NUMBER (i.e., 0x4B657654) to the MAC memory at 0xb50 to notify the bootcode that the following reset is a warm reset.
	temp32 = BCM_T3_MAGIC_NUMBER;
	status = bcm_indirect_mem_write(&_bcm_device, BCM_STD_SOFT_GENCOMM_ADDR, temp32);
	ERROR_CHECK(status);
	c_printf("INFO: Step 6 complete\n");



	//STEP7: Reset the core clocks
	tempAddr.offset = PCI_REG_ADDR( BCM_MISC_CFG_SHADOW_REG );
	status = _pci_set_bits(1, tempAddr, BCM_MISC_CFG_CORE_CLK_RESET, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step 7 complete\n");

	//STEP8: Wait for core reset
	c_printf("INFO: Step 8 Waiting for core rest...");
	__delay(10);
	c_printf("Done\n");


	//STEP9: Disable interrupts, turn on indirect access, set endianness
	tempAddr.offset = PCI_REG_ADDR( BCM_MISC_HOST_CTL_REG );
	_pci_read_long(1, tempAddr, &temp32);
	c_printf("DEBUG: Step9 - BCM_MISC_HOST_CTL_REG = 0x%x\n", temp32);


	status = _pci_set_bits(1, tempAddr, BCM_MISC_HOST_CTL_MASK_PCI_INT |
										BCM_MISC_HOST_CTL_CLEAR_INTA |
										BCM_MISC_HOST_CTL_ENDIAN_WORD_SWAP |
										BCM_MISC_HOST_CTL_STATE_RW_EA |
										BCM_MISC_HOST_CTL_CLK_CTL_EA |
										BCM_MISC_HOST_CTL_INDIRECT_ACCESS, 1);
	ERROR_CHECK(status);

	__delay(1);
	_pci_read_long(1, tempAddr, &temp32);
	c_printf("DEBUG: Step9 - BCM_MISC_HOST_CTL_REG = 0x%x\n", temp32);


	//Step10: Enable memory_space and bus_master bits of command register
	//Update command register
	tempAddr.offset = PCI_REG_ADDR( PCI_COMMAND_REG );
	status = _pci_set_bits(1, tempAddr, PCI_CMD_BUS_MASTER | PCI_CMD_MEMORY_SPACE, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step10 - complete\n");

	//Step11: Disable pci-x relaxed ordering using PCI-X command register
	tempAddr.offset = PCI_REG_ADDR( BCM_PCI_X_CMD_REG );
	status = _pci_set_bits(1, tempAddr, BCM_PCI_X_CMD_RELAXED_ORDER_EA, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step11 - complete\n");

	//Step12: Enable MAC memory arbiter
	status = bcm_indirect_reg_set_bits(&_bcm_device, BCM_MEM_ARB_MODE_REG, BCM_MEM_ARB_MODE_EA, 1);
	ERROR_CHECK(status);
	c_printf("INFO: Step12 - complete\n");

	//Step13: Skip, specific to BCM5700 only

	//Step14: Enable PCI Clock control register
	//Skip, configured all needed registers in  in step 9

	//Step15: Configure Byte swapping for non-frame data and data byte-swap
	status = bcm_indirect_reg_set_bits(&_bcm_device, BCM_MODE_CTL_REG,
													/*BCM_MODE_CTL_BYTE_SWAP_DATA	|
													BCM_MODE_CTL_BYTE_SWAP_NFD	| */
													//BCM_MODE_CTL_WORD_SWAP_DATA	|
													BCM_MODE_CTL_USE_HOST_TX_BD_RINGS |
													BCM_MODE_CTL_WORD_SWAP_NFD	,
													1);
	ERROR_CHECK(status);
	c_printf("INFO: Step15 & 16 - complete\n");

	//Step17: Poll for bootcode completetion
	c_printf("INFO: Step17 - Waiting for bootcode completion...\n");
	temp32 = 0;
	while(temp32 != ~BCM_T3_MAGIC_NUMBER){
		status = bcm_indirect_mem_read(&_bcm_device, BCM_STD_SOFT_GENCOMM_ADDR, &temp32);
		ERROR_CHECK(status);
	}
	c_printf("INFO: Step17 - Boot code completed\n");

	//Step18: Initialize Ethernet MAC Mode register
	status = bcm_indirect_reg_write(&_bcm_device, BCM_ETH_MAC_MODE_REG, 0x0);
	ERROR_CHECK(status);
	c_printf("INFO: Step 18 complete\n");

	//Steps 19 through 21 are not needed for the BCM5788

	//Step22: Enable tagged modes - NOT SUPPORTED on BCM5788
	/*tempAddr.offset = PCI_REG_ADDR( BCM_MISC_HOST_CTL_REG );
	status = _pci_set_bits(1, tempAddr, BCM_MISC_HOST_CTL_TAGGED_STATUS_MODE, 1);
	ERROR_CHECK(status);*/

	//Step23: Restore cache line size and subsystem vendor ID
	tempAddr.offset = PCI_CACHE_LINE_SIZE_REG;
	status = _pci_write_byte(1, tempAddr, PCI_CACHE_LINE_SIZE_REG_OFFSET, _bcm_device.pci_device->config.cacheLineSize);
	ERROR_CHECK(status);


	tempAddr.offset = PCI_REG_ADDR(BCM_SUBSYS_VENDOR_ID_REG);
	status = _pci_write_byte(1, tempAddr, PCI_REG_OFFSET(BCM_SUBSYS_VENDOR_ID_REG), _bcm_device.pci_device->config.headers.type0.systemVendorId);
	ERROR_CHECK(status);
	c_printf("INFO: Step 23 complete\n");

	//Steps 24 through 26 are not needed for the BCM5788

	//Step 27: Configure DMA settings
	tempAddr.offset = PCI_REG_ADDR( BCM_DMA_RW_CTL_REG );
	status = _pci_write_long(1, tempAddr, BCM_DMA_RW_CTL_REG_SUGGESTED);
	ERROR_CHECK(status);
	c_printf("INFO: Step 27 complete\n");

	//Step 28 - Already configured in step 15

	//Step 29 - Configure host based send rings



	/*bcm_indirect_reg_read(&_bcm_device, BCM_ETH_LED_CTL_REG, &temp32);
	c_printf("INDIRECT MODE LED TEST 0x%x\n", temp32);

	temp32 = BCM_ETH_LED_CTL_TRAFFIC_LED | BCM_ETH_LED_CTL_1000Mbps_LED | BCM_ETH_LED_CTL_100Mbps_LED | BCM_ETH_LED_CTL_10Mbps_LED;

	bcm_indirect_reg_set_bits(&_bcm_device, BCM_ETH_LED_CTL_REG, BCM_ETH_LED_CTL_OVERR_TRAFFIC_LED | BCM_ETH_LED_CTL_OVERR_LINK_LED, 1);
	bcm_indirect_reg_set_bits(&_bcm_device, BCM_ETH_LED_CTL_REG, (0x3 << 11), 0);



	while(1){
		uint32_t state=0;

		bcm_indirect_reg_set_bits(&_bcm_device, BCM_ETH_LED_CTL_REG, temp32, 1);
		bcm_indirect_reg_read(&_bcm_device, BCM_ETH_LED_CTL_REG, &state);

		//state = (state >> 7) & 0x0f;

		c_printf("ON 0x%x\n", state);
		__delay(20);

		bcm_indirect_reg_set_bits(&_bcm_device, BCM_ETH_LED_CTL_REG, temp32, 0);
		bcm_indirect_reg_read(&_bcm_device, BCM_ETH_LED_CTL_REG, &state);

		//state = (state >> 7) & 0x0f;
		c_printf("OFF 0x%x\n", state);
		__delay(20);
	}*/

	uint32_t* ptr = (uint32_t*) bcm_std_get_ptr(&_bcm_device, Std_Raw, BCM_ETH_LED_CTL_REG);

	temp32 = *ptr;

	uint32_t devVen = *((uint32_t*) bcm_std_get_ptr(&_bcm_device, Std_Raw, 0x0000));

	c_printf("STANDARD MODE LED TEST 0x%x ptr=0x%x devVen=0x%x\n", temp32, _bcm_device.base_address, devVen);

	temp32 = BCM_ETH_LED_CTL_TRAFFIC_LED | BCM_ETH_LED_CTL_1000Mbps_LED | BCM_ETH_LED_CTL_100Mbps_LED | BCM_ETH_LED_CTL_10Mbps_LED;

	*ptr |= (BCM_ETH_LED_CTL_OVERR_TRAFFIC_LED | BCM_ETH_LED_CTL_OVERR_LINK_LED);
	*ptr &= ~(0x3<<11);

	while(1){
		uint32_t state=0;

		*ptr |= temp32;
		state = *ptr;
		c_printf("ON 0x%x\n", state);
		__delay(20);

		*ptr &= ~temp32;
		state = *ptr;
		c_printf("OFF 0x%x\n", state);
		__delay(20);
	}
}



/*

7.
Reset the core clocks. Set the CORE_CLock_Blocks-Reset bit in the General Control Miscellaneous Configuration
register (see “Miscellaneous Configuration Register (Offset 0x6804)” on page 504). The GPHY_Power_Down_Override
bit (bit-26) should also be set for the following MACs:


6.
Write the T3_MAGIC_NUMBER (i.e., 0x4B657654) to the MAC memory at 0xb50 to notify the bootcode that the following reset is a warm reset.


5
Prepare the chip for writing T3_MAGIC_NUMBER to device memory location 0xB50.
	a. Set the Enable bit in the Memory Arbiter Mode register (see “Memory Arbiter Mode Register (Offset 0x4000)” on page 460).
	b. Set the Enable_Indirect_Access bit of the Miscellaneous Host Control register (see “Miscellaneous Host Control Register (Offset 0x68)” on page 325).
	c. Initialize the Endian_Word_Swap and Endian_Byte_Swap bits of Miscellaneous Host Control register (offset 0x68) depending on the Host Platform Endianness (This step is required only if Indirect Access registers are accessed through memory mapped accesses).
	d. Initialize the Byte_Swap_Non_Frame_Data and Word_Swap_Non_Frame_Data bits of Mode Control register (offset 0x6800) to the required values depending on the Host Platform Endianness.


4.
Acquire the NVRAM lock for BCM5702 and later devices by setting the REQ_SET1 bit of the Software Arbitration
register (see “Software Arbitration Register (Offset 0x7020)” on page 555) and then waiting for the ARB_WON1 bit to
be set.

3.
Save the PCI Cache Line Size register and PCI Subsystem Vendor ID registers (see “Cache Line Size Register (Offset
0x0C)” on page 305 and “Subsystem ID Register (Offset 0x2E)” on page 309) in the PCI configuration space to
temporary variables. These registers must be restored after a core clock reset in Step 7.



2.
Disable interrupts (optional). If the device has not been initialized previously (power-on reset), the host software should
disable and clear interrupts prior to the core_clock reset in Step 7. Set the Mask_PCI_Interrupt_Output and
Clear_Interrupt_INTA bits in the Miscellaneous Host Control register (see “Miscellaneous Host Control Register (Offset
0x68)” on page 325).

1.
Enable MAC memory space decode and bus mastering (optional). If the device has not been initialized previously
(power on reset), the host software must enable these bits to issue the core clock reset in Step 7. Set the Bus_Master
and Memory_Space bits in the PCI Configuration Space Command register (see “Command Register (Offset 0x04)” on
page 302).
 */


status_t bcm_indirect_mem_read(bcm_ethernet_t* dev, uint32_t mem_addr, uint32_t* data){
	//Set the pci register address
	pci_addr_t pci_reg_addr = dev->pci_device->address;
	pci_reg_addr.offset = PCI_REG_ADDR(BCM_MEM_WINDOW_BAR_REG);


	//Write offset to desired register
	if(_pci_write_long(1, pci_reg_addr, mem_addr) != E_SUCCESS){
		__panic("ERROR: bcm_indirect_mem_read - Failed to write BCM_MEM_WINDOW_BAR_REG");
	}

	pci_reg_addr.offset = PCI_REG_ADDR(BCM_MEM_WINDOW_DATA_RW_REG);
	//Read value of desired register
	return _pci_read_long(1, pci_reg_addr, data);
}

status_t bcm_indirect_mem_write(bcm_ethernet_t* dev, uint32_t mem_addr, uint32_t data){
	//Set the pci register address
	pci_addr_t pci_reg_addr = dev->pci_device->address;
	pci_reg_addr.offset = PCI_REG_ADDR(BCM_MEM_WINDOW_BAR_REG);


	//Write offset to desired register
	if(_pci_write_long(1, pci_reg_addr, mem_addr) != E_SUCCESS){
		__panic("ERROR: bcm_indirect_mem_read - Failed to write BCM_MEM_WINDOW_BAR_REG");
	}

	pci_reg_addr.offset = PCI_REG_ADDR(BCM_MEM_WINDOW_DATA_RW_REG);
	//Write value of desired register
	return _pci_write_long(1, pci_reg_addr, data);
}


status_t bcm_indirect_reg_read(bcm_ethernet_t* dev, uint32_t reg_addr, uint32_t* data){
	//Set the pci register address
	pci_addr_t pci_reg_addr = dev->pci_device->address;
	pci_reg_addr.offset = PCI_REG_ADDR(BCM_REG_BAR_REG);


	//Write offset to desired register
	if(_pci_write_long(1, pci_reg_addr, reg_addr) != E_SUCCESS){
		__panic("ERROR: bcm_indirect_reg_read - Failed to write BCM_REG_BAR_REG\n");
	}

	pci_reg_addr.offset = PCI_REG_ADDR(BCM_REG_DATA_RW_REG);
	//Read value of desired register
	return _pci_read_long(1, pci_reg_addr, data);
}

status_t bcm_indirect_reg_write(bcm_ethernet_t* dev, uint32_t reg_addr, uint32_t data){
	//Set the pci register address
	pci_addr_t pci_reg_addr = dev->pci_device->address;
	pci_reg_addr.offset = PCI_REG_ADDR(BCM_REG_BAR_REG);


	//Write offset to desired register
	if(_pci_write_long(1, pci_reg_addr, reg_addr) != E_SUCCESS){
		__panic("ERROR: bcm_indirect_reg_read - Failed to write BCM_REG_BAR_REG\n");
	}

	pci_reg_addr.offset = PCI_REG_ADDR(BCM_REG_DATA_RW_REG);
	//Write value of desired register
	return _pci_write_long(1, pci_reg_addr, data);
}

status_t bcm_indirect_reg_set_bits(bcm_ethernet_t* dev, uint32_t reg, uint32_t mask, boolean_t state){
	uint32_t value;
	status_t status = bcm_indirect_reg_read(dev, reg, &value);

	if(status != E_SUCCESS){
		return status;
	}

	if(state == 0){
		value &= ~mask;
	}
	else{
		value |= mask;
	}

	return bcm_indirect_reg_write(dev, reg, value);
}

void* bcm_std_get_ptr(bcm_ethernet_t* dev, uint8_t type, uint32_t offset){
	uint32_t ptr = ((uint32_t)dev->base_address) + offset;

	if(type == Std_Pci){
		ptr += BCM_STD_PCI_CONFIG_OFFSET;
	}
	else if(type == Std_Mailboxes){
		ptr += BCM_STD_HIGH_PRIO_MAIL_OFFSET;
	}
	else if(type == Std_Regs){
		ptr += BCM_STD_REGISTER_OFFSET;
	}
	else if(type == Std_Mem){
		ptr += BCM_STD_MEM_WINDOW_OFFSET;
	}

	return (void*) ptr;
}
