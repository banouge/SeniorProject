#pragma once

#include <fstream>
#include <unordered_map>
#include "Territory.h"

class Map
{
public:
	Map(std::string name = "");
	~Map();

	const std::string NAME;

private:
	std::unordered_map<std::string, Territory*> territories;
	std::unordered_map<std::string, Region*> regions;
	std::unordered_set<sf::Vector2f*> territoryPositions;

	void loadMap();
	void loadTerritory(std::ifstream& mapFile);
	void loadRegion(std::ifstream& mapFile);
};