#ifndef BROADCOM_ETHERNET_H
#define BROADCOM_ETHERNET_H

#include "pci.h"
#include "types.h"
#include "ethernet.h"

#define BCM5788_PCI_DEVICE	0x1691
#define BCM5788_PCI_VENDOR	0x14e4

#define BCM_T3_MAGIC_NUMBER 0x4B657654

//Standard Memory Map
enum BCM_STD_Region{
	Std_Raw = 0,
	Std_Pci,
	Std_Mailboxes,
	Std_Regs,
	Std_Mem
};

#define BCM_STD_PCI_CONFIG_OFFSET		(0x0000)
#define BCM_STD_HIGH_PRIO_MAIL_OFFSET	(0x01ff)
#define BCM_STD_REGISTER_OFFSET			(0x03ff)
#define BCM_STD_MEM_WINDOW_OFFSET		(0x7fff)
#define BCM_STD_END_OFFSET				(0xffff)


#define BCM_SUBSYS_VENDOR_ID_REG		(0x2c)
#define BCM_PCI_X_CMD_REG				(0x42)

#define BCM_PCI_X_CMD_RELAXED_ORDER_EA	(1)

/*
 *	Page 184 - PCI Config space device specific registers
 *
 */
#define BCM_MISC_HOST_CTL_REG			(0x68)
#define BCM_DMA_RW_CTL_REG				(0x6c)
#define BCM_PCI_STATE_REG				(0x70)
#define BCM_CLOCK_CTL_REG				(0x74)
#define BCM_REG_BAR_REG					(0x78)
#define BCM_MEM_WINDOW_BAR_REG			(0x7c)
#define BCM_REG_DATA_RW_REG				(0x80)
#define BCM_MEM_WINDOW_DATA_RW_REG		(0x84)
#define BCM_MODE_CTL_SHADOW_REG			(0x88)
#define BCM_MISC_CFG_SHADOW_REG			(0x8c)
#define BCM_MISC_LOCAL_CTL_SHADOW_REG	(0x90)
//Bytes 0x94 to 0x97 are reserved
#define BCM_UNDI_RX_BD_STD_RING			(0x98)
#define BCM_UNDI_RX_RETURN_RING			(0xA0)
#define BCM_UNDI_TX_BD_RING				(0xA8)

//Bit fields
#define BCM_MISC_HOST_CTL_CLEAR_INTA			(1)
#define BCM_MISC_HOST_CTL_MASK_PCI_INT			(1<<1)
#define BCM_MISC_HOST_CTL_ENDIAN_BYTE_SWAP		(1<<2)
#define BCM_MISC_HOST_CTL_ENDIAN_WORD_SWAP		(1<<3)
#define BCM_MISC_HOST_CTL_STATE_RW_EA			(1<<4)
#define BCM_MISC_HOST_CTL_CLK_CTL_EA			(1<<5)
#define BCM_MISC_HOST_CTL_REG_WORD_SWAP			(1<<6)
#define BCM_MISC_HOST_CTL_INDIRECT_ACCESS		(1<<7)
#define BCM_MISC_HOST_CTL_MASK_INT_MODE			(1<<8)
// RESERVED ON BCM5788 #define BCM_MISC_HOST_CTL_TAGGED_STATUS_MODE	(1<<9)

#define BCM_DMA_RW_CTL_REG_SUGGESTED			0x763f0000
#define BCM_DMA_RW_CTL_R_WATERMARK				(0x7<<16)
#define BCM_DMA_RW_CTL_W_WATERMARK				(0x7<<19)
#define BCM_DMA_RW_CTL_DFLT_PCI_W_CMD			(0x3<<28)
#define BCM_DMA_RW_CTL_DFLT_PCI_R_CMD			(0xf<<24)




/*
 *	Page 374 - Ethernet MAC Memory Control Registers
 */
#define BCM_ETH_MAC_MODE_REG			(0x400)
#define BCM_ETH_MAC_STATUS_REG			(0x404)
#define BCM_ETH_MAC_EVENT_EA_REG		(0x408)
#define BCM_ETH_LED_CTL_REG				(0x40c)
#define BCM_ETH_MAC_ADDR_HIGH_1_REG		(0x410)
#define BCM_ETH_MAC_ADDR_LOW_1_REG		(0x414)
#define BCM_ETH_MAC_ADDR_HIGH_2_REG		(0x418)
#define BCM_ETH_MAC_ADDR_LOW_2_REG		(0x41c)
#define BCM_ETH_MAC_ADDR_HIGH3__REG		(0x420)
#define BCM_ETH_MAC_ADDR_LOW_3_REG		(0x424)
#define BCM_ETH_MAC_ADDR_HIGH_4_REG		(0x428)
#define BCM_ETH_MAC_ADDR_LOW_4_REG		(0x42c)
#define BCM_ETH_MAC_WOL_PATTERN_PTR_REG	(0x430)
#define BCM_ETH_MAC_WOL_PATTERN_CFG_REG	(0x434)
#define BCM_ETH_MAC_TX_RAND_BACKOFF_REG	(0x438)
#define BCM_ETH_MAC_RX_MTU_SIZE_REG		(0x43c)
#define BCM_ETH_MAC_GBIT_PCS_TEST_REG	(0x440)
#define BCM_ETH_MAC_TX_AUTO_NEGO_REG	(0x444)
#define BCM_ETH_MAC_RX_AUTO_NEGO_REG	(0x448)
#define BCM_ETH_MAC_MI_COMM_REG			(0x44c)
#define BCM_ETH_MAC_MI_STATUS_REG		(0x450)
#define BCM_ETH_MAC_MI_MODE_REG			(0x454)
#define BCM_ETH_MAC_AUTO_POLL_STATUS_REG	(0x458)
#define BCM_ETH_MAC_TX_MODE_REG			(0x45c)
#define BCM_ETH_MAC_TX_STATUS_REG		(0x460)
#define BCM_ETH_MAC_TX_LEN_REG			(0x464)
#define BCM_ETH_MAC_RX_MODE_REG			(0x468)
#define BCM_ETH_MAC_RX_STATUS_REG		(0x46c)
#define BCM_ETH_MAC_HASH_0_REG			(0x470)
#define BCM_ETH_MAC_HASH_1_REG			(0x474)
#define BCM_ETH_MAC_HASH_2_REG			(0x478)
#define BCM_ETH_MAC_HASH_3_REG			(0x47c)
//Registers 0x598 - 0x624 are reserved
#define BCM_ETH_MAC_RX_STATS_MEM_REG	(0x800)
//Registers 0x868 - 0x87f are reserved
//#define BCM_ETH_MAC_RX_STATS_MEM_REG	(0x880)
//Registers 0x8f0 - 0xbff are reserved


//Bit fields
#define BCM_ETH_LED_CTL_OVERR_LINK_LED		(1)
#define BCM_ETH_LED_CTL_1000Mbps_LED		(1<<1)
#define BCM_ETH_LED_CTL_100Mbps_LED			(1<<2)
#define BCM_ETH_LED_CTL_10Mbps_LED			(1<<3)
#define BCM_ETH_LED_CTL_OVERR_TRAFFIC_LED	(1<<4)
#define BCM_ETH_LED_CTL_BLINK_TRAFFIC_LED	(1<<5)
#define BCM_ETH_LED_CTL_TRAFFIC_LED			(1<<6)
#define BCM_ETH_LED_CTL_1000Mbps_STATUS		(1<<7)
#define BCM_ETH_LED_CTL_100Mbps_STATUS		(1<<8)
#define BCM_ETH_LED_CTL_10Mbps_STATUS		(1<<9)
#define BCM_ETH_LED_CTL_TRAFFIC_STATUS		(1<<10)
#define BCM_ETH_LED_CTL_LED_MODE			(0x3<<11)


/*
 *	Page 460 - Memory Arbiter Registers
 */
#define BCM_MEM_ARB_MODE_REG		(0x4000)
#define BCM_MEM_ARB_STATUS_REG		(0x4004)
#define BCM_MEM_ARB_TRAP_LOW_REG	(0x4008)
#define BCM_MEM_ARB_TRAP_HIGH_REG	(0x400c)


//Bit fields
#define BCM_MEM_ARB_MODE_RESET		(1)
#define BCM_MEM_ARB_MODE_EA			(1<<1)


#define BCM_MODE_CTL_REG			(0x6800)
#define BCM_MISC_CFG_REG			(0x6804)
#define BCM_MISC_LOCAL_CTL_REG		(0x6808)

//Bit fields
//NFD = non frame data
//DA = disable
//EA = enable
#define BCM_MODE_CTL_BYTE_SWAP_NFD			(1<<1)
#define BCM_MODE_CTL_WORD_SWAP_NFD			(1<<2)
//Bit 3 - reserved
#define BCM_MODE_CTL_BYTE_SWAP_DATA			(1<<4)
#define BCM_MODE_CTL_WORD_SWAP_DATA			(1<<5)
#define BCM_MODE_CTL_ALLOW_BAD_FRAMES		(1<<11)
//Bit 12 - reserved
#define BCM_MODE_CTL_NO_INT_ON_TXS			(1<<13)
#define BCM_MODE_CTL_NO_INT_ON_RXS			(1<<14)
#define BCM_MODE_CTL_FORCE_32BIT_PCI		(1<<15)
#define BCM_MODE_CTL_HOST_STACK_RDY			(1<<16)
#define BCM_MODE_CTL_USE_HOST_TX_BD_RINGS	(1<<17)
//Bits 18 - 19 reserved
#define BCM_MODE_CTL_TX_PSEUDO_HDR_DA		(1<<20)
#define BCM_MODE_CTL_NVRAM_WRITE_EA			(1<<21)
//Bit 22 - reserved
#define BCM_MODE_CTL_RX_PSEUDO_HDR_DA		(1<<23)
//Bit 24 - reserved
#define BCM_MODE_CTL_RISC_RX_INT_EA			(1<<25)
#define BCM_MODE_CTL_RISC_MAC_INT_EA		(1<<26)
#define BCM_MODE_CTL_RISC_DMA_INT_EA		(1<<27)
#define BCM_MODE_CTL_RISC_FLOW_INT_EA		(1<<28)
//Bits 29 - 31 reserved



#define	BCM_MISC_CFG_CORE_CLK_RESET			(1)
//Bits 8 - 13 are reserved
#define BCM_MISC_CFG_WIRE_SPEED_TMR_DA		(1<<28)
#define BCM_MISC_CFG_WIRE_SPEED_DETECT_EA	(1<<28)
#define BCM_MISC_CFG_ALT_CLOCK_EA			(1<<31)


//Page 549 - Non volatile Memory Interface Registers
#define BCM_NVM_CMD_REG				(0x7000)
#define BCM_NVM_WRITE_REG			(0x7008)
#define BCM_NVM_ADDR_REG			(0x700c)
#define BCM_NVM_READ_REG			(0x7010)
#define BCM_NVM_CFG1_REG			(0x7014)
#define BCM_NVM_CFG2_REG			(0x7018)
#define BCM_NVM_CFG3_REG			(0x701c)
#define BCM_NVM_SOFT_ARB_REG		(0x7020)
#define BCM_NVM_ACCESS_REG			(0x7024)
#define BCM_NVM_WRITE1_REG			(0x7028)
#define BCM_NVM_ARB_WATCHD_TMR_REG	(0x702c)

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


/*
 *
 *	Memory Map Address Pool
 *
 */

#define	BCM_STD_SOFT_GENCOMM_ADDR	0x00000B50


#define BCM_T3_MAGIC_NUMBER		0x4B657654


typedef struct bcm_ethernet{
	pci_device_t* pci_device;
	uint8_t bcm_cache_line;
	void* base_address;
} bcm_ethernet_t;

void bcm_driver_init(pci_device_list_t* list);

status_t bcm_indirect_mem_read(bcm_ethernet_t* dev, uint32_t mem_addr, uint32_t* data);

status_t bcm_indirect_mem_write(bcm_ethernet_t* dev, uint32_t mem_addr, uint32_t data);

status_t bcm_indirect_reg_read(bcm_ethernet_t* dev, uint32_t reg_addr, uint32_t* data);

status_t bcm_indirect_reg_write(bcm_ethernet_t* dev, uint32_t reg_addr, uint32_t data);

status_t bcm_indirect_reg_set_bits(bcm_ethernet_t* dev, uint32_t reg, uint32_t mask, boolean_t state);

inline void* bcm_std_get_ptr(bcm_ethernet_t* dev, uint8_t type, uint32_t offset);

//uint8_t	bcm_read_active_nvram_locks(pci_device_t* device);
//status_t bcm_acquire_nvram_lock(pci_device_t* device);


#endif	//BROADCOM_ETHERNET_H
