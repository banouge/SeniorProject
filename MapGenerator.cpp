#include <algorithm>
#include "MapGenerator.h"

MapGenerator::MapGenerator(int numCells, int numTerritories, float width, float height): noise(Perlin()), delaunay(Delaunay({new sf::Vector2f(103.842392,494.779510),new sf::Vector2f(165.291687,872.400146),new sf::Vector2f(200.237061,321.416565) ,new sf::Vector2f(335.767578,822.446960) ,new sf::Vector2f(495.841187,966.925537) ,new sf::Vector2f(1038.48242,171.301941) ,new sf::Vector2f(1071.38623,735.666382) ,new sf::Vector2f(1379.79834,422.512054) ,new sf::Vector2f(1549.10046,217.614532) ,new sf::Vector2f(1676.82556,975.670715) }, width, height)) //FIXME
{
	numTerritories = (numTerritories > numCells) ? (numCells) : (numTerritories);

	//set height of territories
	for (Territory* territory : delaunay.getVoronoiCells())
	{
		territory->setHeight(noise.noise(territory->POSITION->x, territory->POSITION->y, 0.0f));
	}

	std::sort(delaunay.getVoronoiCells().begin(), delaunay.getVoronoiCells().end(), compareTerritoryPtr);

	//get top territories
	for (int t = 0; t < numTerritories; ++t)
	{
		territories.emplace(delaunay.getVoronoiCells().at(t));
	}

	//remove connections to ignored cells
	for (Territory* territory : territories)
	{
		std::unordered_set<Territory*> ignoredNeighbors;

		for (Territory* neighbor : territory->getNeighbors())
		{
			if (!territories.count(neighbor))
			{
				ignoredNeighbors.emplace(neighbor);
			}
		}

		for (Territory* neighbor : ignoredNeighbors)
		{
			territory->removeNeighbor(neighbor);
		}
	}

	//TODO: make sure map is connected
}

MapGenerator::~MapGenerator()
{
}

bool MapGenerator::compareTerritoryPtr(Territory* a, Territory* b)
{
	return a->getHeight() > b->getHeight();
}