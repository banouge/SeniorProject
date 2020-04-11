#include "GiftCommand.h"
#include "Player.h"

int GiftCommand::bracket = 4;

GiftCommand::GiftCommand(Player* commander, Territory* territory, Player* newOwner) : Command(commander, bracket), COMMANDER(commander), TERRITORY(territory), NEW_OWNER(newOwner)
{
}

void GiftCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (4) : (8);
}

void GiftCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER && !TERRITORY->hasGeneral() && COMMANDER->getNumTerritories() > 1 && NEW_OWNER->isAlive() && COMMANDER != NEW_OWNER)
	{
		TERRITORY->setOwner(NEW_OWNER);
	}

	delete this;
}