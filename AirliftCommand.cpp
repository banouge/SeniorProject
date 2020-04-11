#include "AirliftCommand.h"
#include "Player.h"

int AirliftCommand::bracket = 2;

AirliftCommand::AirliftCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral) : Command(commander, bracket, numArmies, source), DESTINATION(destination), HAS_GENERAL(hasGeneral)
{
}

void AirliftCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (2) : (6);
}

int AirliftCommand::getBracket()
{
	return bracket;
}

void AirliftCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER)
	{
		if (DESTINATION->getOwner() == COMMANDER)
		{
			int numArmies = (TERRITORY->getNumArmies() < NUM_ARMIES) ? (TERRITORY->getNumArmies()) : (NUM_ARMIES);
			TERRITORY->removeArmies(numArmies, false);
			DESTINATION->addArmies(numArmies);

			if (HAS_GENERAL && TERRITORY->hasGeneral() && !TERRITORY->isGeneralExhausted() && !DESTINATION->hasGeneral())
			{
				TERRITORY->removeGeneral();
				DESTINATION->addGeneral();
			}
		}
		else if (COMMANDER->hasTeammate(DESTINATION->getOwner()))
		{
			int numArmies = (TERRITORY->getNumArmies() < NUM_ARMIES) ? (TERRITORY->getNumArmies()) : (NUM_ARMIES);
			TERRITORY->removeArmies(numArmies, false);
			DESTINATION->addArmies(numArmies);
		}
	}
}