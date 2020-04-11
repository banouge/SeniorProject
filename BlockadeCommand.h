#pragma once

#include "Command.h"
#include "Territory.h"

class BlockadeCommand : Command
{
public:
	BlockadeCommand(Player* commander, Territory* territory);

	Player* const COMMANDER;
	Territory* const TERRITORY;

	static void setBracket(bool isEarly);
	static void setMultiplier(float m);

	void resolve();

private:
	static int bracket;
	static float multiplier;
};