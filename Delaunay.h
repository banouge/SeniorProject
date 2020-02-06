#pragma once

#include <unordered_set>
#include <utility>
#include <vector>
#include "SFML/System/Vector2.hpp"

//REFERENCES:
//http://www.geom.uiuc.edu/~samuelp/del_project.html
//http://www.sccg.sk/~samuelcik/dgs/quad_edge.pdf

class Delaunay
{
public:
	Delaunay(int numSites = 0, float width = 1024.0f, float height = 2048.0f);
	Delaunay(std::vector<sf::Vector2f*> siteList, float width = 1024.0f, float height = 2048.0f);
	~Delaunay();

private:
	struct Edge
	{
		Edge(sf::Vector2f* o, sf::Vector2f* d, Edge* ccwo = nullptr, Edge* cwo = nullptr, Edge* ccwd = nullptr, Edge* cwd = nullptr);

		sf::Vector2f* origin;
		sf::Vector2f* destination;
		Edge* ccwAroundOrigin;
		Edge* cwAroundOrigin;
		Edge* ccwAroundDestination;
		Edge* cwAroundDestination;
	};

	const float WIDTH;
	const float HEIGHT;

	std::vector<sf::Vector2f*> sites;
	std::unordered_set<Edge*> edges;

	static bool compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b);
	static int getOrientation(sf::Vector2f* a, sf::Vector2f* b, sf::Vector2f* c);

	std::vector<Edge*> triangulate(int firstSiteIndex, int lastSiteIndex);
	std::vector<Edge*> triangulate3(int firstSiteIndex, int lastSiteIndex);
	std::vector<Edge*> triangulate2(int firstSiteIndex, int lastSiteIndex);
	std::vector<Edge*> merge(std::vector<Edge*> leftEdges, std::vector<Edge*> rightEdges);
};