#include "GiftCommand.h"
#include "Player.h"

int GiftCommand::bracket = 3;

GiftCommand::GiftCommand(Player* commander, Territory* territory, Player* newOwner) : Command(commander, bracket, 0, territory), NEW_OWNER(newOwner)
{
}

void GiftCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (3) : (7);
}

int GiftCommand::getBracket()
{
	return bracket;
}

void GiftCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER && !TERRITORY->hasGeneral() && COMMANDER->getNumTerritories() > 1 && NEW_OWNER->isAlive() && COMMANDER != NEW_OWNER)
	{
		TERRITORY->setOwner(NEW_OWNER);
	}
}