#include <chrono>
#include <numeric>
#include <random>
#include "Perlin.h"

//REFERENCES:
//https://cs.nyu.edu/~perlin/noise/

Perlin::Perlin()
{
	//get seed from time
	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();

	//fill permutation with 0 to 255
	permutation.resize(256);
	std::iota(permutation.begin(), permutation.end(), 0);

	//permute
	std::default_random_engine engine(seed);
	std::shuffle(permutation.begin(), permutation.end(), engine);

	//duplicate all values and place at end
	permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

Perlin::Perlin(unsigned int seed)
{
	//fill permutation with 0 to 255
	permutation.resize(256);
	std::iota(permutation.begin(), permutation.end(), 0);

	//permute
	std::default_random_engine engine(seed);
	std::shuffle(permutation.begin(), permutation.end(), engine);

	//duplicate all values and place at end
	permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

//returns value between -1 and 1
double Perlin::noise(double x, double y, double z)
{
	//get coords of unit cube that contains (x, y, z)
	int cubeX = (int)floor(x) & 255;
	int cubeY = (int)floor(y) & 255;
	int cubeZ = (int)floor(z) & 255;

	//get relative coords in that cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	//get fade coords
	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	//hash coords of cube vertices
	int a = permutation[cubeX] + cubeY;
	int aa = permutation[a] + cubeZ;
	int ab = permutation[a + 1] + cubeZ;
	int b = permutation[cubeX + 1] + cubeY;
	int ba = permutation[b] + cubeZ;
	int bb = permutation[b + 1] + cubeZ;

	//add blended results from cube vertices
	double lerpAA = lerp(u, gradient(permutation[aa], x, y, z), gradient(permutation[ba], x - 1, y, z));
	double lerpAB = lerp(u, gradient(permutation[ab], x, y - 1, z), gradient(permutation[bb], x - 1, y - 1, z));
	double lerpA = lerp(v, lerpAA, lerpAB);
	double lerpBA = lerp(u, gradient(permutation[aa + 1], x, y, z - 1), gradient(permutation[ba + 1], x - 1, y, z - 1));
	double lerpBB = lerp(u, gradient(permutation[ab + 1], x, y - 1, z - 1), gradient(permutation[bb + 1], x - 1, y - 1, z - 1));
	double lerpB = lerp(v, lerpBA, lerpBB);
	return lerp(w, lerpA, lerpB);
}

double Perlin::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double Perlin::gradient(int hash, double x, double y, double z)
{
	//get lowest 4 bits
	hash &= 15;

	//get gradient direction
	double u = (hash < 8) ? (x) : (y);
	double v = (hash < 4) ? (y) : ((hash == 12 || hash == 14) ? (x) : (z));
	return (((hash & 1) == 0) ? (u) : (-u)) + (((hash & 2) == 0) ? (v) : (-v));
}

double Perlin::lerp(double t, double a, double b)
{
	return a + t * (b - a);
}