/**
 * File: vesa_framebuffer.c
 * Author: Benjamin David Mayes <bdm8233@rit.edu>
 * Description: Initializes the vesa framebuffer and some globals with info
 * about the selected vesa mode.
 */
#include "vesa_framebuffer.h"
#include "vbe_info.h"

// global declarations
pixel_t* vesa_video_memory; 
uint32_t vesa_x_resolution;
uint32_t vesa_y_resolution;
uint32_t vesa_red_mask;
uint32_t vesa_red_offset;
uint32_t vesa_blue_mask;
uint32_t vesa_blue_offset;
uint32_t vesa_green_mask;
uint32_t vesa_green_offset;

/**
 * Initializes the vesa framebuffer and sets global variables with information
 * about the current video mode.
 */
void _vesa_init(void) {
    // TODO: incorporate data read in here to drawing routines if selection
    // of the best VESA mode is ever used.

    // obtain the hardcoded address of the vbe_mode_info structure
    vbe_mode_info_t* vmi = (vbe_mode_info_t*)VBE_MODE_INFO_LOCATION;

    // determine the resolution
    vesa_x_resolution = vmi->x_res;
    vesa_y_resolution = vmi->y_res; 
    
    // create the masks, if a mask is of size x that means that x bits are set.
    vesa_red_mask = (1 << (vmi->red_mask_size)) - 1;
    vesa_blue_mask = (1 << (vmi->blue_mask_size)) - 1;
    vesa_green_mask = (1 << (vmi->green_mask_size)) - 1;

    // offsets of the color fields
    vesa_red_offset = vmi->red_field_position;
    vesa_blue_offset = vmi->blue_field_position;
    vesa_green_offset = vmi->green_field_position;

    // the address of the linear framebuffer
    vesa_video_memory = (pixel_t*)vmi->physical_base_ptr;
}
