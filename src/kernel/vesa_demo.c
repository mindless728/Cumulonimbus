/**
 * File: vesa_demo.c
 * Author: Benjamin Mayes <bdm8233@rit.edu>
 * Description Some user functions to test the VESA driver.
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
        // generate the hues
        generate_hues(parameter, hues);
        done = 0;
        while( !done ) {
            // for each pixel we want to check to see if that location is in
            // the mandelbrot set
            int r,c;
            for( r = 0; r < 1024; ++r ) {
                if( c_input_queue() ) break; // let's be safe and break if there is input
                for( c = 0; c < 1280; ++c ) {
                    // mandelbrot set:
                    // (a+bi)^2 = (a^2 - b^2) + i(2*a*b)
                    //             ^new a^       ^new b^
                    // and x is added to a, y to b after the transformation is applied
                    // we perform this function iteratively until one of two
                    // things happen:
                    //      1. The max number of iterations is reached
                    //          - at this point we assume it is in the
                    //            mandelbrot set
                    //      2. a*a+b*b > 4
                    //          - it has been proven that any point that
                    //            satisifies this condition will tend towards
                    //            infinity and is not part of the set
                    //
                    //      Pixels are colored by using the number of iterations
                    //      before termination as an index into the hues array.
                    //
                    //      The result is pretty.
                    int iter = 0;
                    double a = 0.0;
                    double b = 0.0;
                    double aold = 0.0;
                    double bold = 0.0;
                    double x = (c - 640 + xoffset)/zoom;
                    double y = (r - 512 + yoffset)/zoom;
                    while( iter < NUM_ITERS && (a*a+b*b) <= 4.0 ) {
                        // iteratively perform the transformation
                        a = aold*aold - bold*bold + x; 
                        b = 2*aold*bold + y;
                        ++iter;
                        aold = a;
                        bold = b;
                    }

                    // draw the pixel
                    gs_draw_pixel(c,r,hues[iter]);
                }
            }

            // wait for a keypres and do something cool!
            char key = c_getchar();
            switch(key) {
                case '+':
                    // zoom in
                    zoom *= 2.0;
                    xoffset *= 2.0;
                    yoffset *= 2.0;
                    break;
                case '-':
                    // zoom out
                    zoom *= 0.5;
                    xoffset *= 0.5;  
                    yoffset *= 0.5;  
                    break;

                case 'w':
                    // shift the screen up
                    yoffset -= 100.0;
                    break;
                case 's':
                    // shift the screen down
                    yoffset += 100.0;
                    break;
                case 'a':
                    // shift the screen left
                    xoffset -= 100.0;
                    break;
                case 'd':
                    // shift the screen right
                    xoffset += 100.0;
                    break;
                case 'r':
                    // reset the screen
                    xoffset = 0;
                    yoffset = 0;
                    zoom = 128.0; 
                    break;
                case '[':
                    // change the coloring parameter
                    parameter /= 1.1;
                    done = 1;
                    break;
                case ']':
                    // change the coloring parameter
                    parameter *= 1.1;
                    done = 1;
                    break;
                case 'x':
                    // swich back to the main screen
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
                    // see comments in the mandelbrot set for an explanation.
                    // only difference here is that a and b start at x and y
                    // and each iteration the chosen complex parameter is added
                    // to the complex number.
                    //
                    // Fun fact:
                    // Location (x,y) of the mandelbrot set is colored identically
                    // to location (0,0) of the julia set with the complex
                    // parameter x+iy.
                    int iter = 0;
                    double x = (c - 640 + xoffset)/zoom;
                    double y = (r - 512 + yoffset)/zoom;
                    double a = 0;
                    double b = 0;
                    // here a and b are initialized with the location
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
                    // zoom in
                    zoom *= 2.0;
                    xoffset *= 2.0;
                    yoffset *= 2.0;
                    break;
                case '-':
                    // zoom out
                    zoom *= 0.5;
                    xoffset *= 0.5;  
                    yoffset *= 0.5;  
                    break;
                case 'w':
                    // shift the image up
                    yoffset -= 100.0;
                    break;
                case 's':
                    // shift the image down
                    yoffset += 100.0;
                    break;
                case 'a':
                    // shift the image to the left
                    xoffset -= 100.0;
                    break;
                case 'd':
                    // shift the image to the right
                    xoffset += 100.0;
                    break;
                case '1':
                    // adjust the real portion of the complex parameter
                    cx += 0.01;
                    break;
                case '2':
                    // adjust the real portion of the complex parameter
                    cx -= 0.01;
                    break;
                case '3':
                    // adjust the imaginary portion of the complex parameter
                    cy += 0.01;
                    break;
                case '4':
                    // adjust the imaginary portion of the complex parameter
                    cy -= 0.01;
                    break;
                case 'r':
                    // reset the scene
                    xoffset = 0;
                    yoffset = 0;
                    zoom = 128.0; 
                    break;
                case '[':
                    // adjust the coloring parameter
                    parameter /= 1.1;
                    done = 1;
                    break;
                case ']':
                    // adjust the coloring parameter
                    parameter *= 1.1;
                    done = 1;
                    break;
                case 'x':
                    // switch back to the main screen
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
    // for each row y we are going to calculate y/1024 and generate a hue
    for( y = 0; y < 1024; ++y ) {
        double hue = (double)y/1024;

        // determine the sixth of the circle this hue lies in
        double z = 6*hue;
        int h = z;

        // now determine the offset into the sixth to obtain the multiplier
        // for the secondary color of the pixel.
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
