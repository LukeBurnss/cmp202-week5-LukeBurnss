#pragma once
// Mandelbrot set example

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <complex>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

// Import things we need from the standard library
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::complex;
using std::cout;
using std::endl;
using std::ofstream;

// Define the alias "the_clock" for the clock type we're going to use.
typedef std::chrono::steady_clock the_clock;


// The size of the image to generate.
const int WIDTH = 1920;
const int HEIGHT = 1200;

// The number of times to iterate before we assume that a point isn't in the Mandelbrot set.
const int MAX_ITERATIONS = 500;

// The image data. Each pixel is represented as 0xRRGGBB.
extern uint32_t image[HEIGHT][WIDTH];

// Write the image to a TGA file with the given name.
// Format specification: http://www.gamers.org/dEngine/quake3/TGA.txt
void write_tga(const char* filename);

// Computes a single row of the Mandelbrot set and fills the corresponding part of the image array.
void compute_mandelbrot_row(double left, double right, double top, double bottom, int row); 

