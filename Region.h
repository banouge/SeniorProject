#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include "Player.h"

class Territory;

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
	void updateOwner(Player* newOwner);
	int getValue();
	std::unordered_set<Territory*>& getTerritories();
	Player* getOwner();

private:
	int value;
	std::unordered_set<Territory*> territories;
	Player* owner;
};