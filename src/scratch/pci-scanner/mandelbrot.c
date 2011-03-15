#include "uart.h"
#include "x86arch.h"
#include "c_io.h"
//#include "support.h"

#include "startup.h"
#include "mandelbrot.h"

#define	KEYBOARD_DATA	0x60
#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25
#define X_SCALE ((float)(3.5f/(float)(SCREEN_WIDTH)))
#define Y_SCALE ((float)(2.0f/(float)(SCREEN_HEIGHT)))

#define	VIDEO_ADDR(x,y)	( unsigned short * ) \
		( VIDEO_BASE_ADDR + 2 * ( (y) * SCREEN_WIDTH + (x) ) )

struct MandelbrotInfo mbInfo = { 100, 1.0f, 0.0f, 0.0f };

void setCharAt(unsigned int x, unsigned int y, unsigned char value){
	unsigned short *addr = VIDEO_ADDR( x, y );

	/*
	* Left shift color value 7 bits, print a space
	*/

	*addr = (((unsigned short)value & 0x70) << 7) | ' ';
}

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


void drawMandelbrot(void){

	//float xScale = (3.5f/(float)(TEXT_WIDTH));
	//float yScale = (2.0f/(float)(TEXT_HEIGHT));

	int i = 0;

	for(i=0; i<SCREEN_WIDTH; i++){
		int j=0;
		for(j=0; j<SCREEN_HEIGHT; j++){

			float x = (float)(i) + mbInfo.xShift;
			x = mbInfo.zoom * ((x*X_SCALE) - 2.5f);

			float y = -((float)(j) - mbInfo.yShift);
			y = mbInfo.zoom * ((y*Y_SCALE) + 1.0f);

			char iterations = mandlebrotCompute(x, y);

			//cerr<<"("<<i<<", "<<j<<") "<<"("<<x<<", "<<y<<") = "<<(int)iterations<<endl;

			setCharAt(i, j, iterations);
		}
	}
}


void keyboard_handler(int vector, int code){
	char scan = __inb( KEYBOARD_DATA );
	
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );


	switch(scan){
		case 0x48:		//Up Arrow
			mbInfo.yShift += 0.5f;
			break;
		case 0x50:		//Down arrow
			mbInfo.yShift -= 0.5f;
			break;
		case 0x4b:		//Left arrow
			mbInfo.xShift -= 0.5f;
			break;
		case 0x4d:		//Right arrow
			mbInfo.xShift += 0.5f;
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
			mbInfo.maxIter = 100;
			mbInfo.xShift = 0.0f;
			mbInfo.yShift = 0.0f;
			mbInfo.zoom = 1.0f;
			break;
		default:
			//c_printf_at(0, 7, "keyboard_handler vector=%d code=%d scanCode=0x%x\n", vector, code, scan);
			return;
	}

	drawMandelbrot();

	c_printf_at(0,1, "maxIter=%d x=%d y=%d zoom=%d\n", mbInfo.maxIter, (int)(mbInfo.xShift*1000.0f),
													(int)(mbInfo.yShift*1000.0f), (int)(mbInfo.zoom*1000.0f));
}

