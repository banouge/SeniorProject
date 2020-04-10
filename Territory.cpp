#include "Territory.h"

Territory::Territory(std::string name, sf::Vector2f* position): POSITION(position)
{
	this->name = name;
}

Territory::~Territory()
{
	delete shape;
}

void Territory::setName(std::string name)
{
	this->name = name;
}

void Territory::setShape(sf::ConvexShape* shape)
{
	this->shape = shape;
}

void Territory::setHeight(double height)
{
	this->height = height;
}

void Territory::setRegion(Region* region)
{
	this->region = region;
}

void Territory::addNeighbor(Territory* neighbor)
{
	neighbors.emplace(neighbor);
}

void Territory::addDistantNeighbor(Territory* neighbor)
{
	distantNeighbors.emplace(neighbor);
	neighbors.emplace(neighbor);
}

void Territory::removeNeighbor(Territory* neighbor)
{
	neighbors.erase(neighbor);
}

void Territory::writeToOutput(std::ostream& output)
{
	//no need to output region because region knows what territories it has
	output << "NAME: " << name << '\n';
	output << "POSITION: " << std::to_string(POSITION->x) << ';' << std::to_string(POSITION->y) << '\n';
	output << "NUMBER OF VERTICES: " << std::to_string(shape->getPointCount()) << '\n';

	for (int v = 0; v < shape->getPointCount(); ++v)
	{
		output << std::to_string(shape->getPoint(v).x) << ';' << std::to_string(shape->getPoint(v).y) << '\n';
	}

	output << "NUMBER OF NEIGHBORS: " << std::to_string(neighbors.size()) << '\n';

	for (Territory* neighbor : neighbors)
	{
		output << neighbor->getName() << '\n';
	}

	output << "NUMBER OF DISTANT NEIGHBORS: " << std::to_string(distantNeighbors.size()) << '\n';

	for (Territory* neighbor : distantNeighbors)
	{
		output << neighbor->getName() << '\n';
	}
}

double Territory::getHeight()
{
	return height;
}

Region* Territory::getRegion()
{
	return region;
}

std::unordered_set<Territory*>& Territory::getNeighbors()
{
	return neighbors;
}

std::string Territory::getName()
{
	return name;
}

Region::Region(std::string name, int value): NAME(name)
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

int Region::getValue()
{
	return value;
}

std::unordered_set<Territory*>& Region::getTerritories()
{
	return territories;
}