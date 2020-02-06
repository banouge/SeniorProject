#include <algorithm>
#include <random>
#include "Delaunay.h"

//REFERENCES:
//http://www.geom.uiuc.edu/~samuelp/del_project.html

Delaunay::Delaunay(int numSites, float width, float height) : WIDTH(width), HEIGHT(height)
{
	//set up rng
	std::random_device seed;
	std::mt19937 rng(seed());
	std::uniform_real_distribution<float> xDistribution = std::uniform_real_distribution<float>(0.0f, WIDTH);
	std::uniform_real_distribution<float> yDistribution = std::uniform_real_distribution<float>(0.0f, HEIGHT);

	//generate sites
	for (int s = 0; s < numSites; ++s)
	{
		sites.push_back(new sf::Vector2f(xDistribution(rng), yDistribution(rng)));
	}

	std::sort(sites.begin(), sites.end(), compareVector2fPtr);
	triangulate(0, sites.size());
}

Delaunay::Delaunay(std::vector<sf::Vector2f*> &siteList, float width, float height) : WIDTH(width), HEIGHT(height)
{
	//make copies of provided sites so that the destructor won't destroy the originals
	for (sf::Vector2f* site : siteList)
	{
		sites.push_back(new sf::Vector2f(site->x, site->y));
	}

	std::sort(sites.begin(), sites.end(), compareVector2fPtr);
	triangulate(0, sites.size());
}

Delaunay::~Delaunay()
{
	for (sf::Vector2f* site : sites)
	{
		delete site;
	}

	sites.clear();
}

bool Delaunay::compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b)
{
	return a->x < b->x;
}

//triangulate by recursively partitioning sites then merging using edges on convex hull of partitions (returns edges on covex hull)
std::vector<std::tuple<sf::Vector2f*, sf::Vector2f*>> Delaunay::triangulate(int firstSiteIndex, int lastSiteIndex)
{
	//TODO: handle base cases, partition, merge
	return std::vector<std::tuple<sf::Vector2f*, sf::Vector2f*>>();
}