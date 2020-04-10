#include <algorithm>
#include <fstream>
#include <queue>
#include <sstream>
#include "MapGenerator.h"

MapGenerator::MapGenerator(int numCells, int numTerritories, int numRegions, float width, float height, std::string name): noise(Perlin()), delaunay(Delaunay(numCells, width, height))
{
	std::unordered_set<Territory*> disconnectedTerritories;
	std::unordered_set<Territory*> regionSeeds;

	removeUnwantedTerritories((numTerritories > numCells) ? (numCells) : (numTerritories), disconnectedTerritories);
	reconnectMap(disconnectedTerritories, regionSeeds);
	createRegions((numRegions > territories.size()) ? (territories.size()) : (numRegions), regionSeeds);
	outputData(name);
}

MapGenerator::~MapGenerator()
{
	for (Region* region : regions)
	{
		delete region;
	}
}

bool MapGenerator::compareTerritoryPtr(Territory* a, Territory* b)
{
	return a->getHeight() > b->getHeight();
}

std::string MapGenerator::convertNumberToLetters(int number)
{
	std::stringstream stream;
	std::string string = "";
	char character = '!';

	while (number-- > 0)
	{
		stream << (char)('A' + number % 26);
		number /= 26;
	}

	stream >> string;
	reverse(string.begin(), string.end());
	return string;
}

void MapGenerator::outputData(std::string name)
{
	std::ofstream mapFile("Maps/" + name + ".txt");
	std::fstream mapsList("Maps.txt");
	std::string line;

	mapFile << "NUMBER OF TERRITORIES: " << std::to_string(territories.size()) << '\n';

	for (Territory* territory : territories)
	{
		territory->writeToOutput(mapFile);
	}

	mapFile << "NUMBER OF REGIONS: " << std::to_string(regions.size()) << '\n';

	for (Region* region : regions)
	{
		region->writeToOutput(mapFile);
	}

	//check if name is already in list
	while (std::getline(mapsList, line))
	{
		if (line == name)
		{
			return;
		}
	}

	//put name in list if it's not there already
	mapsList.clear();
	mapsList.seekp(0, std::ios_base::end);
	mapsList << name << '\n';
}

void MapGenerator::createRegions(int numRegions, std::unordered_set<Territory*>& regionSeeds)
{
	auto iterator = territories.begin();

	//pick more seeds if necessary
	while (regionSeeds.size() < numRegions)
	{
		if (!regionSeeds.count(*iterator))
		{
			regionSeeds.emplace(*iterator);
		}

		++iterator;
	}

	//create regions using seeds
	while (regions.size() < numRegions)
	{
		Territory* seed = *regionSeeds.begin();
		Region* region = new Region(convertNumberToLetters(regions.size() + 1));
		region->addTerritory(seed);
		seed->setRegion(region);
		seed->setName(region->NAME + std::to_string(region->getTerritories().size()));
		regionSeeds.erase(seed);
		regions.emplace(region);
	}

	expandRegions();
}

void MapGenerator::expandRegions()
{
	std::unordered_map<Region*, std::queue<Territory*>*> queues;
	std::unordered_map<Region*, std::unordered_set<Territory*>*> friendlyBorders;
	std::unordered_map<Region*, std::unordered_set<Territory*>*> hostileBorders;
	
	//initialize
	for (Region* region : regions)
	{
		std::queue<Territory*>* queue = new std::queue<Territory*>();
		std::unordered_set<Territory*>* friendlyBorder = new std::unordered_set<Territory*>();
		std::unordered_set<Territory*>* hostileBorder = new std::unordered_set<Territory*>();

		queues.emplace(region, queue);
		friendlyBorders.emplace(region, friendlyBorder);
		hostileBorders.emplace(region, hostileBorder);

		queue->push(*region->getTerritories().begin());
	}

	//expand regions
	while (!queues.empty())
	{
		std::unordered_set<Region*> completedRegions;

		//process one from each queue
		for (std::pair<Region*, std::queue<Territory*>*> pair : queues)
		{
			Region* region = pair.first;
			std::queue<Territory*>* queue = pair.second;
			Territory* territory = queue->front();
			queue->pop();

			//get connected territories
			for (Territory* neighbor : territory->getNeighbors())
			{
				//check if neighbor is part of a region
				if (neighbor->getRegion())
				{
					//check if neighbor is part of another region
					if (neighbor->getRegion() != region)
					{
						//update borders
						friendlyBorders.at(region)->emplace(territory);
						hostileBorders.at(region)->emplace(neighbor);
						friendlyBorders.at(neighbor->getRegion())->emplace(neighbor);
						hostileBorders.at(neighbor->getRegion())->emplace(territory);
					}
				}
				else
				{
					//update region
					region->addTerritory(neighbor);
					neighbor->setRegion(region);
					neighbor->setName(region->NAME + std::to_string(region->getTerritories().size()));
					queue->push(neighbor);
				}
			}

			if (queue->empty())
			{
				completedRegions.emplace(region);
			}
		}

		for (Region* region : completedRegions)
		{
			std::queue<Territory*>* queue = queues.at(region);
			queues.erase(region);
			delete queue;
		}
	}

	//set values and clean up
	for (Region* region : regions)
	{
		std::unordered_set<Territory*>* friendlyBorder = friendlyBorders.at(region);
		std::unordered_set<Territory*>* hostileBorder = hostileBorders.at(region);

		region->setValue(region->getTerritories().size() + friendlyBorder->size() / 2 + hostileBorder->size() / 4 - 1);

		friendlyBorders.erase(region);
		hostileBorders.erase(region);
		delete friendlyBorder;
		delete hostileBorder;
	}
}

void MapGenerator::removeUnwantedTerritories(int numTerritories, std::unordered_set<Territory*>& disconnectedTerritories)
{
	//set height of territories
	for (Territory* territory : delaunay.getVoronoiCells())
	{
		territory->setHeight(noise.noise(territory->POSITION->x, territory->POSITION->y, 0.0f));
	}

	std::sort(delaunay.getVoronoiCells().begin(), delaunay.getVoronoiCells().end(), compareTerritoryPtr);

	//get top territories
	for (int t = 0; t < numTerritories; ++t)
	{
		disconnectedTerritories.emplace(delaunay.getVoronoiCells().at(t));
	}

	//remove connections to ignored cells
	for (Territory* territory : disconnectedTerritories)
	{
		std::unordered_set<Territory*> ignoredNeighbors;

		for (Territory* neighbor : territory->getNeighbors())
		{
			if (!disconnectedTerritories.count(neighbor))
			{
				ignoredNeighbors.emplace(neighbor);
			}
		}

		for (Territory* neighbor : ignoredNeighbors)
		{
			territory->removeNeighbor(neighbor);
		}
	}
}

void MapGenerator::reconnectMap(std::unordered_set<Territory*>& disconnectedTerritories, std::unordered_set<Territory*>& regionSeeds)
{
	//connection won't form because there is only one connected component so far
	getNewTerritoryConnection(*disconnectedTerritories.begin(), disconnectedTerritories);

	//make sure map is connected
	while (disconnectedTerritories.size())
	{
		std::pair<Territory*, Territory*> connection = getNewTerritoryConnection(*disconnectedTerritories.begin(), disconnectedTerritories);
		connection.first->addDistantNeighbor(connection.second);
		connection.second->addDistantNeighbor(connection.first);
		regionSeeds.emplace(connection.first);
		regionSeeds.emplace(connection.second);
	}
}

std::pair<Territory*, Territory*> MapGenerator::getNewTerritoryConnection(Territory* territory, std::unordered_set<Territory*>& disconnectedTerritories)
{
	std::unordered_set<Territory*> connectedTerritories;
	std::queue<Territory*> queue;
	std::pair<Territory*, Territory*> connection;
	float squareDist = FLT_MAX;
	connectedTerritories.emplace(territory);
	disconnectedTerritories.erase(territory);
	queue.push(territory);

	while (!queue.empty())
	{
		territory = queue.front();
		queue.pop();

		//get connection
		for (Territory* acceptedTerritory : territories)
		{
			float dX = acceptedTerritory->POSITION->x - territory->POSITION->x;
			float dY = acceptedTerritory->POSITION->y - territory->POSITION->y;
			float newSquareDist = dX * dX + dY * dY;

			if (newSquareDist < squareDist)
			{
				squareDist = newSquareDist;
				connection = std::pair<Territory*, Territory*>(territory, acceptedTerritory);
			}
		}

		//get connected territories
		for (Territory* neighbor : territory->getNeighbors())
		{
			if (!connectedTerritories.count(neighbor))
			{
				connectedTerritories.emplace(neighbor);
				disconnectedTerritories.erase(neighbor);
				queue.push(neighbor);
			}
		}
	}

	for (Territory* acceptedTerritory : connectedTerritories)
	{
		territories.emplace(acceptedTerritory);
	}

	return connection;
}