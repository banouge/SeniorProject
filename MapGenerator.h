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

	void removeUnwantedTerritories(int numTerritories, std::unordered_set<Territory*>& disconnectedTerritories);
	void reconnectMap(std::unordered_set<Territory*>& disconnectedTerritories);
	std::pair<Territory*, Territory*> getNewTerritoryConnection(Territory* territory, std::unordered_set<Territory*>& disconnectedTerritories);
};