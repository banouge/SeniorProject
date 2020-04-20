#include "AirliftCommand.h"
#include "BlockadeCommand.h"
#include "DeploymentCommand.h"
#include "GiftCommand.h"
#include "MovementCommand.h"
#include "Player.h"
#include "TurnHandler.h"

const sf::Color Player::NEUTRAL_COLOR = sf::Color(255, 255, 255, 255);
const sf::Color Player::FOG_COLOR = sf::Color(128, 128, 128, 255);
int Player::baseIncome = 5;
std::random_device Player::seed;
std::mt19937 Player::rng(seed());

Player::Player(int index, bool isAi, sf::Color color) : IS_AI(isAi), INDEX(index), COLOR(color)
{
	numGenerals = 0;
	isStillAlive = true;
	income = baseIncome;

	for (int b = 0; b < 10; ++b)
	{
		commandBrackets[b] = new std::vector<Command*>();
	}

	for (int c = 0; c < 10; ++c)
	{
		cardPieces[c] = 0;
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

void Player::setBaseIncome(int base)
{
	baseIncome = base;
}

Command* Player::createDeploymentCommand(Territory* territory, int numArmies)
{
	int index = 0;

	for (; index < commandBrackets[DeploymentCommand::getBracket()]->size(); ++index)
	{
		DeploymentCommand* deploymentCommand = (DeploymentCommand*)(commandBrackets[DeploymentCommand::getBracket()]->at(index));

		if (deploymentCommand->TERRITORY == territory)
		{
			removeCommand(index, commandBrackets[deploymentCommand->BRACKET]);
			break;
		}
	}

	numArmies = (usedIncome + numArmies > income) ? (income - usedIncome) : (numArmies);

	if (territory->getOwner() == this && numArmies > 0)
	{
		Command* command = (Command*)(new DeploymentCommand(this, territory, numArmies));
		int remainingArmies = availableArmies.at(territory) + numArmies;
		usedIncome += numArmies;
		availableArmies.erase(territory);
		availableArmies.emplace(territory, remainingArmies);
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);

		if (index < commandBrackets[command->BRACKET]->size() - 1)
		{
			moveCommand(commandBrackets[command->BRACKET]->size() - 1, index, commandBrackets[command->BRACKET]);
		}

		command->setButtonSize(0.1f * window->getSize().x, 0.05f * window->getSize().y);
		updateCommandButtonPositions();
		return command;
	}

	return nullptr;
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

	numArmies = (numArmies > availableArmies.at(source)) ? (availableArmies.at(source)) : (numArmies);

	if (source->getOwner() == this && source->hasNeighbor(destination) && (numArmies > 0 || hasGeneral))
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

		command->setButtonSize(0.1f * window->getSize().x, 0.05f * window->getSize().y);
		updateCommandButtonPositions();
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

	numArmies = (numArmies > availableArmies.at(source)) ? (availableArmies.at(source)) : (numArmies);
	bool willPlaneBeAbleToLand = (destination->getOwner() == this && (numArmies > 0 || hasGeneral)) || (hasTeammate(destination->getOwner()) && numArmies > 0);

	if (cardPieces[AirliftCommand::INDEX] >= AirliftCommand::getNumPieces() && source->getOwner() == this && willPlaneBeAbleToLand)
	{
		Command* command = (Command*)(new AirliftCommand(this, source, destination, numArmies, hasGeneral));
		int remainingArmies = availableArmies.at(source) - numArmies;
		cardPieces[AirliftCommand::INDEX] -= AirliftCommand::getNumPieces();
		availableArmies.erase(source);
		availableArmies.emplace(source, remainingArmies);
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);
		command->setButtonSize(0.1f * window->getSize().x, 0.05f * window->getSize().y);
		updateCommandButtonPositions();
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

	if (cardPieces[BlockadeCommand::INDEX] >= BlockadeCommand::getNumPieces() && territory->getOwner() == this)
	{
		Command* command = (Command*)(new BlockadeCommand(this, territory));
		cardPieces[BlockadeCommand::INDEX] -= BlockadeCommand::getNumPieces();
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);
		command->setButtonSize(0.1f * window->getSize().x, 0.05f * window->getSize().y);
		updateCommandButtonPositions();
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

	if (cardPieces[GiftCommand::INDEX] >= GiftCommand::getNumPieces() && territory->getOwner() == this)
	{
		Command* command = (Command*)(new GiftCommand(this, territory, newOwner));
		cardPieces[GiftCommand::INDEX] -= GiftCommand::getNumPieces();
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);
		command->setButtonSize(0.1f * window->getSize().x, 0.05f * window->getSize().y);
		updateCommandButtonPositions();
		return command;
	}

	return nullptr;
}

Command* Player::addArmiesToTerritory(Territory* territory, int numArmies)
{
	int index = 0;

	for (; index < commandBrackets[DeploymentCommand::getBracket()]->size(); ++index)
	{
		DeploymentCommand* deploymentCommand = (DeploymentCommand*)(commandBrackets[DeploymentCommand::getBracket()]->at(index));

		if (deploymentCommand->TERRITORY == territory)
		{
			numArmies += deploymentCommand->NUM_ARMIES;
			removeCommand(index, commandBrackets[deploymentCommand->BRACKET]);
			break;
		}
	}

	numArmies = (usedIncome + numArmies > income) ? (income - usedIncome) : (numArmies);

	if (territory->getOwner() == this && numArmies > 0)
	{
		Command* command = (Command*)(new DeploymentCommand(this, territory, numArmies));
		int remainingArmies = availableArmies.at(territory) + numArmies;
		usedIncome += numArmies;
		availableArmies.erase(territory);
		availableArmies.emplace(territory, remainingArmies);
		commands.emplace(command);
		commandBrackets[command->BRACKET]->push_back(command);

		if (index < commandBrackets[command->BRACKET]->size() - 1)
		{
			moveCommand(commandBrackets[command->BRACKET]->size() - 1, index, commandBrackets[command->BRACKET]);
		}

		command->setButtonSize(0.1f * window->getSize().x, 0.05f * window->getSize().y);
		updateCommandButtonPositions();
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

bool Player::isInDeployPhase()
{
	return isDeploying;
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

void Player::setPhase(bool isDeployPhase)
{
	isDeploying = isDeployPhase;
}

void Player::addIncome(int amount)
{
	income += amount;
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
	isDeploying = true;
	usedIncome = 0;

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
		int remainingArmies = 0;

		if (vector->at(index)->BRACKET == 1)
		{
			//removing deployment
			remainingArmies = availableArmies.at(vector->at(index)->TERRITORY) - vector->at(index)->NUM_ARMIES;
			usedIncome -= vector->at(index)->NUM_ARMIES;
		}
		else if (vector->at(index)->BRACKET == 5)
		{
			//removing movement
			remainingArmies = availableArmies.at(vector->at(index)->TERRITORY) + vector->at(index)->NUM_ARMIES;
		}
		else
		{
			//removing card
			CardCommand* command = (CardCommand*)(vector->at(index));
			remainingArmies = availableArmies.at(command->TERRITORY) + command->NUM_ARMIES;
			cardPieces[command->INDEX] += command->COST;
		}

		commands.erase(vector->at(index));
		availableArmies.erase(vector->at(index)->TERRITORY);
		availableArmies.emplace(vector->at(index)->TERRITORY, remainingArmies);
		delete vector->at(index);
		vector->erase(vector->begin() + index);
	}
}

void Player::gainCard()
{
	int sumOfWeights = AirliftCommand::getWeight() + BlockadeCommand::getWeight() + GiftCommand::getWeight();
	cardPieces[AirliftCommand::INDEX] += AirliftCommand::getMinPiecesPerTurn();
	cardPieces[BlockadeCommand::INDEX] += BlockadeCommand::getMinPiecesPerTurn();
	cardPieces[GiftCommand::INDEX] += GiftCommand::getMinPiecesPerTurn();

	if (sumOfWeights)
	{
		std::uniform_int_distribution<int> distribution(0, sumOfWeights - 1);
		int index = distribution(rng);

		if (index < AirliftCommand::getWeight())
		{
			cardPieces[AirliftCommand::INDEX] += 1;
			return;
		}

		index -= AirliftCommand::getWeight();

		if (index < BlockadeCommand::getWeight())
		{
			cardPieces[BlockadeCommand::INDEX] += 1;
			return;
		}

		cardPieces[GiftCommand::INDEX] += 1;
	}
}

void Player::surrender()
{
	clearCommands();
	TurnHandler::submitCommands(this);
	lose();
}

void Player::setWindow(sf::RenderWindow* window)
{
	this->window = window;
}

void Player::drawCommands()
{
	for (Command* command : commands)
	{
		command->draw(window);
	}
}

void Player::updateCommandButtonPositions()
{
	int index = 0;
	float x = 0.9f * window->getSize().x;
	float height = 0.05f * window->getSize().y;

	for (std::vector<Command*>* bracket : commandBrackets)
	{
		for (Command* command : *bracket)
		{
			command->setButtonPosition(x, height * index++);
		}
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

int Player::getRemainingIncome()
{
	return income - usedIncome;
}

int Player::getAvailableArmies(Territory* territory)
{
	return (availableArmies.count(territory)) ? (availableArmies.at(territory)) : (-1);
}

int Player::getDeployedArmies(Territory* territory)
{
	for (Command* command : *commandBrackets[DeploymentCommand::getBracket()])
	{
		if (command->TERRITORY == territory)
		{
			return command->NUM_ARMIES;
		}
	}

	return income - usedIncome;
}

int Player::getMovedArmies(Territory* source, Territory* destination)
{
	for (Command* command : *commandBrackets[MovementCommand::getBracket()])
	{
		if (command->TERRITORY == source && ((MovementCommand*)command)->DESTINATION == destination)
		{
			return command->NUM_ARMIES;
		}
	}

	return availableArmies.at(source);
}

int Player::getAirliftedArmies(Territory* source, Territory* destination)
{
	for (Command* command : *commandBrackets[AirliftCommand::getBracket()])
	{
		if (command->TERRITORY == source && ((AirliftCommand*)command)->DESTINATION == destination)
		{
			return command->NUM_ARMIES;
		}
	}

	return availableArmies.at(source);
}

int Player::getNumCardPieces(int cardIndex)
{
	return cardPieces[cardIndex];
}

std::vector<Command*>* Player::getCommandsInBracket(int bracket)
{
	return commandBrackets[bracket];
}

std::unordered_set<Territory*>& Player::getTerritories()
{
	return territories;
}

void Player::lose()
{
	if (isStillAlive)
	{
		isStillAlive = false;

		while (!territories.empty())
		{
			(*territories.begin())->removeGeneral();
			(*territories.begin())->setOwner(nullptr);
		}
	}
}