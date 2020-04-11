#pragma once

#include "Command.h"
#include "Territory.h"

class DeploymentCommand : public Command
{
public:
	DeploymentCommand(Player* commander, Territory* territory, int numArmies);

	static int getBracket();

	void resolve();
};