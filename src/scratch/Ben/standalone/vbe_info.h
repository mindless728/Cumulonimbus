#ifndef VBE_INFO_H
#define VBE_INFO_H
#include "vesa_framebuffer.h"
#include "int_types.h"
extern void _print_vesa_controller_info(void);
extern void _print_vesa_mode_info(void);
extern void _print_vesa_demo(void);

// locations of VBE information in memory
#define VBE_INFO_LOCATION 0x7C00
#define VBE_MODE_INFO_LOCATION 0x7A00

// Converts a short[2] containing offset:segment to a 20 bit physical memory
// address
#define FARPTR_TO_LINEAR(x) ((((x)[0])&0xFFFF)+((((x)[1])&0xFFFF)<<4))

// The structure representing the data layout of the controller information
// obtained by loading 0x4f00 into %ax and issuing the BIOS interrupt.
typedef struct _vbe_controller_info_t {
    int8_t signature[4];              // Should be VESA
    int16_t version;                  // the VESA version, display in hex
    int16_t oem_string[2];             // isa vbe_far_ptr_t
    uint8_t capabilities[4]; 
    int16_t videomodes[2];            // isa vbe_far_ptr_t
    int16_t total_memory;             // # of 64k blocks
} vbe_controller_info_t;

// The structure representing the data layout of the vesa mode information
// obtained by loading 0x4f01 into %ax, the mode into %bx, and issuing the BIOS
// interrupt.
typedef struct _vbe_mode_info_t {
    // mandatory information for all VBE versions
    uint16_t attributes;            // mode attributes
    uint8_t win_a;                  // window A attributes
    uint8_t win_b;                  // window B attributes
    uint16_t granularity;           // window granularity
    uint16_t winsize;               // window size
    uint16_t segment_a;             // window A start segment
    uint16_t segment_b;             // window B start segment
    uint16_t win_func_ptr[2];       // real mode far pointer to window function
    uint16_t bytes_per_scanline;    // bytes per scanline

    // mandatory information for VBE 1.2 and above
    uint16_t x_res;                 // horizontal resolution in pixels or chars
    uint16_t y_res;                 // vertical resolution in pixels or chars
    uint8_t x_char_size;            // character width in pixels
    uint8_t y_char_size;            // character height in pixels
    uint8_t number_of_planes;       // number of memory planes
    uint8_t bits_per_pixel;         // number of bits per pixel
    uint8_t number_of_banks;        // number of banks
    uint8_t bank_size;              // bank size in kb
    uint8_t number_of_image_pages;  // number of image pages    
    uint8_t reserved_1;             // reserved for page function
    
    // direct color fields (required for direct/6 and YUV/7 memory models
    uint8_t red_mask_size;          // size of direct color red mask
    uint8_t red_field_position;     // bit position of lsb of red mask
    uint8_t green_mask_size;        // size of direct color green mask
    uint8_t green_field_position;   // bit position of lsb of green mask
    uint8_t blue_mask_size;         // size of direct color blue mask
    uint8_t blue_field_position;    // bit position of lsb of red mask
    uint8_t reserved_mask_size;     // size of reserved mask in bits
    uint8_t reserved_field_position;// bit position of lsb of reserved mask
    uint8_t direct_color_mode_info; // direct color mode attributes

    // mandatory information for VBE 2.0 and above
    uint32_t physical_base_ptr;
    uint32_t reserved_2;
    uint16_t reserved_3;
    uint8_t reserved_4[203];
} vbe_mode_info_t;
#endif
