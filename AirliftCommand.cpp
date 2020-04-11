#include "AirliftCommand.h"
#include "Player.h"

int AirliftCommand::bracket = 3;

AirliftCommand::AirliftCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral) : Command(commander, bracket, numArmies, source), COMMANDER(commander), SOURCE(source), DESTINATION(destination), NUM_ARMIES(numArmies), HAS_GENERAL(hasGeneral)
{
}

void AirliftCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (3) : (7);
}

void AirliftCommand::resolve()
{
	if (SOURCE->getOwner() == COMMANDER)
	{
		if (DESTINATION->getOwner() == COMMANDER)
		{
			int numArmies = (SOURCE->getNumArmies() < NUM_ARMIES) ? (SOURCE->getNumArmies()) : (NUM_ARMIES);
			SOURCE->removeArmies(numArmies, false);
			DESTINATION->addArmies(numArmies);

			if (HAS_GENERAL && SOURCE->hasGeneral() && !SOURCE->isGeneralExhausted() && !DESTINATION->hasGeneral())
			{
				SOURCE->removeGeneral();
				DESTINATION->addGeneral();
			}
		}
		else if (COMMANDER->hasTeammate(DESTINATION->getOwner()))
		{
			int numArmies = (SOURCE->getNumArmies() < NUM_ARMIES) ? (SOURCE->getNumArmies()) : (NUM_ARMIES);
			SOURCE->removeArmies(numArmies, false);
			DESTINATION->addArmies(numArmies);
		}
	}

	delete this;
}