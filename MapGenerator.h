#pragma once

#include "Delaunay.h"
#include "Map.h"
#include "Perlin.h"
#include "Region.h"

class MapGenerator
{
public:
	MapGenerator(int numCells, int numTerritories, int numRegions, float width, float height, std::string name);
	~MapGenerator();

private:
	Perlin noise;
	Delaunay delaunay;
	std::unordered_set<Territory*> territories;
	std::unordered_set<Region*> regions;

	static bool compareTerritoryPtr(Territory* a, Territory* b);
	static std::string convertNumberToLetters(int number);

	void outputData(std::string name);
	void createRegions(int numRegions, std::unordered_set<Territory*>& regionSeeds);
	void expandRegions();
	void removeUnwantedTerritories(int numTerritories, std::unordered_set<Territory*>& disconnectedTerritories);
	void reconnectMap(std::unordered_set<Territory*>& disconnectedTerritories, std::unordered_set<Territory*>& regionSeeds);
	std::pair<Territory*, Territory*> getNewTerritoryConnection(Territory* territory, std::unordered_set<Territory*>& disconnectedTerritories);
};