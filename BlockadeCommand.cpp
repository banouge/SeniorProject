#include "BlockadeCommand.h"
#include "Player.h"

int BlockadeCommand::bracket = 4;
float BlockadeCommand::multiplier = 3.0f;

BlockadeCommand::BlockadeCommand(Player* commander, Territory* territory) : Command(commander, bracket), COMMANDER(commander), TERRITORY(territory)
{
}

void BlockadeCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (4) : (8);
}

void BlockadeCommand::setMultiplier(float m)
{
	multiplier = m;
}

void BlockadeCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER && !TERRITORY->hasGeneral() && COMMANDER->getNumTerritories() > 1)
	{
		TERRITORY->setTotalArmies((int)round(multiplier * TERRITORY->getTotalArmies()));
		TERRITORY->setOwner(nullptr);
	}

	delete this;
}