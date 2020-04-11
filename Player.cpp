#include "AirliftCommand.h"
#include "MovementCommand.h"
#include "Player.h"
#include "TurnHandler.h"

Player::Player(bool isAi): IS_AI(isAi)
{
	numGenerals = 0;
	isStillAlive = true;
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

Command* Player::createAirliftCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral)
{
	Command* command = (Command*)(new AirliftCommand(this, source, destination, numArmies, hasGeneral));
	commands.emplace(command);
	airliftCommands.push_back(command);
	return command;
}

bool Player::hasTeammate(Player* player)
{
	return teammates.count(player);
}

bool Player::isAlive()
{
	return isStillAlive;
}

void Player::loseGeneral()
{
	if (--numGenerals <= 0)
	{
		lose();
	}
}

void Player::setNumGenerals(int num)
{
	numGenerals = num;
}

void Player::addTeammate(Player* player)
{
	teammates.emplace(player);
}

void Player::addTerritory(Territory* territory)
{
	territories.emplace(territory);
}

void Player::removeTerritory(Territory* territory)
{
	territories.erase(territory);

	if (territories.empty())
	{
		lose();
	}
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
	airliftCommands.clear();
}

int Player::getNumGenerals()
{
	return numGenerals;
}

void Player::lose()
{
	isStillAlive = false;

	while (!territories.empty())
	{
		(*territories.begin())->setOwner(nullptr);
	}
}