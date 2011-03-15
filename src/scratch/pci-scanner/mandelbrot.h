#ifndef MANDELBROT_H
#define MANDELBROT_H

struct MandelbrotInfo{
	unsigned char maxIter;
	float zoom;
	float xShift;
	float yShift;
};

void setCharAt(unsigned int x, unsigned int y, unsigned char value);

unsigned char mandlebrotCompute(float x, float y);

void drawMandelbrot(void);

void keyboard_handler(int vector, int code);


#endif	//MANDELBROT_H

