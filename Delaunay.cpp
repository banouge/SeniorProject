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

	//there can't be edges without at least 2 vertices
	if (numSites > 1)
	{
		std::sort(sites.begin(), sites.end(), compareVector2fPtr);
		triangulate(0, numSites);
	}
}

Delaunay::Delaunay(std::vector<sf::Vector2f*> &siteList, float width, float height) : WIDTH(width), HEIGHT(height)
{
	//make copies of provided sites so that the destructor won't destroy the originals
	for (sf::Vector2f* site : siteList)
	{
		sites.push_back(new sf::Vector2f(site->x, site->y));
	}

	//there can't be edges without at least 2 vertices
	if (sites.size() > 1)
	{
		std::sort(sites.begin(), sites.end(), compareVector2fPtr);
		triangulate(0, sites.size());
	}
}

Delaunay::~Delaunay()
{
	for (sf::Vector2f* site : sites)
	{
		delete site;
	}

	for (DelaunayEdge* edge : edges)
	{
		delete edge;
	}

	sites.clear();
	edges.clear();
}

bool Delaunay::compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b)
{
	return a->x < b->x;
}

//-1 = clockwise, 0 = colinear, 1 = counterclockwise
int Delaunay::getOrientation(sf::Vector2f* a, sf::Vector2f* b, sf::Vector2f* c)
{
	float o = (b->y - a->y) * (c->x - b->x) - (c->y - b->y) * (b->x - a->x);
	return (o < 0.0f) ? (-1) : ((o > 0.0f) ? (1) : (0));
}

//triangulate by recursively partitioning sites then merging using edges on convex hull of partitions (returns edges on covex hull)
std::vector<DelaunayEdge*> Delaunay::triangulate(int firstSiteIndex, int lastSiteIndex)
{
	int numSites = lastSiteIndex - firstSiteIndex + 1;

	//base cases
	if (numSites == 2)
	{
		return triangulate2(firstSiteIndex, lastSiteIndex);
	}
	else if (numSites == 3)
	{
		return triangulate3(firstSiteIndex, lastSiteIndex);
	}

	//partition
	int middleSiteIndex = (lastSiteIndex - firstSiteIndex) / 2 + firstSiteIndex;
	std::vector<DelaunayEdge*> leftEdges = triangulate(firstSiteIndex, middleSiteIndex);
	std::vector<DelaunayEdge*> rightEdges = triangulate(middleSiteIndex + 1, lastSiteIndex);

	//TODO: merge

	//TODO: add edges to edge set once we know they won't be removed

	return std::vector<DelaunayEdge*>();
}

//base case for 3 vertices
std::vector<DelaunayEdge*> Delaunay::triangulate3(int firstSiteIndex, int lastSiteIndex)
{
	std::vector<DelaunayEdge*> newEdges;
	sf::Vector2f* site1;
	sf::Vector2f* site2;
	sf::Vector2f* site3;
	int middleSiteIndex = firstSiteIndex + 1;
	bool isColinear = false;
	bool isFirstBelowMiddle = sites.at(firstSiteIndex)->y < sites.at(middleSiteIndex)->y;
	bool isFirstBelowLast = sites.at(firstSiteIndex)->y < sites.at(lastSiteIndex)->y;
	bool isMiddleBelowLast = sites.at(middleSiteIndex)->y < sites.at(lastSiteIndex)->y;

	//put in clockwise order and start at bottom
	if (isFirstBelowMiddle && isFirstBelowLast)
	{
		site1 = sites.at(firstSiteIndex);

		switch (getOrientation(site1, sites.at(middleSiteIndex), sites.at(lastSiteIndex)))
		{
			case -1:
			{
				site2 = sites.at(middleSiteIndex);
				site3 = sites.at(lastSiteIndex);
				break;
			}
			case 0:
			{
				isColinear = true;
			}
			case 1:
			{
				site2 = sites.at(lastSiteIndex);
				site3 = sites.at(middleSiteIndex);
				break;
			}
		}
	}
	else if (!isFirstBelowMiddle && isMiddleBelowLast)
	{
		//first-middle-last is left-to-right and middle is lowest, so middle-first-last must be clockwise
		site1 = sites.at(middleSiteIndex);
		site2 = sites.at(firstSiteIndex);
		site3 = sites.at(lastSiteIndex);
	}
	else
	{
		site1 = sites.at(lastSiteIndex);

		switch (getOrientation(site1, sites.at(middleSiteIndex), sites.at(firstSiteIndex)))
		{
			case -1:
			{
				site2 = sites.at(middleSiteIndex);
				site3 = sites.at(firstSiteIndex);
				break;
			}
			case 0:
			{
				isColinear = true;
			}
			case 1:
			{
				site2 = sites.at(firstSiteIndex);
				site3 = sites.at(middleSiteIndex);
				break;
			}
		}
	}

	//make edges
	DelaunayEdge* edge1 = new DelaunayEdge(site1, site2);
	DelaunayEdge* edge2 = new DelaunayEdge(site2, site3);
	newEdges.push_back(edge1);
	newEdges.push_back(edge2);

	//last edge only exists if triangle is not degenerate
	if (!isColinear)
	{
		DelaunayEdge* edge3 = new DelaunayEdge(site3, site1);
		newEdges.push_back(edge3);
	}

	return newEdges;
}

//base case for 2 vertices
std::vector<DelaunayEdge*> Delaunay::triangulate2(int firstSiteIndex, int lastSiteIndex)
{
	//start at bottom
	std::vector<DelaunayEdge*> newEdges;
	sf::Vector2f* firstSite = sites.at(firstSiteIndex);
	sf::Vector2f* lastSite = sites.at(lastSiteIndex);
	DelaunayEdge* edge = (firstSite->y < lastSite->y) ? (new DelaunayEdge(firstSite, lastSite)) : (new DelaunayEdge(lastSite, firstSite));
	newEdges.push_back(edge);
	return newEdges;
}