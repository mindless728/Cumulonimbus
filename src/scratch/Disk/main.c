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

//#define ZERO_FIRST_LBA

void dummy(int vector, int code);

int main(void) {
	status_t status;
	uint8_t buf[512] = {0};
	uint8_t * b2 = 0;
	uint32_t sector = 0;
	MBR_t mbr;
	int c,i,j,k;
	
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

	//get the mbr of disk 1
#ifndef ZERO_FIRST_LBA
	ide_pio_lba_read(&ide_devices[1],0,(uint8_t*)&mbr);
	c_printf("Partition[0] on Disk[1]:\n");
	c_printf("Status: 0x%x\n",mbr.partition[0].status);
	c_printf("Partition Type: 0x%x\n",mbr.partition[0].partition_type);
	c_printf("Start of Partition: 0x%x\n",mbr.partition[0].first_LBA);
	c_printf("Size of Partition: 0x%x\n",mbr.partition[0].num_sectors);
	c_printf("MBR Signature: 0x%x\n",mbr.MBR_signature);

	sector = mbr.partition[0].first_LBA;
	c_getchar();
	c = 'a';
	while(1) {
		if((c == 'a') || (c = 'd')) {
			c_clearscreen();
			c_moveto(0,0);
			c_printf("Sector:%x\n",sector);
			ide_pio_lba_read(&ide_devices[1],sector,buf);
			for(i = 0; i < 16; ++i) {
				if(i & 0x8)
					c_printf("%x0: ",2*i);
				else
					c_printf("0%x0: ",2*i);
				for(j = 0; j < 32; ++j) {
					k = 32*i+j;
					if(!(j % 4))
						c_printf(" ");
					if(buf[k] & 0xF0)
						c_printf("%x",buf[k]);
					else
						c_printf("0%x",buf[k]);
				}
				c_printf("\n");
			}
		}
		c = c_getchar();
		if((c == 'a') && sector)
			--sector;
		else if(c == 'd')
			++sector;
		else if((c == 's') && (sector >= 0x100))
			sector -= 0x100;
		else if(c == 'w')
			sector += 0x100;
	}
#endif

#ifdef ZERO_FIRST_LBA
	j = 2*1024*1024+1;
	for(i = 0; i < j; ++i) {
		ide_pio_lba_write(&ide_devices[1],i,buf);
		c_printf_at(0,1,"Clearing Sector: %x\n",i);
	}
	c_printf("Partition Zeroed Out\n");
#endif
		
	while(1) {}
	
	return 0;
}

void dummy(int vector, int code) {
}
