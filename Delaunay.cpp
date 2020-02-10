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

//a-b-c should be counterclockwise
bool Delaunay::isOutsideCircumcircle(sf::Vector2f* a, sf::Vector2f* b, sf::Vector2f* c, sf::Vector2f* p)
{
	//matrix elements
	float ax = a->x;
	float ay = a->y;
	float az = ax * ax + ay * ay;
	float bx = b->x;
	float by = b->y;
	float bz = bx * bx + by * by;
	float cx = c->x;
	float cy = c->y;
	float cz = cx * cx + cy * cy;
	float px = p->x;
	float py = p->y;
	float pz = px * px + py * py;

	//determinants
	float detABC = ax * (by * cz - cy * bz) - bx * (ay * cz - cy * az) + cx * (ay * bz - by * az);
	float detABP = ax * (by * pz - py * bz) - bx * (ay * pz - py * az) + px * (ay * bz - by * az);
	float detACP = ax * (cy * pz - py * cz) - cx * (ay * pz - py * az) + px * (ay * cz - cy * az);
	float detBCP = bx * (cy * pz - py * cz) - cx * (by * pz - py * bz) + px * (by * cz - cy * bz);
	float det = detABC - detABP + detACP - detBCP;

	//- = out, 0 = on, + = in
	return det < 0.0f;
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

	//TODO: we only directly access first and last edges in vectors, maybe only return those instead of whole vector?
	//TODO: zip, reverse final new edge (it will be <- but should be -> to be cw), get new hull (bottom-left-top-right or left-top-right-bottom)
	return newHull;
}

Delaunay::Edge* Delaunay::getLeftCandidate(Edge* baseEdge)
{
	Edge* candidate = baseEdge;
	Edge* nextCandidate = baseEdge->ccwAroundDestination;
	sf::Vector2f* pivot = baseEdge->destination;
	sf::Vector2f* nonPivot = baseEdge->origin;
	sf::Vector2f* nextNonPivot = (nextCandidate->origin == pivot) ? (nextCandidate->destination) : (nextCandidate->origin);

	while (true)
	{
		candidate = nextCandidate;
		nonPivot = nextNonPivot;

		//make sure angle < 180 degrees
		if (getOrientation(baseEdge->destination, baseEdge->origin, nonPivot) != 1)
		{
			//we rotated too far w/o finding a valid candidate
			return nullptr;
		}

		//rotate ccw around pivot and get next candidate's non-pivot endpoint
		nextCandidate = (nextCandidate->origin == pivot) ? (nextCandidate->ccwAroundOrigin) : (nextCandidate->ccwAroundDestination);
		nextNonPivot = (nextCandidate->origin == pivot) ? (nextCandidate->destination) : (nextCandidate->origin);

		//if next candidate is base edge then there is no nextNonPivot to get in the way
		if (nextCandidate == baseEdge || isOutsideCircumcircle(baseEdge->destination, baseEdge->origin, nonPivot, nextNonPivot))
		{
			return candidate;
		}
		else
		{
			//candidate is no longer a valid edge because circumcircle is not empty
			removeEdge(candidate);
		}
	}
}

Delaunay::Edge* Delaunay::getRightCandidate(Edge* baseEdge)
{
	Edge* candidate = baseEdge;
	Edge* nextCandidate = baseEdge->cwAroundOrigin;
	sf::Vector2f* pivot = baseEdge->origin;
	sf::Vector2f* nonPivot = baseEdge->destination;
	sf::Vector2f* nextNonPivot = (nextCandidate->origin == pivot) ? (nextCandidate->destination) : (nextCandidate->origin);

	while (true)
	{
		candidate = nextCandidate;
		nonPivot = nextNonPivot;

		//make sure angle < 180 degrees
		if (getOrientation(baseEdge->destination, baseEdge->origin, nonPivot) != 1)
		{
			//we rotated too far w/o finding a valid candidate
			return nullptr;
		}

		//rotate cw around pivot and get next candidate's non-pivot endpoint
		nextCandidate = (nextCandidate->origin == pivot) ? (nextCandidate->cwAroundOrigin) : (nextCandidate->cwAroundDestination);
		nextNonPivot = (nextCandidate->origin == pivot) ? (nextCandidate->destination) : (nextCandidate->origin);

		//if next candidate is base edge then there is no nextNonPivot to get in the way
		if (nextCandidate == baseEdge || isOutsideCircumcircle(baseEdge->destination, baseEdge->origin, nonPivot, nextNonPivot))
		{
			return candidate;
		}
		else
		{
			//candidate is no longer a valid edge because circumcircle is not empty
			removeEdge(candidate);
		}
	}
}

void Delaunay::removeEdge(Edge* edge)
{
	//point edge->cwAroundDestination to edge->ccwAroundDestination
	if (edge->cwAroundDestination->origin == edge->destination)
	{
		edge->cwAroundDestination->ccwAroundOrigin = edge->ccwAroundDestination;
	}
	else
	{
		edge->cwAroundDestination->ccwAroundDestination = edge->ccwAroundDestination;
	}

	//point edge->ccwAroundDestination to edge->cwAroundDestination
	if (edge->ccwAroundDestination->origin == edge->destination)
	{
		edge->ccwAroundDestination->cwAroundOrigin = edge->cwAroundDestination;
	}
	else
	{
		edge->ccwAroundDestination->cwAroundDestination = edge->cwAroundDestination;
	}

	//point edge->cwAroundOrigin to edge->ccwAroundOrigin
	if (edge->cwAroundOrigin->origin == edge->origin)
	{
		edge->cwAroundOrigin->ccwAroundOrigin = edge->ccwAroundOrigin;
	}
	else
	{
		edge->cwAroundOrigin->ccwAroundDestination = edge->ccwAroundOrigin;
	}

	//point edge->ccwAroundOrigin to edge->cwAroundOrigin
	if (edge->ccwAroundOrigin->origin == edge->origin)
	{
		edge->ccwAroundOrigin->cwAroundOrigin = edge->cwAroundOrigin;
	}
	else
	{
		edge->ccwAroundOrigin->cwAroundDestination = edge->cwAroundOrigin;
	}

	edges.erase(edge);
	delete edge;
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