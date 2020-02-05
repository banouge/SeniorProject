#pragma once

#include <string>

//REFERENCES:
//https://en.wikipedia.org/wiki/BMP_file_format

class Bitmap
{
public:
	Bitmap(int width, int height);
	~Bitmap();

	void paintPixel(int r, int g, int b, int x, int y);
	void createFile(std::string name);

private:
	const int PADDED_WIDTH;
	const int HEIGHT;
	const int SIZE;

	unsigned char* data;
};