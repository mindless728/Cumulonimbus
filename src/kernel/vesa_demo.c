/**
 * file: vesa_demo.c
 * @author Benjamin Mayes
 * @description Some user functions to test the VESA driver.
 */

#include "gs_io.h"
#include "fpu.h"
#include "vesa_demo.h"
#include "user.h"

// an array containing hues to color the fractals
#define NUM_ITERS 2000

/**
 * Generates the hues based on the given parameter.
 *
 * @param The power that the percentage given by n/NUM_ITERS is raised to.
 */
static void generate_hues(double parameter, pixel_t hues[] ) {
    int i = 0;
    // calculate the color for every iteration
    for(; i < NUM_ITERS; ++i ) {
        double num = (double)i / NUM_ITERS;

        // caclulate the "percentage" based on the number and the parameter
        double x = pow(num, parameter);
        // convert the percentage into an angle
        x *= 6;
        int s = (int)x;
        while( x > 2.0 ) x -= 2.0;
        x -= 1.0;
        if( x < 0.0 ) x = -x;
        x = 1 - x;

        // sanity checks
        if( x > 1.0 ) s = 6; 
        if( x < 0.0 ) s = 0;

        // based on the sixth that the angle resides in, we use different
        // coloring rules
        switch(s) {
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
    }
}

/**
 * Prints a mandelbrot set coloring it with parameter.
 *
 * @param parameter The hue generation parameter.
 */
void print_mandelbrot( double parameter ) {
    static pixel_t hues[NUM_ITERS+1];
    double zoom = 128.0;
    double xoffset = 0;
    double yoffset = 0;
    int done = 0;
    hues[NUM_ITERS] = 0.0;
    while( 1 ) {
        generate_hues(parameter, hues);
        done = 0;
        while( !done ) {
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
                    gs_draw_pixel(c,r,hues[iter]);
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
                    parameter /= 1.1;
                    done = 1;
                    break;
                case ']':
                    parameter *= 1.1;
                    done = 1;
                    break;
                case 'x':
                    switchscreen(0);
                    break;
            }
        }
    }
}

/**
 * Prints a julia set of power 2 using the complex parameter cx + cy*i and colored
 * with the parameter.
 *
 * @param parameter The hue coloring parameter.
 * @param cx The real part of the complex parameter.
 * @param cy The imaginary part of the complex parameter.
 */
void print_julia( double parameter, double cx, double cy ) {
    static pixel_t hues[NUM_ITERS+1];
    double zoom = 128.0;
    double xoffset = 0;
    double yoffset = 0;
    int done = 0;
    hues[NUM_ITERS] = 0.0;
    while( 1 ) {
        generate_hues(parameter, hues);
        done = 0;
        while( !done ) {
            int r,c;
            for( r = 0; r < 1024; ++r ) {
                if( c_input_queue() ) break;
                for( c = 0; c < 1280; ++c ) {
                    int iter = 0;
                    double x = (c - 640 + xoffset)/zoom;
                    double y = (r - 512 + yoffset)/zoom;
                    double a = 0;
                    double b = 0;
                    double aold = x;
                    double bold = y;
                    while( iter < NUM_ITERS && (a*a+b*b) <= 4.0 ) {
                        a = aold*aold - bold*bold + cx; 
                        b = 2*aold*bold + cy;
                        ++iter;
                        aold = a;
                        bold = b;
                    }
                    gs_draw_pixel(c,r,hues[iter]);
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
                case '1':
                    cx += 0.01;
                    break;
                case '2':
                    cx -= 0.01;
                    break;
                case '3':
                    cy += 0.01;
                    break;
                case '4':
                    cy -= 0.01;
                    break;
                case 'r':
                    xoffset = 0;
                    yoffset = 0;
                    zoom = 128.0; 
                    break;
                case '[':
                    parameter /= 1.1;
                    done = 1;
                    break;
                case ']':
                    parameter *= 1.1;
                    done = 1;
                    break;
                case 'x':
                    switchscreen(0);
                    break;
            }
        }
    }
}

/**
 * Prints hues smoothly on the screen.
 */
void print_hue_test() {
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
            gs_draw_pixel(x,y, color);
        }
    }
}
