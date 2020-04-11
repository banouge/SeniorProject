#pragma once

#include "Command.h"
#include "Territory.h"

class AirliftCommand : Command
{
public:
	AirliftCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral);

	const int NUM_ARMIES;
	Player* const COMMANDER;
	Territory* const SOURCE;
	Territory* const DESTINATION;
	const bool HAS_GENERAL;

	static void setBracket(bool isEarly);

	void resolve();

private:
	static int bracket;
};