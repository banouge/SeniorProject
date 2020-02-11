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
	return (a->x == b->x) ? (a->y < b->y) : (a->x < b->x);
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

	//TODO: probably won't work properly if bottom vertex is not unique (multiple at same y), may need way to get rightmost for left and leftmost for right (bool parameter?)
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
	return (isRightLower) ? (std::pair<Edge*, Edge*>(bottomEdge, rightEdges.second)) : (std::pair<Edge*, Edge*>(leftEdges.first, bottomEdge));
}

Delaunay::Edge* Delaunay::createBottomEdge(std::pair<Edge*, Edge*> leftEdges, std::pair<Edge*, Edge*> rightEdges)
{
	//make base edge from bottom vertices of each side
	Edge* baseEdge = new Edge(rightEdges.first->origin, leftEdges.first->origin, rightEdges.second, rightEdges.first, leftEdges.second, leftEdges.first);
	edges.emplace(baseEdge);

	//add base edge as neighbor of left edges
	if (leftEdges.first == leftEdges.second)
	{
		//upward edge from triangulate2, base edge only touches origin
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
		//upward edge from triangulate2, base edge only touches origin
		rightEdges.first->ccwAroundOrigin = baseEdge;
		rightEdges.first->cwAroundOrigin = baseEdge;
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
		reverse = new Edge(edge->destination, edge->origin, edge->ccwAroundDestination, edge->cwAroundDestination, edge->ccwAroundOrigin, edge->cwAroundOrigin);
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

Delaunay::Edge::Edge(sf::Vector2f* o, sf::Vector2f* d, Edge* ccwo, Edge* cwo, Edge* ccwd, Edge* cwd)
{
	origin = o;
	destination = d;
	ccwAroundOrigin = ccwo;
	cwAroundOrigin = cwo;
	ccwAroundDestination = ccwd;
	cwAroundDestination = cwd;
}