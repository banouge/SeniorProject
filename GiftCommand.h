#pragma once

#include "Command.h"
#include "Territory.h"

class GiftCommand : public Command
{
public:
	GiftCommand(Player* commander, Territory* territory, Player* newOwner);

	Player* const NEW_OWNER;

	static void setBracket(bool isEarly);
	static int getBracket();

	void resolve();

private:
	static int bracket;
};
