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

	static sf::Color getRandomColor();

	Territory* getTerritoryAtPoint(sf::Vector2i point);
	std::unordered_map<std::string, Region*>& getRegions();
	void draw(sf::RenderWindow* window);
	void setOrigin(sf::Vector2f origin);

private:
	static std::random_device seed;
	static std::mt19937 rng;

	std::unordered_map<std::string, Territory*> territories;
	std::unordered_map<std::string, Region*> regions;
	std::unordered_set<sf::Vector2f*> territoryPositions;
	std::unordered_map<Territory*, int> territoryLeftPointIndices;
	std::unordered_map<Territory*, int> territoryRightPointIndices;
	std::unordered_set<sf::VertexArray*> distantConnections;
	sf::Vector2f origin;

	void loadMap();
	void loadTerritory(std::ifstream& mapFile);
	void loadRegion(std::ifstream& mapFile);
};