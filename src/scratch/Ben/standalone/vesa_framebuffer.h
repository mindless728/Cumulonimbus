#ifndef VESA_FRAMEBUFFER_H_
#define VESA_FRAMEBUFFER_H_
#include "int_types.h"
#include "vbe_info.h"

#define PIXEL_MASK      0xFFFF
#define RED_MASK        0xF800
#define RED_OFFSET      11
#define GREEN_MASK      0x07E0
#define GREEN_OFFSET    5
#define BLUE_MASK       0x001F
#define BLUE_OFFSET     0

#define EXTRACT_RED(p) (((p) & RED_MASK) >> RED_OFFSET)
#define EXTRACT_GREEN(p) (((p) & GREEN_MASK) >> GREEN_OFFSET)
#define EXTRACT_BLUE(p) ((p) & BLUE_MASK)
#define CREATE_PIXEL(r,g,b) (pixel_t)((r) << RED_OFFSET | (g) << GREEN_OFFSET | (b) << BLUE_OFFSET)
#define CREATE_DOUBLE_PIXEL(p1,p2) (((double_pixel_t)(p1)<<16)|(p2))
#define EXTRACT_FIRST_PIXEL(dp) (pixel_t)(*(dp)>>16)
#define SET_FIRST_PIXEL(dp,p) *(dp) = ((0x0000FFFF&*(dp)) | (p) << 16 )
#define EXTRACT_SECOND_PIXEL(dp) (pixel_t)(0xFFFF&*(dp))
#define SET_SECOND_PIXEL(dp,p) *(dp) = ((0xFFFF0000&*(dp)) | (p))
#define GET_PIXEL(x,y) ((vesa_video_memory)+((y)*vesa_x_resolution) + (x))
//#define GET_PIXEL(x,y) (vesa_video_memory->buffer[(x)][(y)])
#define GET_PIXEL_BUF(x,y,buf) ((pixel_t*)(buf)+((y)*vesa_x_resolution) + (x))
//#define GET_PIXEL_BUF(x,y,buf) &((buf)->buffer[(x)][(y)])
#define GET_DOUBLE_PIXEL(x,y) (((double_pixel_t*)vesa_video_memory)+((y)*(vesa_x_resolution >> 1) + (x >> 1)))
//#define GET_DOUBLE_PIXEL(x,y) (vesa_video_memory->dpbuffer[(x)>>2][(y)])

// A pixel datatype
typedef uint16_t pixel_t;
// A double-pixel datatype
typedef uint32_t double_pixel_t;
// A color component
typedef uint8_t color_component_t;

extern uint32_t vesa_x_resolution;
extern uint32_t vesa_y_resolution;

pixel_t* vesa_video_memory;

extern void _vesa_init(void);
extern void _vesa_text_demo(void);
extern void _print_vesa_demo(void);

#endif
