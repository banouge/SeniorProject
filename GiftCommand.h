#pragma once

#include "Command.h"
#include "Territory.h"

class GiftCommand : Command
{
public:
	GiftCommand(Player* commander, Territory* territory, Player* newOwner);

	Player* const COMMANDER;
	Player* const NEW_OWNER;
	Territory* const TERRITORY;

	static void setBracket(bool isEarly);

	void resolve();

private:
	static int bracket;
};
