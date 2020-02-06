#pragma once

#include <tuple>
#include <vector>
#include "SFML/System/Vector2.hpp"

//REFERENCES:
//http://www.geom.uiuc.edu/~samuelp/del_project.html

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
	std::vector<std::tuple<sf::Vector2f*, sf::Vector2f*>> edges;

	static bool compareVector2fPtr(sf::Vector2f* a, sf::Vector2f* b);

	std::vector<std::tuple<sf::Vector2f*, sf::Vector2f*>> triangulate(int firstSiteIndex, int lastSiteIndex);
};