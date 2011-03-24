#ifndef BROADCOM_ETHERNET_H
#define BROADCOM_ETHERNET_H

#include "pci.h"
#include "ethernet.h"

#define BCM5788_PCI_DEVICE	0x1691
#define BCM5788_PCI_VENDOR	0x14e4

#define BCM_T3_MAGIC_NUMBER 0x4B657654

#define BCM_MISC_HOST_CTL_REG		(0x68)
//#define BCM_MISC_HOST_CTL_OFFSET	(0x68)

#define BCM_MISC_HOST_CTL_CLEAR_INTA			(1)
#define BCM_MISC_HOST_CTL_MASK_PCI_INT			(1<<1)
#define BCM_MISC_HOST_CTL_ENDIAN_BYTE_SWAP		(1<<2)
#define BCM_MISC_HOST_CTL_ENDIAN_WORD_SWAP		(1<<3)
#define BCM_MISC_HOST_CTL_STATE_RW_CAP			(1<<4)
#define BCM_MISC_HOST_CTL_CLK_CTL_CAP			(1<<5)
#define BCM_MISC_HOST_CTL_REG_WORD_SWAP			(1<<6)
#define BCM_MISC_HOST_CTL_INDIRECT_ACCESS		(1<<7)
#define BCM_MISC_HOST_CTL_MASK_INT_MODE			(1<<8)
#define BCM_MISC_HOST_CTL_TAGGED_STATUS_MODE	(1<<9)

/*
 *	Page 460 - Memory Arbiter Registers
 */
#define BCM_MEM_ARB_MODE_REG		(0x4000)
#define BCM_MEM_ARB_STATUS_REG		(0x4004)
#define BCM_MEM_ARB_TRAP_LOW_REG	(0x4008)
#define BCM_MEM_ARB_TRAP_HIGH_REG	(0x400c)

/*#define BCM_MEM_ARB_MODE_OFFSET			PCI_REG_OFFSET(0x4000)
#define BCM_MEM_ARB_STATUS_OFFSET		PCI_REG_OFFSET(0x4004)
#define BCM_MEM_ARB_TRAP_LOW_OFFSET		PCI_REG_OFFSET(0x4008)
#define BCM_MEM_ARB_TRAP_HIGH_OFFSET	PCI_REG_OFFSET(0x400c)*/

//Bit fields
#define BCM_MEM_ARB_MODE_RESET		(1)
#define BCM_MEM_ARB_MODE_EA			(1<<1)


//Page 549 - Non volatile Memory Interface Registers
#define BCM_NVM_CMD_REG				0x7000
#define BCM_NVM_WRITE_REG			0x7008
#define BCM_NVM_ADDR_REG			0x700c
#define BCM_NVM_READ_REG			0x7010
#define BCM_NVM_CFG1_REG			0x7014
#define BCM_NVM_CFG2_REG			0x7018
#define BCM_NVM_CFG3_REG			0x701c
#define BCM_NVM_SOFT_ARB_REG		0x7020
#define BCM_NVM_ACCESS_REG			0x7024
#define BCM_NVM_WRITE1_REG			0x7028
#define BCM_NVM_ARB_WATCHD_TMR_REG	0x702c

/*#define BCM_NVM_CMD_OFFSET				(0x7000)
#define BCM_NVM_WRITE_OFFSET			(0x7008)
#define BCM_NVM_ADDR_OFFSET				(0x700c)
#define BCM_NVM_READ_OFFSET				(0x7010)
#define BCM_NVM_CFG1_OFFSET				(0x7014)
#define BCM_NVM_CFG2_OFFSET				(0x7018)
#define BCM_NVM_CFG3_OFFSET				(0x701c)
#define BCM_NVM_SOFT_ARB_OFFSET			(0x7020)
#define BCM_NVM_ACCESS_OFFSET			(0x7024)
#define BCM_NVM_WRITE1_OFFSET			(0x7028)
#define BCM_NVM_ARB_WATCHD_TMR_OFFSET	(0x702c)*/

//Bit fields
#define BCM_NVM_SOFT_ARB_REQ_SET(x)				(1<<x)
#define BCM_NVM_SOFT_ARB_REQ_CLR(x)				(1<<(4+x))
#define BCM_NVM_SOFT_ARB_WON(x)					(1<<(8+x))
#define BCM_NVM_SOFT_ARB_REQ_ACTIVE(x)			(1<<(12+x))


void bcm_driver_init(pci_device_list_t* list);

//uint8_t	bcm_read_active_nvram_locks(pci_device_t* device);
//status_t bcm_acquire_nvram_lock(pci_device_t* device);


#endif	//BROADCOM_ETHERNET_H
