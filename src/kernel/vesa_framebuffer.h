/**
 * File: vesa_framebuffer.h
 * Author: Benjamin David Mayes <bdm8233@rit.edu>
 * Description: Definitions of functions, global variables, and macros that can
 * be used to get information about, use, and abuse the VESA framebuffer.
 */

#ifndef VESA_FRAMEBUFFER_H_
#define VESA_FRAMEBUFFER_H_
#include "types.h"
#include "vbe_info.h"

#define PIXEL_MASK      0xFFFF
#define RED_MASK        0xF800
#define RED_OFFSET      11
#define GREEN_MASK      0x07E0
#define GREEN_OFFSET    5
#define BLUE_MASK       0x001F
#define BLUE_OFFSET     0

// some macros for use with the framebuffer (some of these are specific to 16bpp)

// color extraction macros
#define EXTRACT_RED(p) (((p) & RED_MASK) >> RED_OFFSET)
#define EXTRACT_GREEN(p) (((p) & GREEN_MASK) >> GREEN_OFFSET)
#define EXTRACT_BLUE(p) ((p) & BLUE_MASK)

// pixel creation macros
#define CREATE_PIXEL(r,g,b) (pixel_t)((r) << RED_OFFSET | (g) << GREEN_OFFSET | (b) << BLUE_OFFSET)
#define CREATE_DOUBLE_PIXEL(p1,p2) (((double_pixel_t)(p1)<<16)|(p2))

// double pixel manipulation macros
#define EXTRACT_FIRST_PIXEL(dp) (pixel_t)(*(dp)>>16)
#define SET_FIRST_PIXEL(dp,p) *(dp) = ((0x0000FFFF&*(dp)) | (p) << 16 )
#define EXTRACT_SECOND_PIXEL(dp) (pixel_t)(0xFFFF&*(dp))
#define SET_SECOND_PIXEL(dp,p) *(dp) = ((0xFFFF0000&*(dp)) | (p))

// gets a pixel from the linear framebuffer
#define GET_PIXEL(x,y) ((vesa_video_memory)+((y)*vesa_x_resolution) + (x))
#define GET_DOUBLE_PIXEL(x,y) (((double_pixel_t*)vesa_video_memory)+((y)*(vesa_x_resolution >> 1) + (x >> 1)))

// gets a pixel from a linear framebuffer buffer elsewhere in memory
#define GET_PIXEL_BUF(x,y,buf) ((pixel_t*)(buf)+((y)*vesa_x_resolution) + (x))
#define GET_DOUBLE_PIXEL_BUF(x,y,buf) ((double_pixel_t*)(buf)+((y)*(vesa_x_resolution>>1)) + (x>>1))

// A pixel datatype
typedef uint16_t pixel_t;
// A double-pixel datatype
typedef uint32_t double_pixel_t;

// location of the linear framebuffer
extern pixel_t* vesa_video_memory; 

//resolution of the linear framebuffer
extern uint32_t vesa_x_resolution;
extern uint32_t vesa_y_resolution;

// red component information
extern uint32_t vesa_red_mask;
extern uint32_t vesa_red_offset;

// blue component information
extern uint32_t vesa_blue_mask;
extern uint32_t vesa_blue_offset;

// green component information
extern uint32_t vesa_green_mask;
extern uint32_t vesa_green_offset;

// vesa initialization function
extern void _vesa_init(void);
#endif
