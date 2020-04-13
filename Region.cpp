#include "Region.h"
#include "Territory.h"

const sf::Color Region::NEUTRAL_COLOR = sf::Color(0, 0, 0, 255);

Region::Region(std::string name, sf::Color color, int value) : NAME(name), COLOR(color)
{
	this->value = value;
	button = new TextButton(name + ": " + std::to_string(value), color, sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
}

Region::~Region()
{
	delete button;
}

void Region::draw(sf::RenderWindow* window)
{
	button->draw(window);
}

void Region::setOrigin(sf::Vector2f origin)
{
	button->setPosition(position.x - origin.x, position.y - origin.y);
}

void Region::intitializeButton()
{
	sf::Vector2f min(FLT_MAX, FLT_MAX);
	sf::Vector2f max(-FLT_MAX, -FLT_MAX);
	
	for (Territory* territory : territories)
	{
		if (territory->POSITION->x < min.x)
		{
			min.x = territory->POSITION->x;
		}

		if (territory->POSITION->x > max.x)
		{
			max.x = territory->POSITION->x;
		}

		if (territory->POSITION->y < min.y)
		{
			min.y = territory->POSITION->y;
		}

		if (territory->POSITION->y > max.y)
		{
			max.y = territory->POSITION->y;
		}
	}

	sf::Vector2f center = 0.5f * (min + max);
	sf::Vector2f position;
	float dist = FLT_MAX;

	for (Territory* territory : territories)
	{
		for (int v = 0; v < territory->getShape()->getPointCount(); ++v)
		{
			sf::Vector2f vertex = territory->getShape()->getPoint(v) + *territory->POSITION;
			sf::Vector2f diff = vertex - center;
			float d = diff.x * diff.x + diff.y * diff.y;

			if (d < dist)
			{
				dist = d;
				position = vertex;
			}
		}
	}

	button->setSize(64.0f, 64.0f);
	button->setPosition(position.x - 32.0f, position.y - 32.0f);
	this->position = sf::Vector2f(position.x - 32.0f, position.y - 32.0f);
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