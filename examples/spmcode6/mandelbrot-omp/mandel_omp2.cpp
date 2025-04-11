#include <iostream>
#include <complex>
#include <vector>
#include <hpc_helpers.hpp>
#include "gfx.h"

#if defined(NO_DISPLAY)
#define DISPLAY(X)
#else
#define DISPLAY(X) X
#endif

// a very large value
const int max_iter = 50000;  

// size in pixels of the picture window
const int XSIZE = 1280;
const int YSIZE = 800;

/* Coordinates of the bounding box of the Mandelbrot set */
const double XMIN = -2.3, XMAX = 1.0;
const double SCALE = (XMAX - XMIN)*YSIZE / XSIZE;
const double YMIN = -SCALE/2, YMAX = SCALE/2;

struct pixel {
    int r, g, b;
};

const pixel colors[] = {
    { 66,  30,  15}, 
    { 25,   7,  26},
    {  9,   1,  47},
    {  4,   4,  73},
    {  0,   7, 100},
    { 12,  44, 138},
    { 24,  82, 177},
    { 57, 125, 209},
    {134, 181, 229},
    {211, 236, 248},
    {241, 233, 191},
    {248, 201,  95},
    {255, 170,   0},
    {204, 128,   0},
    {153,  87,   0},
    {106,  52,   3} };
const int NCOLORS = sizeof(colors)/sizeof(pixel);


//  z_(0)   = 0;
//  z_(n+1) = z_n * z_n + (cx + i*cy);
//
// iterates until ||z_n|| > 2, or max_iter
//
int iterate(const std::complex<double>& c) {
	std::complex<double> z=0;
	int iter = 0;
	while(iter < max_iter && std::abs(z) <= 2.0) {
		z = z*z + c;
		++iter;
	}
	return iter;
}

void drawpixel(int x, int y, int iter) {
	int r=0,g=0,b=0;
    if (iter < max_iter) {
		const int m= iter % NCOLORS;
		r = colors[m].r;
		g = colors[m].g;
		b = colors[m].b;
	}
	gfx_color(r, g, b);
    gfx_point(x, y);
}

void drawrowpixels(int y, const std::vector<int>& row) {
	int row_pixels[XSIZE];
	for (int x = 0; x < XSIZE; ++x) {
		int r = 0, g = 0, b = 0;
		if (row[x] < max_iter) {
			int m = row[x] % NCOLORS;
			r = colors[m].r;
			g = colors[m].g;
			b = colors[m].b;
		}
		// Construct a 24-bit pixel value (BGR ordering as used in gfx_color)
		row_pixels[x] = ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
	}
	// Draw the entire row at once.
	gfx_draw_row(y, row_pixels, XSIZE);
}

int main(int argc, char *argv[]) {
    DISPLAY(gfx_open(XSIZE, YSIZE, "Mandelbrot Set"));
#if defined(NO_DISPLAY)	
	volatile int fake=0;
#endif	
	TIMERSTART(mandel_seq);
	#pragma omp parallel for schedule(runtime) 
    for (int y = 0; y < YSIZE; ++y) {
		const double im = YMAX - (YMAX - YMIN) * y / (YSIZE - 1);
		// temporary buffer to store the pixel values for the current line
		std::vector<int> results(XSIZE);
		for (int x = 0; x < XSIZE; ++x) {
            const double re = XMIN + (XMAX - XMIN) * x / (XSIZE - 1);
            results[x] = iterate(std::complex<double>(re, im));
		}
#if defined(NO_DISPLAY)
		fake+=results[0];
#else
#pragma omp critical
			drawrowpixels(y, results);
#endif					
    }
	TIMERSTOP(mandel_seq);
	DISPLAY(std::cout << "Click to finish\n"; gfx_wait());
}
