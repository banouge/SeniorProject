#include "AirliftCommand.h"
#include "BlockadeCommand.h"
#include "GiftCommand.h"
#include "MovementCommand.h"
#include "Player.h"
#include "TurnHandler.h"

Player::Player(int index, bool isAi) : IS_AI(isAi), INDEX(index)
{
	numGenerals = 0;
	isStillAlive = true;

	for (int b = 0; b < 10; ++b)
	{
		commandBrackets[b] = new std::vector<Command*>();
	}
}

Player::~Player()
{
	for (std::vector<Command*>* bracket : commandBrackets)
	{
		delete bracket;
	}

	for (Command* command : commands)
	{
		delete command;
	}
}

Command* Player::createMovementCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral, bool canAttackTeammates, bool canAttack, bool canTransfer)
{
	int index = 0;

	for (; index < commandBrackets[MovementCommand::getBracket()]->size(); ++index)
	{
		MovementCommand* movementCommand = (MovementCommand*)(commandBrackets[MovementCommand::getBracket()]->at(index));

		if (movementCommand->TERRITORY == source && movementCommand->DESTINATION == destination)
		{
			removeCommand(index, commandBrackets[movementCommand->BRACKET]);
			break;
		}
	}

	if (source->getOwner() == this && availableArmies.at(source) >= numArmies && (numArmies > 0 || hasGeneral))
	{
		Command* command = (Command*)(new MovementCommand(this, source, destination, numArmies, hasGeneral, canAttackTeammates, canAttack, canTransfer));
		int remainingArmies = availableArmies.at(source) - numArmies;
		availableArmies.erase(source);
		availableArmies.emplace(source, remainingArmies);
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);

		if (index < commandBrackets[command->BRACKET]->size() - 1)
		{
			moveCommand(commandBrackets[command->BRACKET]->size() - 1, index, commandBrackets[command->BRACKET]);
		}

		return command;
	}

	return nullptr;
}

Command* Player::createAirliftCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral)
{
	int index = 0;

	for (; index < commandBrackets[AirliftCommand::getBracket()]->size(); ++index)
	{
		AirliftCommand* airliftCommand = (AirliftCommand*)(commandBrackets[AirliftCommand::getBracket()]->at(index));

		if (airliftCommand->TERRITORY == source && airliftCommand->DESTINATION == destination)
		{
			removeCommand(index, commandBrackets[airliftCommand->BRACKET]);
			break;
		}
	}

	bool willPlaneBeAbleToLand = (destination->getOwner() == this && (numArmies > 0 || hasGeneral)) || (hasTeammate(destination->getOwner()) && numArmies > 0);

	if (source->getOwner() == this && availableArmies.at(source) >= numArmies && willPlaneBeAbleToLand)
	{
		Command* command = (Command*)(new AirliftCommand(this, source, destination, numArmies, hasGeneral));
		int remainingArmies = availableArmies.at(source) - numArmies;
		availableArmies.erase(source);
		availableArmies.emplace(source, remainingArmies);
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);
		return command;
	}

	return nullptr;
}

Command* Player::createBlockadeCommand(Territory* territory)
{
	for (Command* command : *commandBrackets[BlockadeCommand::getBracket()])
	{
		if (command->TERRITORY == territory)
		{
			return nullptr;
		}
	}

	for (Command* command : *commandBrackets[GiftCommand::getBracket()])
	{
		if (command->TERRITORY == territory)
		{
			return nullptr;
		}
	}

	if (territory->getOwner() == this)
	{
		Command* command = (Command*)(new BlockadeCommand(this, territory));
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);
		return command;
	}

	return nullptr;
}

Command* Player::createGiftCommand(Territory* territory, Player* newOwner)
{
	for (Command* command : *commandBrackets[BlockadeCommand::getBracket()])
	{
		if (command->TERRITORY == territory)
		{
			return nullptr;
		}
	}

	for (Command* command : *commandBrackets[GiftCommand::getBracket()])
	{
		if (command->TERRITORY == territory)
		{
			return nullptr;
		}
	}

	if (territory->getOwner() == this)
	{
		Command* command = (Command*)(new GiftCommand(this, territory, newOwner));
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);
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

bool Player::hasSubmittedCommands()
{
	return areCommandsSubmitted;
}

bool Player::hasCapturedNewTerritoryThisTurn()
{
	for (Territory* territory : territories)
	{
		if (territory->wasCapturedThisTurn())
		{
			return true;
		}
	}

	return false;
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

void Player::setHasSubmittedCommands(bool areSubmitted)
{
	areCommandsSubmitted = areSubmitted;
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

void Player::clearCommands()
{
	for (Command* command : commands)
	{
		delete command;
	}

	for (std::vector<Command*>* bracket : commandBrackets)
	{
		bracket->clear();
	}

	commands.clear();
	availableArmies.clear();
	areCommandsSubmitted = false;

	for (Territory* territory : territories)
	{
		availableArmies.emplace(territory, territory->getTotalArmies());
		territory->rejuvenateArmies();
	}
}

void Player::moveCommand(int from, int to, std::vector<Command*>* vector)
{
	if (from > -1 && to > -1 && from < vector->size() && to < vector->size())
	{
		if (to > from)
		{
			for (int c = from; c < to; ++c)
			{
				std::iter_swap(vector->begin() + c, vector->begin() + c + 1);
			}
		}
		else
		{
			for (int c = from; c > to; --c)
			{
				std::iter_swap(vector->begin() + c, vector->begin() + c - 1);
			}
		}
	}
}

void Player::removeCommand(int index, std::vector<Command*>* vector)
{
	if (index > -1 && index < vector->size())
	{
		int remainingArmies = availableArmies.at(vector->at(index)->TERRITORY) + vector->at(index)->NUM_ARMIES;
		availableArmies.erase(vector->at(index)->TERRITORY);
		availableArmies.emplace(vector->at(index)->TERRITORY, remainingArmies);
		vector->erase(vector->begin() + index);
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

std::vector<Command*>* Player::getCommandsInBracket(int bracket)
{
	return commandBrackets[bracket];
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