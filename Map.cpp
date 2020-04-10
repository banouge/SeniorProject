#include <string>
#include "Map.h"

Map::Map(std::string name): NAME(name)
{
	std::ifstream mapsList("Maps.txt");
	std::string line;
	
	//load map if it exists
	while (std::getline(mapsList, line))
	{
		if (line == name)
		{
			loadMap();
			break;
		}
	}
}

Map::~Map()
{
	for (std::pair<std::string, Region*> pair : regions)
	{
		delete pair.second;
	}

	for (std::pair<std::string, Territory*> pair : territories)
	{
		delete pair.second;
	}

	for (sf::Vector2f* point : territoryPositions)
	{
		delete point;
	}
}

void Map::loadMap()
{
	std::ifstream mapFile("Maps/" + NAME + ".txt");
	std::string line;
	int num;

	//NUMBER OF TERRITORIES: ?
	std::getline(mapFile, line);
	num = std::stoi(line.substr(23));

	//load territories
	for (int t = 0; t < num; ++t)
	{
		loadTerritory(mapFile);
	}

	//NUMBER OF REGIONS: ?
	std::getline(mapFile, line);
	num = std::stoi(line.substr(19));

	//load regions
	for (int r = 0; r < num; ++r)
	{
		loadRegion(mapFile);
	}
}

void Map::loadTerritory(std::ifstream& mapFile)
{
	std::string line;
	std::string name;
	sf::Vector2f* point;
	sf::ConvexShape* shape;
	Territory* territory;
	int n;

	//NAME: ?
	std::getline(mapFile, line);
	name = line.substr(6);

	//POSITION: ?
	std::getline(mapFile, line);
	point = new sf::Vector2f(std::stof(line.substr(10)), std::stof(line.substr(line.find_last_of(';') + 1)));
	territoryPositions.emplace(point);

	//create territory
	territory = new Territory(name, point);
	territories.emplace(name, territory);

	//NUMBER OF VERTICES: ?
	std::getline(mapFile, line);
	n = std::stoi(line.substr(20));

	//create shape
	shape = new sf::ConvexShape(n);
	shape->setPosition(*point);
	territory->setShape(shape);

	//load vertices
	for (int v = 0; v < n; ++v)
	{
		std::getline(mapFile, line);
		shape->setPoint(v, sf::Vector2f(std::stof(line), std::stof(line.substr(line.find_last_of(';') + 1))) - *point);
	}

	//NUMBER OF NEIGHBORS: ?
	std::getline(mapFile, line);
	n = std::stoi(line.substr(21));

	//add neighbors if they have been loaded already
	for (int v = 0; v < n; ++v)
	{
		std::getline(mapFile, line);

		if (territories.count(line))
		{
			territory->addNeighbor(territories.at(line));
			territories.at(line)->addNeighbor(territory);
		}
	}

	//NUMBER OF DISTANT NEIGHBORS: ?
	std::getline(mapFile, line);
	n = std::stoi(line.substr(29));

	//add distant neighbors if they have been loaded already
	for (int v = 0; v < n; ++v)
	{
		std::getline(mapFile, line);

		if (territories.count(line))
		{
			territory->addDistantNeighbor(territories.at(line));
			territories.at(line)->addDistantNeighbor(territory);
		}
	}
}

void Map::loadRegion(std::ifstream& mapFile)
{
	std::string line;
	std::string name;
	Region* region;
	int n;

	//NAME: ?
	std::getline(mapFile, line);
	name = line.substr(6);

	//VALUE: ?
	std::getline(mapFile, line);
	region = new Region(name, std::stoi(line.substr(7)));
	regions.emplace(name, region);

	//NUMBER OF TERRITORIES: ?
	std::getline(mapFile, line);
	n = std::stoi(line.substr(23));

	//add territories
	for (int v = 0; v < n; ++v)
	{
		std::getline(mapFile, line);
		region->addTerritory(territories.at(line));
		territories.at(line)->setRegion(region);
	}
}