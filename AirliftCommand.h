#pragma once

#include "Command.h"
#include "Territory.h"

class AirliftCommand : public Command
{
public:
	AirliftCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral);

	Territory* const DESTINATION;
	const bool HAS_GENERAL;

	static void setBracket(bool isEarly);
	static int getBracket();

	void resolve();

private:
	static int bracket;
};