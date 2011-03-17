/**
*	Author:	Alan Meekins
*	File: mandelbrot.c
*
*	Description:
*		Implements drawing routines for mandelbrot sets
*
*/



#include "uart.h"
#include "x86arch.h"
#include "c_io.h"
#include "startup.h"
#include "mandelbrot.h"

#define DEFAULT_ZOOM 0.07f
#define DEFAULT_MAX_ITER 100
#define	KEYBOARD_DATA	0x60
#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25
#define X_SCALE ((float)(3.5f/(float)(SCREEN_WIDTH)))
#define Y_SCALE ((float)(2.0f/(float)(SCREEN_HEIGHT)))

#define	VIDEO_ADDR(x,y)	( unsigned short * ) \
		( VIDEO_BASE_ADDR + 2 * ( (y) * SCREEN_WIDTH + (x) ) )


#define TRANSITION_STEPS 30
#define PAUSE_STEPS 30

#define DRAW_USER	0x01
#define DRAW_ANIM	0x02
#define ANIM_PAUSE	0x04

char drawMode = ANIM_PAUSE;

int animClock = 0;
int animIndex = 0;
int animFrame = 0;

struct MandelbrotInfo mbInfo = { DEFAULT_MAX_ITER, DEFAULT_ZOOM, -1.0f, 0.0f, 0.0f };

struct MandelbrotInfo stepSize = { DEFAULT_MAX_ITER, 0.0f, 0.0f, 0.0f, 0.0f };

//Animation Key frame data
struct MandelbrotInfo targets[] = {
	{ DEFAULT_MAX_ITER, DEFAULT_ZOOM, -1.0f, 0.0f, 0.0f },
	{ DEFAULT_MAX_ITER, 0.002f, -0.21349712f, 0.8456341f,  0.0f },
	{ DEFAULT_MAX_ITER, DEFAULT_ZOOM, -1.0f, 0.0f, 0.0f },
	{ DEFAULT_MAX_ITER, 0.002f, -1.15056418f, -0.21936683f, 0.0f },
	{ DEFAULT_MAX_ITER, DEFAULT_ZOOM, -1.0f, 0.0f, 0.0f },
	{ DEFAULT_MAX_ITER, 0.002f, 0.2774665f, -0.5384487f, 0.0f },
	{ DEFAULT_MAX_ITER, DEFAULT_ZOOM, -1.0f, 0.0f, 0.0f },
	{ DEFAULT_MAX_ITER, 0.0002f, -0.754f, 0.0732759f, 0.0f },
	{ DEFAULT_MAX_ITER, DEFAULT_ZOOM, -1.0f, 0.0f, 0.0f },
	{ 0, -1.0f, -1.0f, -1.0f, -1.0f }
};

/**
 * 	Implements a simple key-frame animation routine using
 * 	linear interpolation to auto-generate intermediate frames
*/
void animateMandelbrot(void){
	//Check current mode
	if(drawMode == DRAW_USER){
		//Don't animate, user is controlling system
		return;
	}
	else if(drawMode==DRAW_ANIM){
		//Still have frames to draw?
		if(animClock < TRANSITION_STEPS){
			//Compute frame parameters
			animClock++;
			//mbInfo.maxIter -= stepSize.maxIter;
			mbInfo.zoom -= stepSize.zoom;
			mbInfo.xShift -= stepSize.xShift;
			mbInfo.yShift -= stepSize.yShift;
			mbInfo.theta -= stepSize.theta;
		}
		else{
			//Finished animating
			drawMode = ANIM_PAUSE;
			animClock = 0;
			mbInfo = targets[animIndex];
		}
	}
	else if(drawMode==ANIM_PAUSE){
		if(animClock < PAUSE_STEPS){
			animClock++;
		}
		else{
			int prevIndex = animIndex;
			drawMode = DRAW_ANIM;
			animIndex++;
			animClock=0;
			if(targets[animIndex].maxIter == 0){
				animIndex=0;
			}

			//Compute step sizes for animating
			stepSize.maxIter = (targets[prevIndex].maxIter - targets[animIndex].maxIter) / TRANSITION_STEPS;
			stepSize.zoom = (targets[prevIndex].zoom - targets[animIndex].zoom) / TRANSITION_STEPS;
			stepSize.xShift = (targets[prevIndex].xShift - targets[animIndex].xShift) / TRANSITION_STEPS;
			stepSize.yShift = (targets[prevIndex].yShift - targets[animIndex].yShift) / TRANSITION_STEPS;
			stepSize.theta = (targets[prevIndex].theta - targets[animIndex].theta) / TRANSITION_STEPS;
		}
	}

	//Redraw
	drawMandelbrot();
}

/**
 * 	Sets the background color of the specified point
 * 	@param	x
 * 	@param	y
 * 	@param	value	3bit color value
 */
void setCharAt(unsigned int x, unsigned int y, unsigned char value){
	unsigned short *addr = VIDEO_ADDR( x, y );

	//Sets the color bits
	*addr = (((unsigned short)value & 0x70) << 7) | ' ';
}

/**
 * 	Returns a value bassed on whether the point
 * 	(x,y) is contained in the mandelbrot set.
 * 	@param	x
 * 	@param	y
 */
unsigned char mandlebrotCompute(float x, float y){
	unsigned char i=0;
	float x0 = x;
	float y0 = y;

	float value=0;

	while( value < 16.0f && i < mbInfo.maxIter){
		float xtemp = ((x*x) - (y*y)) + x0;
		y = (2.0f*x*y) + y0;


		x=xtemp;

		value = (x*x)*(y*y);

		i++;

	}

	return mbInfo.maxIter-i;
}

/**
 * 	Draws a mandelbrot set to the console using
 * 	the mbInfo struct for viewing parameters.
 */
void drawMandelbrot(void){

	int i = 0;

	for(i=0; i<SCREEN_WIDTH; i++){
		int j=0;
		for(j=0; j<SCREEN_HEIGHT; j++){

			float x = (i - ((float)SCREEN_WIDTH)/2.0f);
			float y = ((((float)SCREEN_HEIGHT)/2.0f) - j) * 2.0f;

			//Shift and zoom
			x = (mbInfo.zoom * x) + mbInfo.xShift;
			y = (mbInfo.zoom * y) + mbInfo.yShift;


			char iterations = mandlebrotCompute(x, y);

			setCharAt(i, j, iterations);
		}
	}
}


/**
 * 	Handler for console inputs, supports starting and
 * 	stopping animation, scrolling and panning, zoom,
 * 	and color scheme(using number keys 1-7)
 */
void keyboard_handler(int vector, int code){
	char scan = __inb( KEYBOARD_DATA );

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );


	switch(scan){
		case 0x48:		//Up Arrow
			mbInfo.yShift += 0.05f;
			break;
		case 0x50:		//Down arrow
			mbInfo.yShift -= 0.05f;
			break;
		case 0x4b:		//Left arrow
			mbInfo.xShift -= 0.05f;
			break;
		case 0x4d:		//Right arrow
			mbInfo.xShift += 0.05f;
			break;
		case 0x49:		//Zoom out
			mbInfo.zoom *= 1.5f;
			break;
		case 0x51:		//Zoom in
			mbInfo.zoom *= 0.8f;
			break;
		case 0x02:		//1
			mbInfo.maxIter = 64;
			break;
		case 0x03:		//2
			mbInfo.maxIter = 100;
			break;
		case 0x04:		//3
			mbInfo.maxIter = 115;
			break;
		case 0x05:		//4
			mbInfo.maxIter = 127;
			break;
		case 0x06:		//5
			mbInfo.maxIter = 200;
			break;
		case 0x07:		//6
			mbInfo.maxIter = 215;
			break;
		case 0x08:		//7
			mbInfo.maxIter = 255;
			break;
		case 0x01:		//ESC - Reset
			mbInfo.maxIter = DEFAULT_MAX_ITER;
			mbInfo.xShift = 0.0f;
			mbInfo.yShift = 0.0f;
			mbInfo.zoom = DEFAULT_ZOOM;
			if(drawMode == DRAW_USER){
				drawMode = ANIM_PAUSE;
			}
			else{
				drawMode = DRAW_USER;
			}
			break;
		default:
			//c_printf_at(0, 7, "keyboard_handler vector=%d code=%d scanCode=0x%x\n", vector, code, scan);
			return;
	}

	drawMandelbrot();

	c_printf_at(0,1, "maxIter=%d x=%d y=%d zoom=%d\n", mbInfo.maxIter, (int)(mbInfo.xShift*1000.0f),
													(int)(mbInfo.yShift*1000.0f), (int)(mbInfo.zoom*1000.0f));
}

