#include "GiftCommand.h"

const int GiftCommand::INDEX = 2;
int GiftCommand::bracket = 3;
int GiftCommand::weight = 0;
int GiftCommand::numPieces = 1;
int GiftCommand::minPiecesPerTurn = 0;

GiftCommand::GiftCommand(Player* commander, Territory* territory, Player* newOwner) : CardCommand(commander, bracket, 0, territory, numPieces, INDEX), NEW_OWNER(newOwner), button("", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255))
{
	button.setString("Player " + std::to_string(newOwner->INDEX) + " gains " + territory->getName() + ".");
}

void GiftCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (3) : (7);
}

int GiftCommand::getBracket()
{
	return bracket;
}

void GiftCommand::setWeight(int w)
{
	weight = w;
}

int GiftCommand::getWeight()
{
	return weight;
}

void GiftCommand::setNumPieces(int n)
{
	numPieces = n;
}

int GiftCommand::getNumPieces()
{
	return numPieces;
}

void GiftCommand::setMinPiecesPerTurn(int n)
{
	minPiecesPerTurn = n;
}

int GiftCommand::getMinPiecesPerTurn()
{
	return minPiecesPerTurn;
}

void GiftCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER && !TERRITORY->hasGeneral() && COMMANDER->getNumTerritories() > 1 && NEW_OWNER->isAlive() && COMMANDER != NEW_OWNER)
	{
		TERRITORY->setOwner(NEW_OWNER);
	}
}

void GiftCommand::draw(sf::RenderWindow* window)
{
	button.draw(window);
}

void GiftCommand::setButtonPosition(float x, float y)
{
	button.setPosition(x, y);
}

void GiftCommand::setButtonSize(float x, float y)
{
	button.setSize(x, y, 12);
}