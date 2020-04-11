#pragma once

#include "Territory.h"

class Player;

class Command
{
public:
	Command(Player* commander, int bracket, int numArmies, Territory* territory);

	const int BRACKET;
	const int NUM_ARMIES;
	Player* const COMMANDER;
	Territory* const TERRITORY;

	virtual void resolve() = 0;
};