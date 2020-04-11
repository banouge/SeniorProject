#pragma once

#include "Command.h"
#include "Territory.h"

class BlockadeCommand : public Command
{
public:
	BlockadeCommand(Player* commander, Territory* territory);

	static void setBracket(bool isEarly);
	static void setMultiplier(float m);
	static int getBracket();

	void resolve();

private:
	static int bracket;
	static float multiplier;
};