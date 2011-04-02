#ifndef INTEL_8255X_ETHERNET_H
#define INTEL_8255X_ETHERNET_H

#include "pci.h"
#include "pci_defines.h"
#include "types.h"


typedef struct intel_csr{
	uint16_t status;
	uint16_t command;

	uint32_t general_ptr;
	uint32_t port;

	uint16_t reserved0;
	uint16_t eeprom_ctl;

	uint32_t mdi_ctl;
	uint32_t rx_dma_byte_count;

	uint8_t reserved1;
	uint16_t flow_ctl;
	uint8_t pmdr;

	uint8_t general_ctl;
	uint8_t general_status;
	uint16_t reserved2;

	uint32_t reserved3;
	uint32_t func_event;
	uint32_t func_event_mask;
	uint32_t func_present_state;
	uint32_t force_event;

} intel_csr_t;

typedef struct intel_ethernet{
	pci_device_t* pci;
	intel_csr_t* csr_bar;
	uint32_t ioport;
	void* flash_bar;

} intel_ethernet_t;

void i8255x_driver_init(pci_device_list_t* list);

#define INTEL8255x_DEVICE_ID 0x1229

/* SCB Status Word Bit masks */
//! Recieve Unit Satus
#define INTEL_ETH_SCB_STATUS_RUS	(0xf<<2)
	#define SCB_STATUS_RUS_IDLE				(0x0)
	#define SCB_STATUS_RUS_SUSPENDED		(0x1)
	#define SCB_STATUS_RUS_NO_RESOURCES		(0x2)
	#define SCB_STATUS_RUS_READY			(0x4)

//! Command Unit Status
#define INTEL_ETH_SCB_STATUS_CUS	(0x3<<2)
	#define SCB_STATUS_CUS_IDLE				(0x0)
	#define SCB_STATUS_CUS_SUSPENDED		(0x1)
	#define SCB_STATUS_CUS_LPQ_ACTIVE		(0x2)
	#define SCB_STATUS_CUS_HQP_ACTIVE		(0x3)
//! Flow Control Pause
#define INTEL_ETH_SCB_STATUS_FCP	(1<<8)
//! Software Interrupt
#define INTEL_ETH_SCB_STATUS_SWI	(1<<10)
//! MDI Read/Write Cycle complete
#define INTEL_ETH_SCB_STATUS_MDI	(1<<11)
//! Recieve unit not ready
#define INTEL_ETH_SCB_STATUS_RNR	(1<<12)
//! Control not active
#define INTEL_ETH_SCB_STATUS_CNA	(1<<13)
//! Recieve unit finished recieving frame
#define INTEL_ETH_SCB_STATUS_FR		(1<<14)
//! Control unit finished executing command
#define INTEL_ETH_SCB_STATUS_CX		(1<<15)


//! Recieve unit command
#define INTEL_ETH_SCB_CMD_RUC			(0x7)
	#define SCB_CMD_RUC_NOP					(0x0)
	#define SCB_CMD_RUC_START				(0x1)
	#define SCB_CMD_RUC_RESUME				(0x2)
	#define SCB_CMD_RUC_RX_DMA_REDIRECT		(0x3)
	#define SCB_CMD_RUC_ABORT				(0x4)
	#define SCB_CMD_RUC_LD_HDS				(0x5)
	#define SCB_CMD_RUC_LD_RU_BASE			(0x6)
//! Command unit command
#define INTEL_ETH_SCB_CMD_CUC			(0xf<<3)
	#define SCB_CMD_CUC_NOP					(0x0)
	#define SCB_CMD_CUC_START				(0x1)
	#define SCB_CMD_CUC_RESUME				(0x2)
	#define SCB_CMD_CUC_LD_COUNTERS_ADDR	(0x4)
	#define SCB_CMD_CUC_DUMP_COUNTERS		(0x5)
	#define SCB_CMD_CUC_LD_CU_BASE			(0x6)
	#define SCB_CMD_CUC_DUMP_RST_COUNTERS	(0x7)
	#define SCB_CMD_CUC_STATIC_RESUME		(0xA)
#define INTEL_ETH_SCB_CMD_INTA_MASK		(1<<8)
#define INTEL_ETH_SCB_CMD_TRIGGER_SI	(1<<9)
#define INTEL_ETH_SCB_CMD_INTS_MASK		(0x3f<<10)
	#define INTEL_ETH_SCB_CMD_FCP_MASK		(1<<10)
	#define INTEL_ETH_SCB_CMD_ER_MASK		(1<<11)
	#define INTEL_ETH_SCB_CMD_RNR_MASK		(1<<12)
	#define INTEL_ETH_SCB_CMD_CNA_MASK		(1<<13)
	#define INTEL_ETH_SCB_CMD_FR_MASK		(1<<14)
	#define INTEL_ETH_SCB_CMD_CX_MASK		(1<<15)


#define INTEL_ETH_SCB_GNRL_STATUS_LINK		(1)
#define INTEL_ETH_SCB_GNRL_STATUS_SPEED		(1<<1)
#define INTEL_ETH_SCB_GNRL_STATUS_DX_MODE	(1<<2)

#endif	//INTEL_8255X_ETHERNET_H


