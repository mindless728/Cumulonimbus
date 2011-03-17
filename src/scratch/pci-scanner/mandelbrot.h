/**
*	Author:	Alan Meekins
*	File: mandelbrot.h
*/


#ifndef MANDELBROT_H
#define MANDELBROT_H

/**
 * Defines a view point
 */
struct MandelbrotInfo{
	unsigned char maxIter;
	float zoom;
	float xShift;
	float yShift;
	float theta;
};

void setCharAt(unsigned int x, unsigned int y, unsigned char value);

unsigned char mandlebrotCompute(float x, float y);

void drawMandelbrot(void);

void animateMandelbrot(void);

void keyboard_handler(int vector, int code);


#endif	//MANDELBROT_H
