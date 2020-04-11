#include "BlockadeCommand.h"
#include "Player.h"

int BlockadeCommand::bracket = 3;
float BlockadeCommand::multiplier = 3.0f;

BlockadeCommand::BlockadeCommand(Player* commander, Territory* territory) : Command(commander, bracket, 0, territory)
{
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

void BlockadeCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER && !TERRITORY->hasGeneral() && COMMANDER->getNumTerritories() > 1)
	{
		TERRITORY->setTotalArmies((int)round(multiplier * TERRITORY->getTotalArmies()));
		TERRITORY->setOwner(nullptr);
	}
}