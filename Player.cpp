#include "MovementCommand.h"
#include "Player.h"
#include "TurnHandler.h"

Player::Player(bool isAi): IS_AI(isAi)
{
}

Player::~Player()
{
	for (Command* command : commands)
	{
		delete command;
	}
}

Command* Player::createMovementCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral, bool canAttackTeammates, bool canAttack, bool canTransfer)
{
	Command* command = (Command*)(new MovementCommand(this, source, destination, numArmies, hasGeneral, canAttackTeammates, canAttack, canTransfer));
	commands.emplace(command);
	movementCommands.push_back(command);
	return command;
}

bool Player::hasTeammate(Player* player)
{
	return teammates.count(player);
}

void Player::loseGeneral()
{
	--numGenerals;
}

void Player::setNumGenerals(int num)
{
	numGenerals = num;
}

void Player::addTeammate(Player* player)
{
	teammates.emplace(player);
}

void Player::clearCommands(bool haveCommandsResolved)
{
	if (!haveCommandsResolved)
	{
		for (Command* command : commands)
		{
			delete command;
		}
	}

	commands.clear();
	movementCommands.clear();
}

int Player::getNumGenerals()
{
	return numGenerals;
}