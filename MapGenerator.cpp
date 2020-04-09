#include <algorithm>
#include <queue>
#include "MapGenerator.h"

MapGenerator::MapGenerator(int numCells, int numTerritories, float width, float height): noise(Perlin()), delaunay(Delaunay({new sf::Vector2f(103.842392,494.779510),new sf::Vector2f(165.291687,872.400146),new sf::Vector2f(200.237061,321.416565) ,new sf::Vector2f(335.767578,822.446960) ,new sf::Vector2f(495.841187,966.925537) ,new sf::Vector2f(1038.48242,171.301941) ,new sf::Vector2f(1071.38623,735.666382) ,new sf::Vector2f(1379.79834,422.512054) ,new sf::Vector2f(1549.10046,217.614532) ,new sf::Vector2f(1676.82556,975.670715) }, width, height)) //FIXME
{
	std::unordered_set<Territory*> disconnectedTerritories;
	removeUnwantedTerritories((numTerritories > numCells) ? (numCells) : (numTerritories), disconnectedTerritories);
	reconnectMap(disconnectedTerritories);
}

MapGenerator::~MapGenerator()
{
}

bool MapGenerator::compareTerritoryPtr(Territory* a, Territory* b)
{
	return a->getHeight() > b->getHeight();
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

void MapGenerator::reconnectMap(std::unordered_set<Territory*>& disconnectedTerritories)
{
	//connection won't form because there is only one connected component so far
	getNewTerritoryConnection(*disconnectedTerritories.begin(), disconnectedTerritories);

	//make sure map is connected
	while (disconnectedTerritories.size())
	{
		std::pair<Territory*, Territory*> connection = getNewTerritoryConnection(*disconnectedTerritories.begin(), disconnectedTerritories);
		connection.first->addDistantNeighbor(connection.second);
		connection.second->addDistantNeighbor(connection.first);
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