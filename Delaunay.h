#pragma once

#include <unordered_set>
#include <utility>
#include <vector>
#include "SFML/System/Vector2.hpp"

//REFERENCES:
//http://www.geom.uiuc.edu/~samuelp/del_project.html

typedef std::pair<sf::Vector2f*, sf::Vector2f*> DelaunayEdge;

class Delaunay
{
public:
	Delaunay(int numSites = 0, float width = 1024.0f, float height = 2048.0f);
	Delaunay(std::vector<sf::Vector2f*> &siteList, float width = 1024.0f, float height = 2048.0f);
	~Delaunay();

private:
	const float WIDTH;
	const float HEIGHT;

	std::vector<sf::Vector2f*> sites;
	std::unordered_set<DelaunayEdge*> edges;

	static bool compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b);
	static int getOrientation(sf::Vector2f* a, sf::Vector2f* b, sf::Vector2f* c);

	std::vector<DelaunayEdge*> triangulate(int firstSiteIndex, int lastSiteIndex);
	std::vector<DelaunayEdge*> triangulate3(int firstSiteIndex, int lastSiteIndex);
	std::vector<DelaunayEdge*> triangulate2(int firstSiteIndex, int lastSiteIndex);
};