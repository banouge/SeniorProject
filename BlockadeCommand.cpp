#include "BlockadeCommand.h"

const int BlockadeCommand::INDEX = 1;
int BlockadeCommand::bracket = 3;
int BlockadeCommand::weight = 0;
int BlockadeCommand::numPieces = 1;
int BlockadeCommand::minPiecesPerTurn = 0;
float BlockadeCommand::multiplier = 3.0f;

BlockadeCommand::BlockadeCommand(Player* commander, Territory* territory) : CardCommand(commander, bracket, 0, territory, numPieces, INDEX), button("", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255))
{
	button.setString(territory->getName() + " is scorched.");
}

void BlockadeCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (3) : (7);
}

void BlockadeCommand::setMultiplier(float m)
{
	multiplier = m;
}

int BlockadeCommand::getBracket()
{
	return bracket;
}

void BlockadeCommand::setWeight(int w)
{
	weight = w;
}

int BlockadeCommand::getWeight()
{
	return weight;
}

void BlockadeCommand::setNumPieces(int n)
{
	numPieces = n;
}

int BlockadeCommand::getNumPieces()
{
	return numPieces;
}

void BlockadeCommand::setMinPiecesPerTurn(int n)
{
	minPiecesPerTurn = n;
}

int BlockadeCommand::getMinPiecesPerTurn()
{
	return minPiecesPerTurn;
}

void BlockadeCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER && !TERRITORY->hasGeneral() && COMMANDER->getNumTerritories() > 1)
	{
		TERRITORY->setTotalArmies((int)round(multiplier * TERRITORY->getTotalArmies()));
		TERRITORY->setOwner(nullptr);
	}
}

void BlockadeCommand::draw(sf::RenderWindow* window)
{
	button.draw(window);
}

void BlockadeCommand::setButtonPosition(float x, float y)
{
	button.setPosition(x, y);
}

void BlockadeCommand::setButtonSize(float x, float y)
{
	button.setSize(x, y, 12);
}