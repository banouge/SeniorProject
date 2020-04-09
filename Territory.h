#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/System/Vector2.hpp"

class Region;

class Territory
{
public:
	Territory(std::string name, sf::Vector2f* position);
	~Territory();

	sf::Vector2f* POSITION;

	void setName(std::string name);
	void setShape(sf::ConvexShape* shape);
	void setHeight(double height);
	void setRegion(Region* region);
	void addNeighbor(Territory* neighbor);
	void addDistantNeighbor(Territory* neighbor);
	void removeNeighbor(Territory* neighbor);
	void writeToOutput(std::ostream& output);
	double getHeight();
	Region* getRegion();
	std::unordered_set<Territory*>& getNeighbors();
	std::string getName();

private:
	std::string name;
	sf::ConvexShape* shape;
	std::unordered_set<Territory*> neighbors;
	std::unordered_set<Territory*> distantNeighbors;
	Region* region;
	double height;
};

class Region
{
public:
	Region(std::string name, int value = -1);
	~Region();

	const std::string NAME;

	void setValue(int value);
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);
	void writeToOutput(std::ostream& output);
	int getValue();
	std::unordered_set<Territory*>& getTerritories();

private:
	int value;
	std::unordered_set<Territory*> territories;
};