#pragma once

#include "Delaunay.h"
#include "Map.h"
#include "Perlin.h"

class MapGenerator
{
public:
	MapGenerator(int numCells, int numTerritories, float width, float height);
	~MapGenerator();

private:
	Perlin noise;
	Delaunay delaunay;
	std::unordered_set<Territory*> territories;

	static bool compareTerritoryPtr(Territory* a, Territory* b);
};