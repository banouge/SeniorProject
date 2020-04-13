#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include "SFML/Graphics.hpp"
#include "SFML/System/Vector2.hpp"
#include "Player.h"

class Region;

class Territory
{
public:
	Territory(std::string name, sf::Vector2f* position);
	~Territory();

	sf::Vector2f* const POSITION;

	static void setBaseNeutralArmies(int base);
	static void setBaseFriendlyArmies(int base);
	static int getBaseNeutralArmies();
	static int getBaseFriendlyArmies();

	void draw(sf::RenderWindow* window);
	void setOrigin(sf::Vector2f origin);
	void updateText();
	void initializeText();
	void setName(std::string name);
	void setShape(sf::ConvexShape* shape);
	void setHeight(double height);
	void setRegion(Region* region);
	void setTotalArmies(int totalArmies);
	void addGeneral();
	void removeGeneral();
	void setOwner(Player* owner);
	void addNeighbor(Territory* neighbor);
	void addDistantNeighbor(Territory* neighbor);
	void addArmies(int numNewArmies);
	void removeArmies(int numLostArmies, bool shouldExhaustedBeRemovedFirst);
	void removeNeighbor(Territory* neighbor);
	void writeToOutput(std::ostream& output);
	void rejuvenateArmies();
	double getHeight();
	Region* getRegion();
	std::unordered_set<Territory*>& getNeighbors();
	std::string getName();
	sf::ConvexShape* getShape();
	int getNumArmies();
	int getTotalArmies();
	Player* getOwner();
	bool hasGeneral();
	bool isGeneralExhausted();
	bool wasCapturedThisTurn();
	bool hasNeighbor(Territory* neighbor);

private:
	static int baseNeutralArmies;
	static int baseFriendlyArmies;

	std::string name;
	sf::ConvexShape* shape;
	sf::Text* text;
	std::unordered_set<Territory*> neighbors;
	std::unordered_set<Territory*> distantNeighbors;
	Region* region;
	double height;
	int numArmies;
	int numExhaustedArmies;
	bool doesHaveGeneral;
	bool hasExhaustedGeneral;
	Player* oldOwner;
	Player* owner;
	sf::Vector2f centroidOffset;
};