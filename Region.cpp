#include "Region.h"
#include "Territory.h"

const sf::Color Region::NEUTRAL_COLOR = sf::Color(0, 0, 0, 255);

Region::Region(std::string name, sf::Color color, int value) : NAME(name), COLOR(color)
{
	this->value = value;
}

Region::~Region()
{
}

void Region::setValue(int value)
{
	this->value = value;
}

void Region::addTerritory(Territory* territory)
{
	territories.emplace(territory);
}

void Region::removeTerritory(Territory* territory)
{
	territories.erase(territory);
}

void Region::writeToOutput(std::ostream& output)
{
	output << "NAME: " << NAME << '\n';
	output << "VALUE: " << std::to_string(value) << '\n';
	output << "NUMBER OF TERRITORIES: " << std::to_string(territories.size()) << '\n';

	for (Territory* territory : territories)
	{
		output << territory->getName() << '\n';
	}
}

void Region::updateOwner(Player* newOwner)
{
	if (owner)
	{
		owner->addIncome(-value);
	}

	if (!newOwner)
	{
		owner = nullptr;
		return;
	}

	for (Territory* territory : territories)
	{
		if (territory->getOwner() != newOwner)
		{
			return;
		}
	}

	owner = newOwner;
	owner->addIncome(value);
}

int Region::getValue()
{
	return value;
}

std::unordered_set<Territory*>& Region::getTerritories()
{
	return territories;
}

Player* Region::getOwner()
{
	return owner;
}