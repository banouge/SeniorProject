#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include "Player.h"

class Territory;

class Region
{
public:
	Region(std::string name, sf::Color color, int value = -1);
	~Region();

	static const sf::Color NEUTRAL_COLOR;

	const std::string NAME;
	const sf::Color COLOR;

	void setValue(int value);
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);
	void writeToOutput(std::ostream& output);
	void updateOwner(Player* newOwner);
	int getValue();
	std::unordered_set<Territory*>& getTerritories();
	Player* getOwner();

private:
	int value;
	std::unordered_set<Territory*> territories;
	Player* owner;
};