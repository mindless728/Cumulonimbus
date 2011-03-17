#include "c_io.h"
#include "vbe_info.h"
#include "random.h"

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
void _print_vesa_demo() {
    vbe_mode_info_t* c = (vbe_mode_info_t*)VBE_MODE_INFO_LOCATION;
    srandom(0);
    uint32_t iter = 0;
    while( 1 ) {
        uint16_t* lol = (uint16_t*)c->physical_base_ptr;
        uint32_t offset = 0;
        uint32_t max_offset = c->x_res * c->y_res;
        do {
            float grey_int1 = 0.0f;
            grey_int1 = (float)((offset+iter) % (c->x_res)) / (c->x_res >> 1 );
            if( grey_int1 >= 0.99f && grey_int1 <= 1.01f ) {
                grey_int1 = 0.99f;
            } else if( grey_int1 > 1.0f ){
                grey_int1 -= 1.0f;
                grey_int1 = 1.0f - grey_int1; 
            }  
            *lol = ((uint16_t)(grey_int1 * 32)) | ((uint16_t)(grey_int1 * 64)) << 5 | ((uint16_t)(grey_int1 * 32)) << 11;
            ++lol;
            ++offset;
        } while( offset < max_offset );
        iter += 2;
    }
    // static demo
    /*while( 1 ) {
        if( c->bits_per_pixel == 24 ) {
            uint32_t offset = 0;
            uint8_t* lol = (uint8_t*)(c->physical_base_ptr);
            uint32_t thresh1 = (c->x_res*c->y_res)/3;
            uint32_t thresh2 = 2*thresh1;
            uint32_t thresh3 = c->x_res*c->y_res;
            while( offset < thresh3 ) { 
                // BB GG RR in memory, if 24 bits are used then 0xRRGGBB
                //blue
                if( offset < thresh1 ) *lol = (uint8_t)0xFF;
                ++lol;
                //green
                if( offset >= thresh1 && offset < thresh2 ) *lol = (uint8_t)0xFF;
                ++lol;
                //red
                if( offset > thresh2 ) *lol = (uint8_t)0xFF;
                ++lol;
                ++offset;
            }
        } else {
            // 0xAARRGGBB
            uint32_t offset= 0;
            uint32_t* lol = (uint32_t*)(c->physical_base_ptr);
            while( offset < c->x_res*c->y_res ) {
                *lol = 0x00FFFFFF & random();
                ++lol;
                ++offset;
            }
        }
    }*/
}

uint16_t get_pixel(uint32_t x, uint32_t y) {
    return 0;
}

void set_pixel( uint32_t x, uint32_t y, uint32_t color ) {

}
