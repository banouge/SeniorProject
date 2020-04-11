#include "ArmyHandler.h"
#include "DeploymentCommand.h"
#include "Player.h"

DeploymentCommand::DeploymentCommand(Player* commander, Territory* territory, int numArmies) : Command(commander, 1, numArmies, territory)
{
}

int DeploymentCommand::getBracket()
{
	return 1;
}

void DeploymentCommand::resolve()
{
	TERRITORY->addArmies(NUM_ARMIES);
	TERRITORY->rejuvenateArmies();
}