#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "Player.h"

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
	void setNumArmies(int numArmies);
	void setHasGeneral(bool doesHaveGeneral);
	void setOwner(Player* owner);
	void addNeighbor(Territory* neighbor);
	void addDistantNeighbor(Territory* neighbor);
	void removeNeighbor(Territory* neighbor);
	void writeToOutput(std::ostream& output);
	double getHeight();
	Region* getRegion();
	std::unordered_set<Territory*>& getNeighbors();
	std::string getName();
	sf::ConvexShape* getShape();
	int getNumArmies();
	Player* getOwner();
	bool hasGeneral();

private:
	std::string name;
	sf::ConvexShape* shape;
	std::unordered_set<Territory*> neighbors;
	std::unordered_set<Territory*> distantNeighbors;
	Region* region;
	double height;
	int numArmies;
	bool doesHaveGeneral;
	Player* owner;
};