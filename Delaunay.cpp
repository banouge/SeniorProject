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
		std::pair<Delaunay::Edge*, Delaunay::Edge*> bottomEdges = triangulate(0, numSites - 1);
		createVoronoiCells(bottomEdges);
	}
}

Delaunay::Delaunay(std::vector<sf::Vector2f*> siteList, float width, float height) : WIDTH(width), HEIGHT(height)
{
	sites = siteList;

	//there can't be edges without at least 2 vertices
	if (sites.size() > 1)
	{
		std::sort(sites.begin(), sites.end(), compareVector2fPtr);
		std::pair<Delaunay::Edge*, Delaunay::Edge*> bottomEdges = triangulate(0, sites.size() - 1);
		createVoronoiCells(bottomEdges);
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

	for (Territory* cell : voronoiCells)
	{
		delete cell;
	}

	for (sf::Vector2f* vertex : voronoiVertices)
	{
		delete vertex;
	}
}

std::vector<Territory*>& Delaunay::getVoronoiCells()
{
	return voronoiCells;
}

bool Delaunay::compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b)
{
	return (a->x == b->x) ? (a->y < b->y) : (a->x < b->x);
}

bool Delaunay::compareVector2fPtrFloatPairPtr(std::pair<sf::Vector2f*, float>* a, std::pair<sf::Vector2f*, float>* b)
{
	return a->second < b->second;
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
std::pair<Delaunay::Edge*, Delaunay::Edge*> Delaunay::triangulate(int firstSiteIndex, int lastSiteIndex)
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
	std::pair<Edge*, Edge*> leftEdges = triangulate(firstSiteIndex, middleSiteIndex);
	std::pair<Edge*, Edge*> rightEdges = triangulate(middleSiteIndex + 1, lastSiteIndex);

	//merge
	return merge(leftEdges, rightEdges);
}

//base case for 3 vertices
std::pair<Delaunay::Edge*, Delaunay::Edge*> Delaunay::triangulate3(int firstSiteIndex, int lastSiteIndex)
{
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
			default:
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
			default:
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
	Edge* edge2 = new Edge(site2, site3, edge1, edge1);
	edges.emplace(edge1);
	edges.emplace(edge2);

	//fill in neighbors
	edge1->ccwAroundDestination = edge2;
	edge1->cwAroundDestination = edge2;

	if (!isColinear)
	{
		//last edge only exists if triangle is not degenerate
		Edge* edge3 = new Edge(site3, site1, edge2, edge2, edge1, edge1);
		edges.emplace(edge3);
		
		//fill in neighbors
		edge1->ccwAroundOrigin = edge3;
		edge1->cwAroundOrigin = edge3;
		edge2->ccwAroundDestination = edge3;
		edge2->cwAroundDestination = edge3;
		
		//return edges on left and right of bottom vertex
		return std::pair<Edge*, Edge*>(edge1, edge3);
	}
	else
	{
		//fill in neighbors
		edge1->ccwAroundOrigin = edge1;
		edge1->cwAroundOrigin = edge1;
		edge2->ccwAroundDestination = edge2;
		edge2->cwAroundDestination = edge2;

		//return bottom edge twice
		return std::pair<Edge*, Edge*>(edge1, edge1);
	}
}

//base case for 2 vertices
std::pair<Delaunay::Edge*, Delaunay::Edge*> Delaunay::triangulate2(int firstSiteIndex, int lastSiteIndex)
{
	//start at bottom
	sf::Vector2f* firstSite = sites.at(firstSiteIndex);
	sf::Vector2f* lastSite = sites.at(lastSiteIndex);
	Edge* edge = (firstSite->y < lastSite->y) ? (new Edge(firstSite, lastSite)) : (new Edge(lastSite, firstSite));

	//fill in neighbors
	edge->ccwAroundOrigin = edge;
	edge->cwAroundOrigin = edge;
	edge->ccwAroundDestination = edge;
	edge->cwAroundDestination = edge;
	edges.emplace(edge);

	//return only edge twice
	return std::pair<Edge*, Edge*>(edge, edge);
}

std::pair<Delaunay::Edge*, Delaunay::Edge*> Delaunay::merge(std::pair<Edge*, Edge*> leftEdges, std::pair<Edge*, Edge*> rightEdges)
{
	//make base edge from bottom vertices of each side
	Edge* baseEdge = createBottomEdge(leftEdges, rightEdges);
	Edge* bottomEdge = baseEdge;

	//zip (make each base edge <-)
	while (true)
	{
		Edge* leftCandidate = getLeftCandidate(baseEdge);
		Edge* rightCandidate = getRightCandidate(baseEdge);
		sf::Vector2f* leftEndpoint = nullptr;
		sf::Vector2f* rightEndpoint = nullptr;

		if (leftCandidate)
		{
			leftEndpoint = (leftCandidate->origin == baseEdge->destination) ? (leftCandidate->destination) : (leftCandidate->origin);
		}

		if (rightCandidate)
		{
			rightEndpoint = (rightCandidate->origin == baseEdge->origin) ? (rightCandidate->destination) : (rightCandidate->origin);
		}

		//make sure candidate exists and other candidate endpoint isn't in circumcircle (impossible to have both violate each other's circumcircles)
		bool isLeftWinner = leftCandidate && (!rightCandidate || isOutsideCircumcircle(baseEdge->destination, baseEdge->origin, leftEndpoint, rightEndpoint));
		bool isRightWinner = rightCandidate && !isLeftWinner;

		if (isLeftWinner)
		{
			//make triangle with baseEdge and leftCandidate and make new edge new base edge
			baseEdge = addEdgeUsingLeftCandidate(baseEdge, leftCandidate, leftEndpoint);
		}
		else if (isRightWinner)
		{
			//make triangle with baseEdge and rightCandidate and make new edge new base edge
			baseEdge = addEdgeUsingRightCandidate(baseEdge, rightCandidate, rightEndpoint);
		}
		else
		{
			//done because there are no candidates left
			break;
		}
	}

	//reverse top edge to keep outer edges clockwise
	reverseEdge(baseEdge);

	//return edges on left and right of bottom vertex
	bool isRightLower = bottomEdge->origin->y < bottomEdge->destination->y;
	return (isRightLower) ? (std::pair<Edge*, Edge*>(bottomEdge, bottomEdge->ccwAroundOrigin)) : (std::pair<Edge*, Edge*>(bottomEdge->cwAroundDestination, bottomEdge));
}

Delaunay::Edge* Delaunay::createBottomEdge(std::pair<Edge*, Edge*>& leftEdges, std::pair<Edge*, Edge*>& rightEdges)
{
	bool areVerticesExtreme = false;

	//find extreme vertices (all vertices should be on one side of the resulting edge)
	while (!areVerticesExtreme)
	{
		areVerticesExtreme = true;

		//shift to vertex that is on the wrong side (eventually all will be on right side)
		if (getOrientation(leftEdges.first->origin, rightEdges.second->destination, leftEdges.first->destination) < 0)
		{
			areVerticesExtreme = false;
			
			if (leftEdges.first == leftEdges.second)
			{
				leftEdges.first = reverseEdge(leftEdges.first);
				leftEdges.second = leftEdges.first;
			}
			else
			{
				leftEdges.second = leftEdges.first;
				leftEdges.first = leftEdges.first->cwAroundDestination;
			}
		}
		else if (getOrientation(leftEdges.first->origin, rightEdges.second->destination, leftEdges.second->origin) < 0)
		{
			areVerticesExtreme = false;
			
			if (leftEdges.first == leftEdges.second)
			{
				leftEdges.first = reverseEdge(leftEdges.first);
				leftEdges.second = leftEdges.first;
			}
			else
			{
				leftEdges.first = leftEdges.second;
				leftEdges.second = leftEdges.second->ccwAroundOrigin;
			}
		}
		else if (getOrientation(leftEdges.first->origin, rightEdges.second->destination, rightEdges.first->destination) < 0)
		{
			areVerticesExtreme = false;
			
			if (rightEdges.first == rightEdges.second)
			{
				rightEdges.first = reverseEdge(rightEdges.first);
				rightEdges.second = rightEdges.first;
			}
			else
			{
				rightEdges.second = rightEdges.first;
				rightEdges.first = rightEdges.first->cwAroundDestination;
			}
		}
		else if (getOrientation(leftEdges.first->origin, rightEdges.second->destination, rightEdges.second->origin) < 0)
		{
			areVerticesExtreme = false;
			
			if (rightEdges.first == rightEdges.second)
			{
				rightEdges.first = reverseEdge(rightEdges.first);
				rightEdges.second = rightEdges.first;
			}
			else
			{
				rightEdges.first = rightEdges.second;
				rightEdges.second = rightEdges.second->ccwAroundOrigin;
			}
		}
	}

	//make base edge from extreme vertices of each side
	Edge* baseEdge = new Edge(rightEdges.second->destination, leftEdges.first->origin, rightEdges.second, rightEdges.first, leftEdges.second, leftEdges.first);
	edges.emplace(baseEdge);

	//add base edge as neighbor of left edges
	if (leftEdges.first == leftEdges.second)
	{
		//base edge only touches origin
		leftEdges.first->ccwAroundOrigin = baseEdge;
		leftEdges.first->cwAroundOrigin = baseEdge;
	}
	else
	{
		leftEdges.first->ccwAroundOrigin = baseEdge;
		leftEdges.second->cwAroundDestination = baseEdge;
	}

	//add base edge as neighbor of right edges
	if (rightEdges.first == rightEdges.second)
	{
		//base edge only touches destination
		rightEdges.second->ccwAroundDestination = baseEdge;
		rightEdges.second->cwAroundDestination = baseEdge;
	}
	else
	{
		rightEdges.first->ccwAroundOrigin = baseEdge;
		rightEdges.second->cwAroundDestination = baseEdge;
	}

	return baseEdge;
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

Delaunay::Edge* Delaunay::addEdgeUsingLeftCandidate(Edge* baseEdge, Edge* candidateEdge, sf::Vector2f* endpoint)
{
	//get neighboring edge info
	bool isCandidateDestinationSame = candidateEdge->destination == endpoint;
	Edge* leftEdge = (isCandidateDestinationSame) ? (candidateEdge->ccwAroundDestination) : (candidateEdge->ccwAroundOrigin);
	Edge* rightEdge = baseEdge->cwAroundOrigin;
	bool isLeftDestinationSame = leftEdge->destination == endpoint;
	bool isRightOriginSame = rightEdge->origin == baseEdge->origin;

	//create new edge
	Edge* newEdge = new Edge(baseEdge->origin, endpoint, baseEdge, rightEdge, leftEdge, candidateEdge);
	edges.emplace(newEdge);
	
	//add new to left
	if (isLeftDestinationSame)
	{
		leftEdge->cwAroundDestination = newEdge;
	}
	else
	{
		leftEdge->cwAroundOrigin = newEdge;
	}

	//add new to candidate
	if (isCandidateDestinationSame)
	{
		candidateEdge->ccwAroundDestination = newEdge;
	}
	else
	{
		candidateEdge->ccwAroundOrigin = newEdge;
	}

	//add new to right
	if (isRightOriginSame)
	{
		rightEdge->ccwAroundOrigin = newEdge;
	}
	else
	{
		rightEdge->ccwAroundDestination = newEdge;
	}

	//add new to base
	baseEdge->cwAroundOrigin = newEdge;

	//return new so that it can become the new base
	return newEdge;
}

Delaunay::Edge* Delaunay::addEdgeUsingRightCandidate(Edge* baseEdge, Edge* candidateEdge, sf::Vector2f* endpoint)
{
	//get neighboring edge info
	bool isCandidateOriginSame = candidateEdge->origin == endpoint;
	Edge* leftEdge = baseEdge->ccwAroundDestination;
	Edge* rightEdge = (isCandidateOriginSame) ? (candidateEdge->cwAroundOrigin) : (candidateEdge->cwAroundDestination);
	bool isLeftDestinationSame = leftEdge->destination == baseEdge->destination;
	bool isRightOriginSame = rightEdge->origin == endpoint;

	//create new edge
	Edge* newEdge = new Edge(endpoint, baseEdge->destination, candidateEdge, rightEdge, leftEdge, baseEdge);
	edges.emplace(newEdge);

	//add new to left
	if (isLeftDestinationSame)
	{
		leftEdge->cwAroundDestination = newEdge;
	}
	else
	{
		leftEdge->cwAroundOrigin = newEdge;
	}

	//add new to candidate
	if (isCandidateOriginSame)
	{
		candidateEdge->cwAroundOrigin = newEdge;
	}
	else
	{
		candidateEdge->cwAroundDestination = newEdge;
	}

	//add new to right
	if (isRightOriginSame)
	{
		rightEdge->ccwAroundOrigin = newEdge;
	}
	else
	{
		rightEdge->ccwAroundDestination = newEdge;
	}

	//add new to base
	baseEdge->ccwAroundDestination = newEdge;

	//return new so that it can become the new base
	return newEdge;
}

Delaunay::Edge* Delaunay::replaceEdge(Edge* edge, bool isReverse)
{
	Edge* reverse = nullptr;

	if (isReverse)
	{
		reverse = new Edge(edge->destination, edge->origin);
		reverse->ccwAroundOrigin = (edge == edge->ccwAroundDestination) ? (reverse) : (edge->ccwAroundDestination);
		reverse->cwAroundOrigin = (edge == edge->cwAroundDestination) ? (reverse) : (edge->cwAroundDestination);
		reverse->ccwAroundDestination = (edge == edge->ccwAroundOrigin) ? (reverse) : (edge->ccwAroundOrigin);
		reverse->cwAroundDestination = (edge == edge->cwAroundOrigin) ? (reverse) : (edge->cwAroundOrigin);
		edges.emplace(reverse);
	}

	//point edge->cwAroundDestination to edge->ccwAroundDestination or reversed edge
	if (edge->cwAroundDestination->origin == edge->destination)
	{
		edge->cwAroundDestination->ccwAroundOrigin = (isReverse) ? (reverse) : (edge->ccwAroundDestination);
	}
	else
	{
		edge->cwAroundDestination->ccwAroundDestination = (isReverse) ? (reverse) : (edge->ccwAroundDestination);
	}

	//point edge->ccwAroundDestination to edge->cwAroundDestination or reversed edge
	if (edge->ccwAroundDestination->origin == edge->destination)
	{
		edge->ccwAroundDestination->cwAroundOrigin = (isReverse) ? (reverse) : (edge->cwAroundDestination);
	}
	else
	{
		edge->ccwAroundDestination->cwAroundDestination = (isReverse) ? (reverse) : (edge->cwAroundDestination);
	}

	//point edge->cwAroundOrigin to edge->ccwAroundOrigin or reversed edge
	if (edge->cwAroundOrigin->origin == edge->origin)
	{
		edge->cwAroundOrigin->ccwAroundOrigin = (isReverse) ? (reverse) : (edge->ccwAroundOrigin);
	}
	else
	{
		edge->cwAroundOrigin->ccwAroundDestination = (isReverse) ? (reverse) : (edge->ccwAroundOrigin);
	}

	//point edge->ccwAroundOrigin to edge->cwAroundOrigin or reversed edge
	if (edge->ccwAroundOrigin->origin == edge->origin)
	{
		edge->ccwAroundOrigin->cwAroundOrigin = (isReverse) ? (reverse) : (edge->cwAroundOrigin);
	}
	else
	{
		edge->ccwAroundOrigin->cwAroundDestination = (isReverse) ? (reverse) : (edge->cwAroundOrigin);
	}

	//remove old edge
	edges.erase(edge);
	delete edge;

	//return reversed edge if edge was reversed
	return reverse;
}

Delaunay::Edge* Delaunay::reverseEdge(Edge* edge)
{
	return replaceEdge(edge, true);
}

void Delaunay::removeEdge(Edge* edge)
{
	replaceEdge(edge, false);
}

void Delaunay::createVoronoiCells(std::pair<Delaunay::Edge*, Delaunay::Edge*> bottomEdges)
{
	std::unordered_map<sf::Vector2f*, int> siteIndices;
	std::unordered_map<sf::Vector2f*, Edge*> siteEdges;
	std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*> siteVertexSets;
	std::unordered_set<int> vertexIds;
	sf::Vector2f* siteNW = sites.at(0);
	sf::Vector2f* siteNE = sites.at(0);
	sf::Vector2f* siteSW = sites.at(0);
	sf::Vector2f* siteSE = sites.at(0);
	float distNW = FLT_MAX;
	float distNE = FLT_MAX;
	float distSW = FLT_MAX;
	float distSE = FLT_MAX;
	int siteIndex = 0;

	//create vertex sets and territories and find corner sites and site indices
	for (sf::Vector2f* site : sites)
	{
		float dXNW = site->x;
		float dXNE = site->x - WIDTH;
		float dXSW = site->x;
		float dXSE = site->x - WIDTH;
		float dYNW = site->y - HEIGHT;
		float dYNE = site->y - HEIGHT;
		float dYSW = site->y;
		float dYSE = site->y;
		float distNWNew = dXNW * dXNW + dYNW * dYNW;
		float distNENew = dXNE * dXNE + dYNE * dYNE;
		float distSWNew = dXSW * dXSW + dYSW * dYSW;
		float distSENew = dXSE * dXSE + dYSE * dYSE;

		siteVertexSets.emplace(site, new std::vector<std::pair<sf::Vector2f*, float>*>());
		siteIndices.emplace(site, ++siteIndex);
		voronoiCells.push_back(new Territory("Territory " + std::to_string(siteIndex), site));

		if (distNWNew < distNW)
		{
			siteNW = site;
			distNW = distNWNew;
		}

		if (distNENew < distNE)
		{
			siteNE = site;
			distNE = distNENew;
		}

		if (distSWNew < distSW)
		{
			siteSW = site;
			distSW = distSWNew;
		}

		if (distSENew < distSE)
		{
			siteSE = site;
			distSE = distSENew;
		}
	}

	//add corners to vertex sets
	sf::Vector2f* nw = new sf::Vector2f(0.0f, HEIGHT);
	sf::Vector2f* ne = new sf::Vector2f(WIDTH, HEIGHT);
	sf::Vector2f* sw = new sf::Vector2f(0.0f, 0.0f);
	sf::Vector2f* se = new sf::Vector2f(WIDTH, 0.0f);
	siteVertexSets.at(siteNW)->push_back(new std::pair<sf::Vector2f*, float>(nw, atan2f(HEIGHT - siteNW->y, -siteNW->x)));
	siteVertexSets.at(siteNE)->push_back(new std::pair<sf::Vector2f*, float>(ne, atan2f(HEIGHT - siteNE->y, WIDTH - siteNE->x)));
	siteVertexSets.at(siteSW)->push_back(new std::pair<sf::Vector2f*, float>(sw, atan2f(-siteSW->y, -siteSW->x)));
	siteVertexSets.at(siteSE)->push_back(new std::pair<sf::Vector2f*, float>(se, atan2f(-siteSE->y, WIDTH - siteSE->x)));
	voronoiVertices.emplace(nw);
	voronoiVertices.emplace(ne);
	voronoiVertices.emplace(sw);
	voronoiVertices.emplace(se);

	//get vertices on outer walls
	getOuterVertices(bottomEdges.first, siteIndices, siteVertexSets);

	//get an edge for each vertex
	for (Edge* edge : edges)
	{
		siteEdges.emplace(edge->origin, edge);
		siteEdges.emplace(edge->destination, edge);
	}

	//get other vertices
	for (int s = 0; s < sites.size(); ++s)
	{
		getOtherVertices(sites.at(s), siteIndices, siteEdges, siteVertexSets, vertexIds);
	}

	//create cells and clean up
	for (int s = 0; s < sites.size(); ++s)
	{
		sf::ConvexShape* cell = new sf::ConvexShape(siteVertexSets.at(sites.at(s))->size());
		std::sort(siteVertexSets.at(sites.at(s))->begin(), siteVertexSets.at(sites.at(s))->end(), compareVector2fPtrFloatPairPtr);

		//add vertices to cell
		for (int v = 0; v < siteVertexSets.at(sites.at(s))->size(); ++v)
		{
			cell->setPoint(v, *siteVertexSets.at(sites.at(s))->at(v)->first);
		}

		voronoiCells.at(s)->setShape(cell);

		for (std::pair<sf::Vector2f*, float>* pair : *siteVertexSets.at(sites.at(s)))
		{
			delete pair;
		}

		delete siteVertexSets.at(sites.at(s));
	}
}

//find and return circumcenter of triangle abc (intersection point of perpendicular bisectors of 2 edges)
sf::Vector2f* Delaunay::getCircumcenter(sf::Vector2f* siteA, sf::Vector2f* siteB, sf::Vector2f* siteC)
{
	//ax + by = c
	float a = siteA->x - siteB->x;
	float b = siteA->y - siteB->y;
	float c = (siteA->x * siteA->x - siteB->x * siteB->x + siteA->y * siteA->y - siteB->y * siteB->y) / 2.0f;

	//dx + ey = f
	float d = siteA->x - siteC->x;
	float e = siteA->y - siteC->y;
	float f = (siteA->x * siteA->x - siteC->x * siteC->x + siteA->y * siteA->y - siteC->y * siteC->y) / 2.0f;

	float denominator = a * e - b * d;
	return new sf::Vector2f((c * e - b * f) / denominator, (a * f - c * d) / denominator);
}

bool Delaunay::isSiteTooClose(sf::Vector2f* site, Edge* edge, float x, float y)
{
	float dXE = edge->origin->x - x;
	float dYE = edge->origin->y - y;
	float dXS = site->x - x;
	float dYS = site->y - y;
	float dS = dXS * dXS + dYS * dYS;
	float dE = dXE * dXE + dYE * dYE;
	return dS < dE;
}

void Delaunay::getOuterVertex(Edge* edge, bool isOutCw, char direction, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets)
{
	sf::Vector2f* site = nullptr;
	Edge* edge1 = nullptr;
	Edge* edge2 = nullptr;
	bool isOutCw1 = false;
	bool isOutCw2 = false;
	Territory* territory1 = voronoiCells.at(siteIndices.at(edge->origin) - 1);
	Territory* territory2 = voronoiCells.at(siteIndices.at(edge->destination) - 1);

	if (isOutCw)
	{
		site = (edge->ccwAroundOrigin->origin == edge->origin) ? (edge->ccwAroundOrigin->destination) : (edge->ccwAroundOrigin->origin);
		edge1 = edge->ccwAroundOrigin;
		edge2 = edge->cwAroundDestination;
		isOutCw1 = edge1->origin == edge->origin;
		isOutCw2 = edge2->destination == edge->destination;
	}
	else
	{
		site = (edge->cwAroundOrigin->origin == edge->origin) ? (edge->cwAroundOrigin->destination) : (edge->cwAroundOrigin->origin);
		edge1 = edge->ccwAroundDestination;
		edge2 = edge->cwAroundOrigin;
		isOutCw1 = edge1->origin == edge->destination;
		isOutCw2 = edge2->destination == edge->origin;
	}

	//ax + by = c
	float a = edge->origin->x - edge->destination->x;
	float b = edge->origin->y - edge->destination->y;
	float c = (edge->origin->x * edge->origin->x - edge->destination->x * edge->destination->x + edge->origin->y * edge->origin->y - edge->destination->y * edge->destination->y) / 2.0f;

	//check N wall
	if (direction == 'N')
	{
		float n = (a == 0) ? (-1) : ((c - b * HEIGHT) / a);

		if (n >= 0.0f && n <= WIDTH)
		{
			if (isSiteTooClose(site, edge, n, HEIGHT))
			{
				getOuterVertex(edge1, isOutCw1, 'N', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'N', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(n, HEIGHT);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}

	//check S wall
	if (direction == 'S')
	{
		float n = (a == 0) ? (-1) : (c / a);

		if (n >= 0.0f && n <= WIDTH)
		{
			if (isSiteTooClose(site, edge, n, 0.0f))
			{
				getOuterVertex(edge1, isOutCw1, 'S', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'S', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(n, 0.0f);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}

	//check E wall
	if (direction == 'E')
	{
		float n = (b == 0) ? (-1) : ((c - a * WIDTH) / b);

		if (n >= 0.0f && n <= HEIGHT)
		{
			if (isSiteTooClose(site, edge, WIDTH, n))
			{
				getOuterVertex(edge1, isOutCw1, 'E', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'E', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(WIDTH, n);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}

	//check W wall
	if (direction == 'W')
	{
		float n = (b == 0) ? (-1) : (c / b);

		if (n >= 0.0f && n <= HEIGHT)
		{
			if (isSiteTooClose(site, edge, 0.0f, n))
			{
				getOuterVertex(edge1, isOutCw1, 'W', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'W', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(0.0f, n);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}
}

void Delaunay::getOuterVertex(Edge* edge, bool isOutCw, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets)
{
	sf::Vector2f* site = nullptr;
	Edge* edge1 = nullptr;
	Edge* edge2 = nullptr;
	bool isOutCw1 = false;
	bool isOutCw2 = false;
	Territory* territory1 = voronoiCells.at(siteIndices.at(edge->origin) - 1);
	Territory* territory2 = voronoiCells.at(siteIndices.at(edge->destination) - 1);

	if (isOutCw)
	{
		site = (edge->ccwAroundOrigin->origin == edge->origin) ? (edge->ccwAroundOrigin->destination) : (edge->ccwAroundOrigin->origin);
		edge1 = edge->ccwAroundOrigin;
		edge2 = edge->cwAroundDestination;
		isOutCw1 = edge1->origin == edge->origin;
		isOutCw2 = edge2->destination == edge->destination;
	}
	else
	{
		site = (edge->cwAroundOrigin->origin == edge->origin) ? (edge->cwAroundOrigin->destination) : (edge->cwAroundOrigin->origin);
		edge1 = edge->ccwAroundDestination;
		edge2 = edge->cwAroundOrigin;
		isOutCw1 = edge1->origin == edge->destination;
		isOutCw2 = edge2->destination == edge->origin;
	}

	//ax + by = c
	float a = edge->origin->x - edge->destination->x;
	float b = edge->origin->y - edge->destination->y;
	float c = (edge->origin->x * edge->origin->x - edge->destination->x * edge->destination->x + edge->origin->y * edge->origin->y - edge->destination->y * edge->destination->y) / 2.0f;

	//edge center
	float x = (edge->origin->x + edge->destination->x) / 2.0f;
	float y = (edge->origin->y + edge->destination->y) / 2.0f;

	//site projected onto ax + by = c
	float k = ((site->x - x) * b + (y - site->y) * a) / (a * a + b * b);
	float u = x + b * k;
	float v = y - a * k;

	//check N wall
	if (y > v)
	{
		float n = (a == 0) ? (-1) : ((c - b * HEIGHT) / a);

		if (n >= 0.0f && n <= WIDTH)
		{
			if (isSiteTooClose(site, edge, n, HEIGHT))
			{
				getOuterVertex(edge1, isOutCw1, 'N', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'N', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(n, HEIGHT);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}

	//check S wall
	if (y < v)
	{
		float n = (a == 0) ? (-1) : (c / a);

		if (n >= 0.0f && n <= WIDTH)
		{
			if (isSiteTooClose(site, edge, n, 0.0f))
			{
				getOuterVertex(edge1, isOutCw1, 'S', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'S', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(n, 0.0f);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}

	//check E wall
	if (x > u)
	{
		float n = (b == 0) ? (-1) : ((c - a * WIDTH) / b);

		if (n >= 0.0f && n <= HEIGHT)
		{
			if (isSiteTooClose(site, edge, WIDTH, n))
			{
				getOuterVertex(edge1, isOutCw1, 'E', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'E', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(WIDTH, n);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}

	//check W wall
	if (x < u)
	{
		float n = (b == 0) ? (-1) : (c / b);

		if (n >= 0.0f && n <= HEIGHT)
		{
			if (isSiteTooClose(site, edge, 0.0f, n))
			{
				getOuterVertex(edge1, isOutCw1, 'W', siteIndices, siteVertexSets);
				getOuterVertex(edge2, isOutCw2, 'W', siteIndices, siteVertexSets);
				return;
			}
			else
			{
				sf::Vector2f* vertex = new sf::Vector2f(0.0f, n);
				voronoiVertices.emplace(vertex);
				siteVertexSets.at(edge->origin)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->origin->y, vertex->x - edge->origin->x)));
				siteVertexSets.at(edge->destination)->push_back(new std::pair<sf::Vector2f*, float>(vertex, atan2f(vertex->y - edge->destination->y, vertex->x - edge->destination->x)));
				territory1->addNeighbor(territory2);
				territory2->addNeighbor(territory1);
				return;
			}
		}
	}
}

void Delaunay::getOuterVertices(Edge* firstEdge, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets)
{
	Edge* edge = firstEdge;

	do
	{
		getOuterVertex(edge, false, siteIndices, siteVertexSets);
		edge = edge->cwAroundDestination;
	} while (edge != firstEdge);
}

void Delaunay::getOtherVertices(sf::Vector2f* site, std::unordered_map<sf::Vector2f*, int>& siteIndices, std::unordered_map<sf::Vector2f*, Edge*>& siteEdges, std::unordered_map<sf::Vector2f*, std::vector<std::pair<sf::Vector2f*, float>*>*>& siteVertexSets, std::unordered_set<int>& vertexIds)
{
	Edge* edge = siteEdges.at(site);
	bool isSiteEdgeOrigin = edge->origin == site;
	sf::Vector2f* edgeSite = (isSiteEdgeOrigin) ? (edge->destination) : (edge->origin);

	//create cell vertices
	do
	{
		Edge* nextEdge = (isSiteEdgeOrigin) ? (edge->ccwAroundOrigin) : (edge->ccwAroundDestination);
		bool isSiteNextEdgeOrigin = nextEdge->origin == site;
		sf::Vector2f* nextEdgeSite = (isSiteNextEdgeOrigin) ? (nextEdge->destination) : (nextEdge->origin);

		if (getOrientation(nextEdgeSite, edgeSite, site) < 0)
		{
			//site trio forms vertex
			int vertexId = getVertexId(siteIndices, nextEdgeSite, edgeSite, site);

			//make sure vertex wasn't already checked
			if (!vertexIds.count(vertexId))
			{
				sf::Vector2f* circumcenter = getCircumcenter(site, edgeSite, nextEdgeSite);
				vertexIds.emplace(vertexId);

				if (circumcenter->x >= 0.0f && circumcenter->y >= 0.0f && circumcenter->x <= WIDTH && circumcenter->y <= HEIGHT)
				{
					voronoiVertices.emplace(circumcenter);
					siteVertexSets.at(site)->push_back(new std::pair<sf::Vector2f*, float>(circumcenter, atan2f(circumcenter->y - site->y, circumcenter->x - site->x)));
					siteVertexSets.at(edgeSite)->push_back(new std::pair<sf::Vector2f*, float>(circumcenter, atan2f(circumcenter->y - edgeSite->y, circumcenter->x - edgeSite->x)));
					siteVertexSets.at(nextEdgeSite)->push_back(new std::pair<sf::Vector2f*, float>(circumcenter, atan2f(circumcenter->y - nextEdgeSite->y, circumcenter->x - nextEdgeSite->x)));
					Territory* territory1 = voronoiCells.at(siteIndices.at(site) - 1);
					Territory* territory2 = voronoiCells.at(siteIndices.at(edgeSite) - 1);
					Territory* territory3 = voronoiCells.at(siteIndices.at(nextEdgeSite) - 1);
					territory1->addNeighbor(territory2);
					territory1->addNeighbor(territory3);
					territory2->addNeighbor(territory1);
					territory2->addNeighbor(territory3);
					territory3->addNeighbor(territory1);
					territory3->addNeighbor(territory2);
				}
			}
		}

		edge = nextEdge;
		isSiteEdgeOrigin = isSiteNextEdgeOrigin;
		edgeSite = nextEdgeSite;
	}
	while (edge != siteEdges.at(site));
}

int Delaunay::getVertexId(std::unordered_map<sf::Vector2f*, int>& siteIndices, sf::Vector2f* siteA, sf::Vector2f* siteB, sf::Vector2f* siteC)
{
	int indexA = siteIndices.at(siteA);
	int indexB = siteIndices.at(siteB);
	int indexC = siteIndices.at(siteC);
	int base = sites.size() + 1;

	if (indexA < indexB)
	{
		//ABC or ACB or CAB
		if (indexA < indexC)
		{
			//ABC or ACB
			return (indexB < indexC) ? ((indexA * base + indexB) * base + indexC) : ((indexA * base + indexC) * base + indexB);
		}
		else
		{
			//CAB
			return (indexC * base + indexA) * base + indexB;
		}
	}
	else if (indexB < indexC)
	{
		//BAC or BCA
		return (indexA < indexC) ? ((indexB * base + indexA) * base + indexC) : ((indexB * base + indexC) * base + indexA);
	}
	else
	{
		//CBA
		return (indexC * base + indexB) * base + indexA;
	}
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