#include "vesa_framebuffer.h"
#include "vbe_info.h"
#include "gs_io.h"
#include "c_io.h"
#include "util.h"
#include "timer_isr.h"
#include "fpu.h"

pixel_t* vesa_video_memory; 
uint32_t vesa_x_resolution;
uint32_t vesa_y_resolution;
uint32_t vesa_red_mask;
uint32_t vesa_red_offset;
uint32_t vesa_blue_mask;
uint32_t vesa_blue_offset;
uint32_t vesa_green_mask;
uint32_t vesa_green_offset;
gs_framebuffer_t framebuffer;

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

/*void _vesa_text_demo(void) {
    //gs_set_draw_mode( GS_DRAW_MODE_XOR  ); 
    char ch = 0;
    int index = 3*1280;

    gs_puts_at(0,0,"Type on the screen! Hit escape to see a cooler demo!");
    while( ch != '\033' ) {
        gs_putc_at(index%1280,FONT_CHAR_HEIGHT*(index/1280), ch = c_getchar() );
        index += FONT_CHAR_WIDTH;
    }
}*/

#define NUM_ITERS 500
void _print_mandelbrot( double parameter ) {
    double zoom = 128.0;
    int xoffset = 0;
    int yoffset = 128;
    pixel_t hues[NUM_ITERS+1];
    hues[NUM_ITERS] = 0.0;
    int i = 0;
    for(; i < NUM_ITERS; ++i ) {
        double num = 1.0 - (double)i / NUM_ITERS;
        double h = pow(num, parameter);
        double location = 360*h / 60;
        while( location > 2.0 ) location -= 2.0;
        double x = location - 1;
        if( x < 0 ) x = -x;
        x = 1 - x;
        int l = (int)location;
        switch((int)(360*h/60)) {
            case 0: 
            hues[i] = CREATE_PIXEL(255,(int)(511*x),0); 
            break;
            case 1: 
            hues[i] = CREATE_PIXEL((int)(255*x),511,0); 
            break;
            case 2: 
            hues[i] = CREATE_PIXEL(0,511,(int)(255*x)); 
            break;
            case 3: 
            hues[i] = CREATE_PIXEL(0,(int)(511*x),255); 
            break;
            case 4: 
            hues[i] = CREATE_PIXEL(255,0,(int)(255*x)); 
            break;
            case 5: 
            hues[i] = CREATE_PIXEL((int)(255*x),0,255); 
            break;
            default: 
            hues[i] = 0;
            break;
        }
    }
    while( 1 ) {
        int r,c;
        for( r = 0; r < 1024; ++r ) {
            if( c_input_queue() ) break;
            for( c = 0; c < 1280; ++c ) {
                int iter = 0;
                double a = 0.0;
                double b = 0.0;
                double aold = 0.0;
                double bold = 0.0;
                double x = (c - 640 + xoffset)/zoom;
                double y = (r - 512 + yoffset)/zoom;
                while( iter < NUM_ITERS && (a*a+b*b) <= 4.0 ) {
                    a = aold*aold - bold*bold + x; 
                    b = 2*aold*bold + y;
                    ++iter;
                    aold = a;
                    bold = b;
                }
                *GET_PIXEL(c,r) = hues[iter];
            }
        }
        char key = c_getchar();
        switch(key) {
            case '+':
                zoom *= 2.0;
                xoffset *= 2.0;
                yoffset *= 2.0;
            break;
            case '-':
                zoom *= 0.5;
                xoffset *= 0.5;  
                yoffset *= 0.5;  
            break;

            case 'w':
                yoffset -= 100.0;
                break;
            case 's':
                yoffset += 100.0;
                break;
            case 'a':
                xoffset -= 100.0;
                break;
            case 'd':
                xoffset += 100.0;
                break;
            case 'r':
                xoffset = 0;
                yoffset = 0;
                zoom = 128.0; 
        }
    }
}

/*void _print_vesa_demo() {
    vbe_mode_info_t* c = (vbe_mode_info_t*)VBE_MODE_INFO_LOCATION;
    uint32_t iter = 0;
    uint32_t x_res = c->x_res;
    uint32_t y_res = c->y_res;
    uint32_t x_res_over2 = c->x_res >> 1;
    uint32_t max_offset = c->x_res;
 
    // Here's an overoptimized demo!
    while( 1 ) {
        gs_puts_at_buf(0,10,"HELLO WORLD", &framebuffer );
        gs_puts_at_buf(0,84, "0123456789", &framebuffer );
        gs_puts_at_buf(0,100,"ABCDEFGHIJKLMNOPQRSTUVWXYZ", &framebuffer );
        gs_puts_at_buf(0,116,"abcdefghijklmnopqrstuvwxyz", &framebuffer );
        gs_puts_at_buf(0,132,"Timer ticks: ", &framebuffer);
        gs_puts_at_buf(156,132,itoa(_get_time()), &framebuffer);

        gs_draw_buf( &framebuffer );
        double_pixel_t* lol = (double_pixel_t*)&framebuffer;

        // draw the first line of the scene
        uint32_t offset = 0;
        do {   
            uint32_t grey_intensity = (((offset+iter) % x_res) << 7) / (x_res_over2);
            
            if( grey_intensity > 128 ){
                grey_intensity = ~grey_intensity;
                grey_intensity &= 0xFF;
            } else if( grey_intensity == 128 ) {
                grey_intensity = 127;
            }
            uint32_t frac = grey_intensity >> 2;
            pixel_t write = CREATE_PIXEL(frac,frac<<1,frac);

            *lol = CREATE_DOUBLE_PIXEL(write,write);
            ++lol;
            offset += 2;
        } while( offset < max_offset );
        // copy the first line to the rest of the scene
        int i,j;
        double_pixel_t* begin = lol;
        for( i = 1; i < y_res; ++i ) {
            lol = (double_pixel_t*)&framebuffer;
            for( j = 0; j < x_res; ++j ) {
                *begin = *lol;
                ++lol; ++begin;
            }
            
        }
        iter += 10;
    }
}*/
