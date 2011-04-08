#include "gs_io.h"
#include "fpu.h"
#include "vesa_demo.h"
static gs_framebuffer_t framebuffer;

void _vesa_text_demo(void) {
    //gs_set_draw_mode( GS_DRAW_MODE_XOR  ); 
    char ch = 0;
    int index = 3*1280;

    gs_puts_at(0,0,"Type on the screen! Hit escape to see a cooler demo!");
    while( ch != '\033' ) {
        gs_putc_at(index%1280,FONT_CHAR_HEIGHT*(index/1280), ch = c_getchar() );
        index += FONT_CHAR_WIDTH;
    }
}

void _print_hue_test() {
    int x = 0;
    int y = 0;
    for( y = 0; y < 1024; ++y ) {
        double hue = (double)y/1024;
        double z = 6*hue;
        int h = z;
        while( z > 2.0 ) z -= 2.0;
        z -= 1.0;
        if( z < 0.0 ) z = -z;
        z = 1 - z;
        pixel_t color = 0;
        switch(h) {
            case 0: 
            color = CREATE_PIXEL(31,(int)(63*z),0); 
            break;
            case 1: 
            color = CREATE_PIXEL((int)(31*z),63,0); 
            break;
            case 2: 
            color = CREATE_PIXEL(0,63,(int)(31*z)); 
            break;
            case 3: 
            color = CREATE_PIXEL(0,(int)(63*z),31); 
            break;
            case 4: 
            color = CREATE_PIXEL((int)(31*z),0,31); 
            break;
            case 5: 
            color = CREATE_PIXEL(31,0,(int)(31*z)); 
            break;
            default: 
            break;
        }
        for( x = 0; x < 1280; ++x ) {
            *GET_PIXEL(x,y) = color;
        }
    }
    c_getchar();
}

#define NUM_ITERS 2000
void _print_mandelbrot( double parameter ) {
    static double zoom = 128.0;
    static int xoffset = 0;
    static int yoffset = 128;
    static int cycle = 0;
    static pixel_t hues[NUM_ITERS+1];
    hues[NUM_ITERS] = 0.0;
    int i = 0;
    for(; i < NUM_ITERS; ++i ) {
        double num = (double)i / NUM_ITERS;
        double x = pow(num, parameter);
        x *= 6;
        int s = (int)x;
        while( x > 2.0 ) x -= 2.0;
        x -= 1.0;
        if( x < 0.0 ) x = -x;
        x = 1 - x;
        if( x > 1.0 ) s = 6;//x = 1.0; 
        if( x < 0.0 ) s = 6;//x = 0.0; 
        switch((s+cycle) % 6) {
            case 0: 
            hues[i] = CREATE_PIXEL(31,(int)(63*x),0); 
            break;
            case 1: 
            hues[i] = CREATE_PIXEL((int)(31*x),63,0); 
            break;
            case 2: 
            hues[i] = CREATE_PIXEL(0,63,(int)(31*x)); 
            break;
            case 3: 
            hues[i] = CREATE_PIXEL(0,(int)(63*x),31); 
            break;
            case 4: 
            hues[i] = CREATE_PIXEL((int)(31*x),0,31); 
            break;
            case 5: 
            hues[i] = CREATE_PIXEL(31,0,(int)(31*x)); 
            break;
            default: 
            hues[i] = 0xFFFF;
            break;
        }
        *GET_PIXEL(i,0) = hues[i]; 
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
                break;
            case '[':
                _print_mandelbrot( parameter / 1.1 );
                break;
            case ']':
                _print_mandelbrot( parameter * 1.1 );
                break;
            case '{':
                ++cycle;
                _print_mandelbrot( parameter );
                break;
            case '}':
                --cycle;
                _print_mandelbrot( parameter );
                break;
        }
    }
}

void _print_vesa_demo() {
    vbe_mode_info_t* c = (vbe_mode_info_t*)VBE_MODE_INFO_LOCATION;
    uint32_t iter = 0;
    uint32_t x_res = c->x_res;
    uint32_t y_res = c->y_res;
    uint32_t x_res_over2 = c->x_res >> 1;
    uint32_t max_offset = c->x_res;
 
    // Here's an overoptimized demo!
    while( 1 ) {
        /*gs_puts_at_buf(0,10,"HELLO WORLD", &framebuffer );
        gs_puts_at_buf(0,84, "0123456789", &framebuffer );
        gs_puts_at_buf(0,100,"ABCDEFGHIJKLMNOPQRSTUVWXYZ", &framebuffer );
        gs_puts_at_buf(0,116,"abcdefghijklmnopqrstuvwxyz", &framebuffer );
        gs_puts_at_buf(0,132,"Timer ticks: ", &framebuffer);
        gs_puts_at_buf(156,132,itoa(_get_time()), &framebuffer);*/

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
}
