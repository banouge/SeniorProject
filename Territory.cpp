#include "Region.h"
#include "Resources.h"
#include "Territory.h"

int Territory::baseNeutralArmies = 3;
int Territory::baseFriendlyArmies = 5;

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
	delete text;
}

void Territory::setBaseNeutralArmies(int base)
{
	baseNeutralArmies = base;
}

void Territory::setBaseFriendlyArmies(int base)
{
	baseFriendlyArmies = base;
}

int Territory::getBaseNeutralArmies()
{
	return baseNeutralArmies;
}

int Territory::getBaseFriendlyArmies()
{
	return baseFriendlyArmies;
}

void Territory::draw(sf::RenderWindow* window)
{
	window->draw(*shape);
	window->draw(*text);
}

void Territory::setOrigin(sf::Vector2f origin)
{
	shape->setOrigin(origin);
	text->setPosition(shape->getPosition() + centroidOffset - shape->getOrigin());
}

void Territory::updateText()
{
	text->setString((doesHaveGeneral) ? (name + ": " + std::to_string(numArmies + numExhaustedArmies) + " G") : (name + ": " + std::to_string(numArmies + numExhaustedArmies)));
	text->setOrigin(0.5f * text->getLocalBounds().width, 0.5f * text->getLocalBounds().height);
}

void Territory::initializeText()
{
	sf::Vector2f min = sf::Vector2f(FLT_MAX, FLT_MAX);
	sf::Vector2f max = sf::Vector2f(-FLT_MAX, -FLT_MAX);

	for (int v = 0; v < shape->getPointCount(); ++v)
	{
		if (shape->getPoint(v).x < min.x)
		{
			min.x = shape->getPoint(v).x;
		}

		if (shape->getPoint(v).x > max.x)
		{
			max.x = shape->getPoint(v).x;
		}

		if (shape->getPoint(v).y < min.y)
		{
			min.y = shape->getPoint(v).y;
		}

		if (shape->getPoint(v).y > max.y)
		{
			max.y = shape->getPoint(v).y;
		}
	}

	centroidOffset = 0.5f * (min + max);

	text = new sf::Text((doesHaveGeneral) ? (name + ": " + std::to_string(numArmies + numExhaustedArmies) + " G") : (name + ": " + std::to_string(numArmies + numExhaustedArmies)), Resources::arialFont);
	text->setOrigin(0.5f * text->getLocalBounds().width, 0.5f * text->getLocalBounds().height);
	text->setPosition(*POSITION + centroidOffset);
	text->setOutlineThickness(3.0f);
	text->setOutlineColor(sf::Color(0, 0, 0, 255));
	text->setFillColor(sf::Color(255, 255, 255, 255));
}

void Territory::setName(std::string name)
{
	this->name = name;
}

void Territory::setShape(sf::ConvexShape* shape)
{
	this->shape = shape;
	shape->setOutlineThickness(3.0f);
	shape->setOutlineColor(Region::NEUTRAL_COLOR);
	shape->setFillColor(Player::NEUTRAL_COLOR);
}

void Territory::setHeight(double height)
{
	this->height = height;
}

void Territory::setRegion(Region* region)
{
	this->region = region;

	if (!region || !region->getValue())
	{
		shape->setOutlineColor(Region::NEUTRAL_COLOR);
	}
	else
	{
		shape->setOutlineColor(region->COLOR);
	}
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

	updateText();
}

void Territory::addGeneral()
{
	doesHaveGeneral = true;
	hasExhaustedGeneral = true;
	updateText();
}

void Territory::removeGeneral()
{
	doesHaveGeneral = false;
	hasExhaustedGeneral = false;
	updateText();
}

void Territory::setOwner(Player* owner)
{
	if (this->owner)
	{
		this->owner->removeTerritory(this);
	}

	this->owner = owner;

	if (owner)
	{
		owner->addTerritory(this);
		shape->setFillColor(owner->COLOR);
	}
	else
	{
		shape->setFillColor(Player::NEUTRAL_COLOR);
	}

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
	updateText();
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

	updateText();
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
	oldOwner = owner;
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

bool Territory::wasCapturedThisTurn()
{
	return owner != oldOwner;
}

bool Territory::hasNeighbor(Territory* neighbor)
{
	return neighbors.count(neighbor);
}