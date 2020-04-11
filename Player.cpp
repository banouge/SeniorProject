#include "AirliftCommand.h"
#include "BlockadeCommand.h"
#include "GiftCommand.h"
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
	if (availableArmies.at(source) >= numArmies && numArmies > 0 && source->getOwner() == this)
	{
		Command* command = (Command*)(new MovementCommand(this, source, destination, numArmies, hasGeneral, canAttackTeammates, canAttack, canTransfer));
		int remainingArmies = availableArmies.at(source) - numArmies;
		availableArmies.erase(source);
		availableArmies.emplace(source, remainingArmies);
		commands.emplace(command);
		movementCommands.push_back(command);
		return command;
	}

	return nullptr;
}

Command* Player::createAirliftCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral)
{
	if (availableArmies.at(source) >= numArmies && numArmies > 0 && source->getOwner() == this)
	{
		Command* command = (Command*)(new AirliftCommand(this, source, destination, numArmies, hasGeneral));
		int remainingArmies = availableArmies.at(source) - numArmies;
		availableArmies.erase(source);
		availableArmies.emplace(source, remainingArmies);
		commands.emplace(command);
		airliftCommands.push_back(command);
		return command;
	}

	return nullptr;
}

Command* Player::createBlockadeCommand(Territory* territory)
{
	if (territory->getOwner() == this)
	{
		Command* command = (Command*)(new BlockadeCommand(this, territory));
		commands.emplace(command);
		ownershipCommands.push_back(command);
		return command;
	}

	return nullptr;
}

Command* Player::createGiftCommand(Territory* territory, Player* newOwner)
{
	if (territory->getOwner() == this)
	{
		Command* command = (Command*)(new GiftCommand(this, territory, newOwner));
		commands.emplace(command);
		ownershipCommands.push_back(command);
		return command;
	}

	return nullptr;
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
	ownershipCommands.clear();
	availableArmies.clear();

	for (Territory* territory : territories)
	{
		availableArmies.erase(territory);
		availableArmies.emplace(territory, territory->getTotalArmies());
	}
}

void Player::moveCommand(int from, int to, std::vector<Command*>& vector)
{
	if (from > -1 && to > -1 && from < vector.size() && to < vector.size())
	{
		if (to > from)
		{
			for (int c = from; c < to; ++c)
			{
				std::iter_swap(vector.begin() + c, vector.begin() + c + 1);
			}
		}
		else
		{
			for (int c = from; c > to; --c)
			{
				std::iter_swap(vector.begin() + c, vector.begin() + c - 1);
			}
		}
	}
}

void Player::removeCommand(int index, std::vector<Command*>& vector)
{
	if (index > -1 && index < vector.size())
	{
		int remainingArmies = availableArmies.at(vector.at(index)->TERRITORY) + vector.at(index)->NUM_ARMIES;
		availableArmies.erase(vector.at(index)->TERRITORY);
		availableArmies.emplace(vector.at(index)->TERRITORY, remainingArmies);
		vector.erase(vector.begin() + index);
	}
}

int Player::getNumGenerals()
{
	return numGenerals;
}

int Player::getNumTerritories()
{
	return territories.size();
}

std::vector<Command*>& Player::getMovementCommands()
{
	return movementCommands;
}

std::vector<Command*>& Player::getAirliftCommands()
{
	return airliftCommands;
}

std::vector<Command*>& Player::getOwnershipCommands()
{
	return ownershipCommands;
}

void Player::lose()
{
	if (isStillAlive)
	{
		isStillAlive = false;

		while (!territories.empty())
		{
			(*territories.begin())->setOwner(nullptr);
		}
	}
}