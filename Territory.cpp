#include "Region.h"
#include "Territory.h"

Territory::Territory(std::string name, sf::Vector2f* position): POSITION(position)
{
	this->name = name;
	region = nullptr;
	owner = nullptr;
	numArmies = 0;
	numExhaustedArmies = 0;
	doesHaveGeneral = false;
	hasExhaustedGeneral = false;
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

void Territory::setTotalArmies(int totalArmies)
{
	int delta = totalArmies - numArmies - numExhaustedArmies;

	if (delta >= 0)
	{
		numExhaustedArmies += delta;
	}
	else if (numExhaustedArmies + delta >= 0)
	{
		numExhaustedArmies += delta;
	}
	else
	{
		numExhaustedArmies = 0;
		numArmies = totalArmies;
	}
}

void Territory::addGeneral()
{
	doesHaveGeneral = true;
	hasExhaustedGeneral = true;
}

void Territory::removeGeneral()
{
	doesHaveGeneral = false;
	hasExhaustedGeneral = false;
}

void Territory::setOwner(Player* owner)
{
	this->owner = owner;

	if (region)
	{
		region->updateOwner(owner);
	}
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

void Territory::addArmies(int numNewArmies)
{
	numExhaustedArmies += numNewArmies;
}

void Territory::removeArmies(int numLostArmies, bool shouldExhaustedBeRemovedFirst)
{
	if (shouldExhaustedBeRemovedFirst)
	{
		if (numExhaustedArmies >= numLostArmies)
		{
			numExhaustedArmies -= numLostArmies;
		}
		else if (numArmies + numExhaustedArmies >= numLostArmies)
		{
			numArmies += numExhaustedArmies - numLostArmies;
			numExhaustedArmies = 0;
		}
		else
		{
			numArmies = 0;
			numExhaustedArmies = 0;
		}
	}
	else
	{
		if (numArmies >= numLostArmies)
		{
			numArmies -= numLostArmies;
		}
		else if (numArmies + numExhaustedArmies >= numLostArmies)
		{
			numExhaustedArmies += numArmies - numLostArmies;
			numArmies = 0;
		}
		else
		{
			numArmies = 0;
			numExhaustedArmies = 0;
		}
	}
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

void Territory::rejuvenateArmies()
{
	numArmies += numExhaustedArmies;
	numExhaustedArmies = 0;
	doesHaveGeneral = hasExhaustedGeneral || doesHaveGeneral;
	hasExhaustedGeneral = false;
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

sf::ConvexShape* Territory::getShape()
{
	return shape;
}

int Territory::getNumArmies()
{
	return numArmies;
}

int Territory::getTotalArmies()
{
	return numArmies + numExhaustedArmies;
}

Player* Territory::getOwner()
{
	return owner;
}

bool Territory::hasGeneral()
{
	return doesHaveGeneral;
}

bool Territory::isGeneralExhausted()
{
	return hasExhaustedGeneral;
}