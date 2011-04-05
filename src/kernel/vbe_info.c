#include "c_io.h"
#include "vbe_info.h"
#include "vesa_framebuffer.h"
#include "gs_io.h"

void _print_vesa_controller_info() {
        vbe_controller_info_t* c = (vbe_controller_info_t*)VBE_INFO_LOCATION;
        int i = 0;
        c_printf( "vci_t size:%d\n", sizeof(vbe_controller_info_t));
        c_printf( "vmi_t size:%d\n", sizeof(vbe_mode_info_t));
        c_printf( "\nsignature: 0x%x\n", *((uint32_t*)c));
        c_printf( "vbeci[%d] = 0x%x:\n",i,c);
        c_printf( "version: 0x%x\n", c->version );
        c_printf( "oem string: %s\n", (char*)FARPTR_TO_LINEAR(c->oem_string));
        c_printf( "capabilities: 0x%x\n", (uint32_t*)c->capabilities );
        c_printf( "video modes addr: 0x%x\n", FARPTR_TO_LINEAR(c->videomodes) );
        uint16_t* modes = (uint16_t*)FARPTR_TO_LINEAR(c->videomodes);
        while( *modes != 0xFFFF ) {
                c_printf( "0x%x ", *modes );
                ++modes;
        }
        c_printf( "\n64k blocks of memory: %d\n", c->total_memory );
}

void _print_vesa_mode_info() {
    vbe_mode_info_t* c = (vbe_mode_info_t*)VBE_MODE_INFO_LOCATION;
    c_printf("%dx%d, %dbpp\nPBP: 0x%x\n", c->x_res, c->y_res, c->bits_per_pixel, c->physical_base_ptr );
    c_printf("RED MASK: 0x%x/0x%x\n", c->red_mask_size, c->red_field_position ); 
    c_printf("GREEN MASK: 0x%x/0x%x\n", c->green_mask_size, c->green_field_position ); 
    c_printf("BLUE MASK: 0x%x/0x%x\n", c->blue_mask_size, c->blue_field_position ); 

    c_printf("RSVD MASK: 0x%x/0x%x\n", c->reserved_mask_size, c->reserved_field_position ); 
}

