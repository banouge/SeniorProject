#include <algorithm>
#include <random>
#include "Delaunay.h"

//REFERENCES:
//http://www.geom.uiuc.edu/~samuelp/del_project.html
//http://www.sccg.sk/~samuelcik/dgs/quad_edge.pdf

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
		triangulate(0, numSites - 1);
	}
}

Delaunay::Delaunay(std::vector<sf::Vector2f*> siteList, float width, float height) : WIDTH(width), HEIGHT(height)
{
	sites = siteList;

	//there can't be edges without at least 2 vertices
	if (sites.size() > 1)
	{
		std::sort(sites.begin(), sites.end(), compareVector2fPtr);
		triangulate(0, sites.size() - 1);
	}
}

Delaunay::~Delaunay()
{
	for (sf::Vector2f* site : sites)
	{
		delete site;
	}

	for (Edge* edge : edges)
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
	float o = (c->y - b->y) * (b->x - a->x) - (b->y - a->y) * (c->x - b->x);
	return (o < 0.0f) ? (-1) : ((o > 0.0f) ? (1) : (0));
}

//triangulate by recursively partitioning sites then merging using edges on convex hull of partitions (returns edges on covex hull)
std::vector<Delaunay::Edge*> Delaunay::triangulate(int firstSiteIndex, int lastSiteIndex)
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
	std::vector<Edge*> leftEdges = triangulate(firstSiteIndex, middleSiteIndex);
	std::vector<Edge*> rightEdges = triangulate(middleSiteIndex + 1, lastSiteIndex);

	//merge
	return merge(leftEdges, rightEdges);
}

//base case for 3 vertices
std::vector<Delaunay::Edge*> Delaunay::triangulate3(int firstSiteIndex, int lastSiteIndex)
{
	std::vector<Edge*> newEdges;
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
			case 1:
			{
				site2 = sites.at(lastSiteIndex);
				site3 = sites.at(middleSiteIndex);
				break;
			}
			case 0:
			{
				isColinear = true;
			}
			case -1:
			{
				site2 = sites.at(middleSiteIndex);
				site3 = sites.at(lastSiteIndex);
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
			case 1:
			{
				site2 = sites.at(firstSiteIndex);
				site3 = sites.at(middleSiteIndex);
				break;
			}
			case 0:
			{
				isColinear = true;
			}
			case -1:
			{
				site2 = sites.at(middleSiteIndex);
				site3 = sites.at(firstSiteIndex);
				break;
			}
		}
	}

	//make edges
	Edge* edge1 = new Edge(site1, site2);
	Edge* edge2 = new Edge(site2, site3);
	newEdges.push_back(edge1);
	newEdges.push_back(edge2);
	edges.emplace(edge1);
	edges.emplace(edge2);

	//fill in neighbors
	edge1->ccwAroundDestination = edge2;
	edge1->cwAroundDestination = edge2;
	edge2->ccwAroundOrigin = edge1;
	edge2->cwAroundOrigin = edge1;

	if (!isColinear)
	{
		//last edge only exists if triangle is not degenerate
		Edge* edge3 = new Edge(site3, site1);
		newEdges.push_back(edge3);
		edges.emplace(edge3);
		
		//fill in neighbors
		edge1->ccwAroundOrigin = edge3;
		edge1->cwAroundOrigin = edge3;
		edge2->ccwAroundDestination = edge3;
		edge2->cwAroundDestination = edge3;
		edge3->ccwAroundOrigin = edge2;
		edge3->cwAroundOrigin = edge2;
		edge3->ccwAroundDestination = edge1;
		edge3->cwAroundDestination = edge1;
	}
	else
	{
		//fill in neighbors
		edge1->ccwAroundOrigin = edge1;
		edge1->cwAroundOrigin = edge1;
		edge2->ccwAroundDestination = edge2;
		edge2->cwAroundDestination = edge2;
	}

	return newEdges;
}

//base case for 2 vertices
std::vector<Delaunay::Edge*> Delaunay::triangulate2(int firstSiteIndex, int lastSiteIndex)
{
	//start at bottom
	std::vector<Edge*> newEdges;
	sf::Vector2f* firstSite = sites.at(firstSiteIndex);
	sf::Vector2f* lastSite = sites.at(lastSiteIndex);
	Edge* edge = (firstSite->y < lastSite->y) ? (new Edge(firstSite, lastSite)) : (new Edge(lastSite, firstSite));

	//fill in neighbors
	edge->ccwAroundOrigin = edge;
	edge->cwAroundOrigin = edge;
	edge->ccwAroundDestination = edge;
	edge->cwAroundDestination = edge;
	newEdges.push_back(edge);
	edges.emplace(edge);
	return newEdges;
}

std::vector<Delaunay::Edge*> Delaunay::merge(std::vector<Edge*> leftEdges, std::vector<Edge*> rightEdges)
{
	//make base edge from bottom vertices of each side
	std::vector<Edge*> newHull;
	Edge* baseEdge = new Edge(rightEdges.at(0)->origin, leftEdges.at(0)->origin);
	Edge* bottomEdge = baseEdge;
	edges.emplace(baseEdge);

	//fill in neighbors
	baseEdge->ccwAroundOrigin = rightEdges.at(rightEdges.size() - 1);
	baseEdge->cwAroundOrigin = rightEdges.at(0);
	baseEdge->ccwAroundDestination = leftEdges.at(leftEdges.size() - 1);
	baseEdge->cwAroundDestination = leftEdges.at(0);
	leftEdges.at(0)->ccwAroundOrigin = baseEdge;
	leftEdges.at(leftEdges.size() - 1)->cwAroundDestination = baseEdge;
	rightEdges.at(0)->ccwAroundOrigin = baseEdge;
	rightEdges.at(rightEdges.size() - 1)->cwAroundDestination = baseEdge;

	//TODO: zip (need circumcircle checker), get new hull (bottom-left-top-right or left-top-right-bottom)
	return newHull;
}

Delaunay::Edge::Edge(sf::Vector2f* o, sf::Vector2f* d, Edge* ccwo, Edge* cwo, Edge* ccwd, Edge* cwd)
{
	origin = o;
	destination = d;
	ccwAroundOrigin = ccwo;
	cwAroundOrigin = cwo;
	ccwAroundDestination = ccwd;
	cwAroundDestination = cwd;
}