#pragma once

#include <fstream>
#include <unordered_map>
#include "Region.h"
#include "Territory.h"

class Map
{
public:
	Map(std::string name = "");
	~Map();

	const std::string NAME;

	Territory* getTerritoryAtPoint(sf::Vector2i point);
	void draw(sf::RenderWindow* window);

private:
	std::unordered_map<std::string, Territory*> territories;
	std::unordered_map<std::string, Region*> regions;
	std::unordered_set<sf::Vector2f*> territoryPositions;
	std::unordered_map<Territory*, int> territoryLeftPointIndices;
	std::unordered_map<Territory*, int> territoryRightPointIndices;
	std::random_device seed;
	std::mt19937 rng;

	void loadMap();
	void loadTerritory(std::ifstream& mapFile);
	void loadRegion(std::ifstream& mapFile);
	sf::Color getRandomColor();
};