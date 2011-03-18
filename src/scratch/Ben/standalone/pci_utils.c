#include "c_io.h"
#include "support.h"
#include "startup.h"
#include "pci_utils.h"

uint32_t _get_pci_register( uint32_t bus, uint32_t device, 
            uint32_t func, uint32_t reg ) {
        uint32_t pci = 0x80000000 | ((bus&0xFF) << 16) | ((device&0x1F) << 11) | ((func&0x7) << 8) | ((reg & 0x3F) << 2);
        __outl( PCI_CONFIG_ADDR_PORT, pci );
        return __inl( PCI_CONFIG_DATA_PORT );
}

void _scan_all_pci_devices() {
    uint32_t i;
    uint32_t j;
    uint32_t k;
    for( i = 0; i < 256; ++i ) {
        for( j = 0; j < 32; ++j ) {
            for( k = 0; k < 8; ++k ) {
                uint32_t data = _get_pci_register( i, j, k, 0 );
                if( data != 0xFFFFFFFF ){
                    c_printf( "Bus: %d     Device: %d     Function: %d\n",i,j,k );
                    // device exists, extract information
                    c_printf( "Vendor ID:  0x%x\nDevice ID: 0x%x\n", data & 0xFFFF, (data & 0xFFFF0000) >> 16 );
                    c_gets(0,2);
                }
            }
        }
    }
}
