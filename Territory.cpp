#include "Territory.h"

Territory::Territory(std::string name, sf::Vector2f* position): NAME(name), POSITION(position)
{
}

Territory::~Territory()
{
}

void Territory::setShape(sf::ConvexShape* shape)
{
	this->shape = shape;
}

void Territory::setHeight(double height)
{
	this->height = height;
}

void Territory::addNeighbor(Territory* neighbor)
{
	neighbors.emplace(neighbor);
}

void Territory::removeNeighbor(Territory* neighbor)
{
	neighbors.erase(neighbor);
}

double Territory::getHeight()
{
	return height;
}

std::unordered_set<Territory*>& Territory::getNeighbors()
{
	return neighbors;
}

std::string Territory::toString()
{
	//TODO
	return std::string();
}