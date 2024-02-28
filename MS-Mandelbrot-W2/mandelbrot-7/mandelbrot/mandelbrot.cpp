#include "mandelbrot.h"
#include "Farm.h"

uint32_t image[HEIGHT][WIDTH];

void compute_mandelbrot_row(double left, double right, double top, double bottom, int row)
{
	for (int x = 0; x < WIDTH; ++x)
	{
		// Work out the point in the complex plane that
		// corresponds to this pixel in the output image.
		complex<double> c(left + (x * (right - left) / WIDTH),
			top + (row * (bottom - top) / HEIGHT));

		// Start off z at (0, 0).
		complex<double> z(0.0, 0.0);

		// Iterate z = z^2 + c until z moves more than 2 units
		// away from (0, 0), or we've iterated too many times.
		int iterations = 0;
		while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
		{
			z = (z * z) + c;

			++iterations;
		}

		if (iterations == MAX_ITERATIONS)
		{
			// z didn't escape from the circle.
			// This point is in the Mandelbrot set.
			//image[y][x] = 0x000000; // black

			uint8_t red = (iterations % 256);  // Example for red gradient
			uint8_t green = 0; ((iterations + 100) % 256);                  // Set green to 0
			uint8_t blue = (iterations % 256);                   // Set blue to 0

			image[row][x] = (red << 16) | (green << 8) | blue; // Combine channels
		}
		else
		{
			// z escaped within less than MAX_ITERATIONS
			// iterations. This point isn't in the set.
			//image[y][x] = 0xFFFFFF; // white
			// 
			// Create a colorful palette based on the number of iterations
			uint8_t red = static_cast<uint8_t>(128.0f + sin(iterations * 0.15f) * 128.0f);
			uint8_t green = 0;// static_cast<uint8_t>(128.0f + sin(iterations * 0.16f) * 128.0f);
			uint8_t blue = static_cast<uint8_t>(128.0f + sin(iterations * 0.17f) * 128.0f);

			image[row][x] = (red << 16) | (green << 8) | blue;

		}
	}
}

void write_tga(const char* filename)
{
	ofstream outfile(filename, ofstream::binary);

	uint8_t header[18] = {
		0, // no image ID
		0, // no colour map
		2, // uncompressed 24-bit image
		0, 0, 0, 0, 0, // empty colour map specification
		0, 0, // X origin
		0, 0, // Y origin
		WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, // width
		HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, // height
		24, // bits per pixel
		0, // image descriptor
	};
	outfile.write((const char*)header, 18);

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			uint8_t pixel[3] = {
				image[y][x] & 0xFF, // blue channel
				(image[y][x] >> 8) & 0xFF, // green channel
				(image[y][x] >> 16) & 0xFF, // red channel
			};
			outfile.write((const char*)pixel, 3);
		}
	}

	outfile.close();
	if (!outfile)
	{
		// An error has occurred at some point since we opened the file.
		cout << "Error writing to " << filename << endl;
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	Farm farm;
	for (int i = 0; i < 1200; ++i) {
		MandelbrotTask mbt{ -2.0, 1.0, 1.125, -1.125, i };
		farm.add_task(mbt);
	}
	farm.run();
	return 0;
}
