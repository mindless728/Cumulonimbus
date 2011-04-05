#ifndef INTEL_8255X_ETHERNET_H
#define INTEL_8255X_ETHERNET_H

#include "pci.h"
#include "pci_defines.h"
#include "types.h"

typedef struct intel_action_hdr{
	uint16_t status;
	uint16_t command;

	uint32_t link_addr;
} intel_action_hdr_t;


typedef struct intel_rx_buffer{
	intel_action_hdr_t header;

	uint16_t actual_count;
	uint16_t size;
} intel_rx_buffer_t;


typedef struct intel_tx_buffer{
	intel_action_hdr_t header;

	uint32_t tbd_addr;
} intel_tx_buffer_t;


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
	int irq_vector;
	boolean_t wrong_irq;

	boolean_t cu_transition;
	boolean_t ru_transition;

	intel_action_hdr_t* command_base;
	intel_rx_buffer_t* rx_buffer_base;
	intel_tx_buffer_t* tx_buffer_base;
} intel_ethernet_t;

status_t i8255x_driver_init(pci_device_list_t* list);

status_t i8255x_driver_setup_irq(void);

status_t i8255x_driver_transmit(void* frame, uint16_t size, boolean_t blocking);

status_t i8225x_driver_rx_wait(void);

status_t i8225x_driver_tx_wait(void);

status_t i8255x_driver_recieve(void*);

void i8255x_driver_isr(int vector, int code);

void i8255x_write_ru_cmd(uint8_t cmd, uint32_t generel_ptr);

void i8255x_write_cu_cmd(uint8_t cmd, uint32_t generel_ptr);



#define INTEL8255x_DEVICE_ID 0x1229

/* SCB Status Word Bit masks */
//! Recieve Unit Satus
#define INTEL_ETH_SCB_STATUS_RUS_SHIFT		(2)
	#define INTEL_ETH_SCB_STATUS_RUS		(0xf<<2)
	#define SCB_STATUS_RUS_IDLE				(0x0)
	#define SCB_STATUS_RUS_SUSPENDED		(0x1)
	#define SCB_STATUS_RUS_NO_RESOURCES		(0x2)
	#define SCB_STATUS_RUS_READY			(0x4)

//! Command Unit Status
#define INTEL_ETH_SCB_STATUS_CUS_SHIFT		(6)
	#define INTEL_ETH_SCB_STATUS_CUS_MASK	(0x3<<6)
	#define SCB_STATUS_CUS_IDLE				(0x0)
	#define SCB_STATUS_CUS_SUSPENDED		(0x1)
	#define SCB_STATUS_CUS_LPQ_ACTIVE		(0x2)
	#define SCB_STATUS_CUS_HQP_ACTIVE		(0x3)
#define INTEL_ETH_SCB_STATUS_ACK_MASK	(0xff<<8)
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


/* SCB Command Word Bit masks */
//! Recieve unit command
#define INTEL_ETH_SCB_CMD_RUC_SHIFT			(0)
	#define INTEL_ETH_SCB_CMD_RUC_MASK		(0x7<<0)
	#define SCB_CMD_RUC_NOP					(0x0)
	#define SCB_CMD_RUC_START				(0x1)
	#define SCB_CMD_RUC_RESUME				(0x2)
	#define SCB_CMD_RUC_RX_DMA_REDIRECT		(0x3)
	#define SCB_CMD_RUC_ABORT				(0x4)
	#define SCB_CMD_RUC_LD_HDS				(0x5)
	#define SCB_CMD_RUC_LD_RU_BASE			(0x6)
//! Command unit command
#define INTEL_ETH_SCB_CMD_CUC_SHIFT			(4)
	#define INTEL_ETH_SCB_CMD_CUC_MASK		(0xf<<4)
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
#define INTEL_ETH_SCB_CMD_INTS_SHIFT		(10)
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

//! Control Unit Action Commands
#define CU_ACTION_NOP		(0x0)
#define CU_ACTION_IA_SETUP	(0x1)
#define CU_ACTION_CONFIGURE	(0x2)
#define CU_ACTION_MC_SETUP	(0x3)
#define CU_ACTION_TRANSMIT	(0x4)
#define CU_ACTION_LOAD_uC	(0x5)
#define CU_ACTION_DUMP		(0x6)
#define CU_ACTION_DIAGNOSE	(0x7)

// Command Header Bit Masks
#define ACTION_HDR_STATUS_OK	(1<<13)
#define ACTION_HDR_STATUS_C		(1<<15)

#define ACTION_HDR_CMD_ID_MASK	(0x3)
#define ACTION_HDR_CMD_I		(1<<13)
#define ACTION_HDR_CMD_S		(1<<14)
#define ACTION_HDR_CMD_EL		(1<<15)



#endif	//INTEL_8255X_ETHERNET_H


