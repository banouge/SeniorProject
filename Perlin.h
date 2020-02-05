#pragma once

#include <vector>

//REFERENCES:
//https://cs.nyu.edu/~perlin/noise/

class Perlin
{
public:
	Perlin();
	Perlin(unsigned int seed);

	double noise(double x, double y, double z);

private:
	std::vector<int> permutation;

	double fade(double t);
	double gradient(int hash, double x, double y, double z);
	double lerp(double t, double a, double b);
};