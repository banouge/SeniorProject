#pragma once

#include <string>
#include <unordered_set>
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/System/Vector2.hpp"

class Territory
{
public:
	Territory(std::string name, sf::Vector2f* position);
	~Territory();

	const std::string NAME;
	sf::Vector2f* POSITION;

	void setShape(sf::ConvexShape* shape);
	void setHeight(double height);
	void addNeighbor(Territory* neighbor);
	void addDistantNeighbor(Territory* neighbor);
	void removeNeighbor(Territory* neighbor);
	double getHeight();
	std::unordered_set<Territory*>& getNeighbors();
	std::string toString();

private:
	sf::ConvexShape* shape;
	std::unordered_set<Territory*> neighbors;
	std::unordered_set<Territory*> distantNeighbors;
	double height;
};