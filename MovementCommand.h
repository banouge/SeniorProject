#pragma once

#include "Command.h"
#include "Territory.h"

class MovementCommand : public Command
{
public:
	MovementCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral, bool canAttackTeammates, bool canAttack, bool canTransfer);

	Territory* const DESTINATION;
	const bool HAS_GENERAL;
	const bool CAN_ATTACK_TEAMMATES;
	const bool CAN_ATTACK;
	const bool CAN_TRANSFER;

	static int getBracket();

	void resolve();

private:
	void attack();
};