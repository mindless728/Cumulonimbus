#include "vesa_framebuffer.h"
#include "vbe_info.h"

pixel_t* vesa_video_memory; 
uint32_t vesa_x_resolution;
uint32_t vesa_y_resolution;
uint32_t vesa_red_mask;
uint32_t vesa_red_offset;
uint32_t vesa_blue_mask;
uint32_t vesa_blue_offset;
uint32_t vesa_green_mask;
uint32_t vesa_green_offset;

void _vesa_init(void) {
    vbe_mode_info_t* vmi = (vbe_mode_info_t*)VBE_MODE_INFO_LOCATION;
    vesa_x_resolution = vmi->x_res;
    vesa_y_resolution = vmi->y_res; 
    
    //Create the masks, if a mask is of size x that means that x bits are set.
    vesa_red_mask = (1 << (vmi->red_mask_size)) - 1;
    vesa_blue_mask = (1 << (vmi->blue_mask_size)) - 1;
    vesa_green_mask = (1 << (vmi->green_mask_size)) - 1;

    vesa_red_offset = vmi->red_field_position;
    vesa_blue_offset = vmi->blue_field_position;
    vesa_green_offset = vmi->green_field_position;
    vesa_video_memory = (pixel_t*)vmi->physical_base_ptr;
}
