/*
A simple graphics library for CSE 20211 by Douglas Thain

This work is licensed under a Creative Commons Attribution 4.0 International
License.  https://creativecommons.org/licenses/by/4.0/

For course assignments, you should not change this file.
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2013/gfx
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

#ifndef GFX_H
#define GFX_H

/* Open a new graphics window. */
void gfx_open(int width, int height, const char* title);

/* Draw a point at (x,y) */
void gfx_point(int x, int y);

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line(int x1, int y1, int x2, int y2);

/* Change the current drawing color. */
void gfx_color(int red, int green, int blue);

/* Clear the graphics window to the background color. */
void gfx_clear();

/* Change the current background color. */
void gfx_clear_color(int red, int green, int blue);

/* Wait for the user to press a key or mouse button. */
char gfx_wait();

/* Return the X and Y coordinates of the last event. */
int gfx_xpos();
int gfx_ypos();

/* Return the X and Y dimensions of the window. */
int gfx_xsize();
int gfx_ysize();

/* Check to see if an event is waiting. */
int gfx_event_waiting();

/* Flush all previous output to the window. */
void gfx_flush();

/* Added new function: Draw an entire row of pixels.
   'y' is the row index,
   'row' is an array of pixel values (each as a 24-bit integer),
   and 'width' is the number of pixels in the row.
*/
void gfx_draw_row(int y, int* row, int width);

#endif
