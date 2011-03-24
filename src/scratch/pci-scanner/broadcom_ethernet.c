#include "c_io.h"
#include "pci.h"
#include "ethernet.h"
#include "broadcom_ethernet.h"
#include "kalloc.h"
#include "types.h"


#define PRINT_ERROR(x) c_printf("ERROR: bcm_driver_init - code=0x%x\n", x)
#define ERROR_CHECK(x) if(x != E_SUCCESS){PRINT_ERROR(x);return;}

struct bcm_ethernet_t{
	pci_device_t* pci_device;

};

pci_device_t* bcm_device=0;
uint8_t bcm_cache_line;


void bcm_driver_init(pci_device_list_t* list){

	status_t status = _pci_get_device(*list, &bcm_device, BCM5788_PCI_VENDOR, BCM5788_PCI_DEVICE);

	if(status == E_BAD_PARAM){
		c_printf("ERROR: bcm_driver_init failed with E_BAD_PARAM!\n");
		return;
	}
	else if(status == E_NOT_FOUND){
		c_printf("ERROR: bcm_driver_init failed with E_NOT_FOUND!\n");
		return;
	}
	else if(bcm_device == NULL){
		c_printf("ERROR: bcm_driver_init got a null device pointer!\n");
		return;
	}

	c_printf("bcm_driver_init - Initializing network card...\n");

	uint8_t temp8=0;
	uint16_t temp16=0;
	uint32_t temp32=0;
	pci_addr_t tempAddr = bcm_device->address;
	uint8_t byte_offset = 0;

	//STEP1: Enable memory_space and bus_master bits of command register
	//Read command register
	tempAddr.offset = PCI_COMMAND_REG;
	status = _pci_read_long(1, tempAddr, &temp32);
	ERROR_CHECK(status);

	temp32 |= PCI_CMD_BUS_MASTER | PCI_CMD_MEMORY_SPACE;
	//Update command register
	status = _pci_write_long(1, tempAddr, temp32);
	ERROR_CHECK(status);


	//STEP2: Disable interrupts
	//Read Miscellaneous Host Control register(p.325)
	tempAddr.offset = BCM_MISC_HOST_CTL_REG;
	status = _pci_read_long(1, tempAddr, &temp32);
	ERROR_CHECK(status);

	temp32 |= BCM_MISC_HOST_CTL_MASK_PCI_INT | BCM_MISC_HOST_CTL_CLEAR_INTA;
	//Update command register
	status = _pci_write_long(1, tempAddr, temp32);
	ERROR_CHECK(status);


	//STEP3: Save the PCI Cache Line Size register(p.305) and PCI Subsystem Vendor ID registers(p.309)
	uint8_t cacheLineSize = bcm_device->config.cacheLineSize;
	uint16_t subsysVendorId = bcm_device->config.headers.type0.systemVendorId;

	//STEP4: Acquire the NVRAM lock(p.555)
	tempAddr.offset = BCM_NVM_SOFT_ARB_REG;
	status = _pci_read_long(1, tempAddr, &temp32);
	ERROR_CHECK(status);

	do{
		c_printf("DEBUG: Step4 - BCM_NVM_SOFT_ARB_REG = 0x%x\n", temp32);
		temp32 |= BCM_NVM_SOFT_ARB_REQ_SET(1);
		status = _pci_write_long(1, tempAddr, temp32);
		ERROR_CHECK(status);

		status = _pci_read_long(1, tempAddr, &temp32);
		ERROR_CHECK(status);
	}
	while((temp32 & BCM_NVM_SOFT_ARB_WON(1)) == 0);

	c_printf("INFO: Step 4 complete\n");


	//STEP5(a): Set the Enable bit in the Memory Arbiter Mode register p460.
	tempAddr.offset = BCM_MEM_ARB_MODE_REG;
	status = _pci_read_long(1, tempAddr, &temp32);
	ERROR_CHECK(status);

	temp32 |= BCM_MEM_ARB_MODE_EA;
	status = _pci_write_long(1, tempAddr, temp32);
	ERROR_CHECK(status);
	c_printf("INFO: Step 5a complete\n");


	//STEP5(b): Set the Enable_Indirect_Access bit of the Miscellaneous Host Control register (p.325)
	tempAddr.offset = BCM_MISC_HOST_CTL_REG;
	status = _pci_read_long(1, tempAddr, &temp32);
	ERROR_CHECK(status);

	temp32 |= BCM_MISC_HOST_CTL_INDIRECT_ACCESS;
	status = _pci_write_long(1, tempAddr, temp32);
	ERROR_CHECK(status);
	c_printf("INFO: Step 5b-d complete\n");

}


/*

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
