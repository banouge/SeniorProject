#include <fstream>
#include "Bitmap.h"

//REFERENCES:
//https://en.wikipedia.org/wiki/BMP_file_format

Bitmap::Bitmap(int width, int height) : PADDED_WIDTH((4 - (3 * width) % 4) % 4 + 3 * width), HEIGHT(height), SIZE(height * PADDED_WIDTH + 54)
{
	data = new unsigned char[SIZE];

	//bitmap file header
	//header field
	data[0] = 'B';
	data[1] = 'M';

	//file size in bytes (little-endian)
	data[2] = SIZE % 256;
	data[3] = (SIZE / 256) % 256;
	data[4] = (SIZE / 65536) % 256;
	data[5] = (SIZE / 16777216) % 256;

	//reserved
	data[6] = 0;
	data[7] = 0;
	data[8] = 0;
	data[9] = 0;

	//index of start of pixel data (little-endian)
	data[10] = 54;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;

	//DIB header (BITMAPINFOHEADER)
	//size of DIB header in bytes (little-endian)
	data[14] = 40;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;

	//image width in pixels (little-endian/signed)
	data[18] = width % 256;
	data[19] = (width / 256) % 256;
	data[20] = (width / 65536) % 256;
	data[21] = (width / 16777216) % 256;

	//image height in pixels (little-endian/signed)
	data[22] = height % 256;
	data[23] = (height / 256) % 256;
	data[24] = (height / 65536) % 256;
	data[25] = (height / 16777216) % 256;

	//number of color planes (little-endian)
	data[26] = 1;
	data[27] = 0;

	//bits per pixel (little-endian)
	data[28] = 24;
	data[29] = 0;

	//compression method
	data[30] = 0;
	data[31] = 0;
	data[32] = 0;
	data[33] = 0;

	//image size (can put dummy 0)
	data[34] = 0;
	data[35] = 0;
	data[36] = 0;
	data[37] = 0;

	//pixels per horizontal meter (can put dummy 0)
	data[38] = 0;
	data[39] = 0;
	data[40] = 0;
	data[41] = 0;

	//pixels per vertical meter (can put dummy 0)
	data[42] = 0;
	data[43] = 0;
	data[44] = 0;
	data[45] = 0;

	//number of colors in palette (can put dummy 0)
	data[46] = 0;
	data[47] = 0;
	data[48] = 0;
	data[49] = 0;

	//number of important colors (can put dummy 0)
	data[50] = 0;
	data[51] = 0;
	data[52] = 0;
	data[53] = 0;

	//pixel data
	//rgb and padding
	for (int i = 54; i < SIZE; ++i)
	{
		data[i] = 0;
	}
}

Bitmap::~Bitmap()
{
	delete[] data;
}

void Bitmap::paintPixel(int r, int g, int b, int x, int y)
{
	int index = (HEIGHT - y - 1) * PADDED_WIDTH + x * 3 + 54;
	data[index] = b % 256;
	data[index + 1] = g % 256;
	data[index + 2] = r % 256;
}

void Bitmap::createFile(std::string name)
{
	std::ofstream file(name, std::ios::binary);

	for (int i = 0; i < SIZE; ++i)
	{
		file << data[i];
	}

	file.close();
}