#pragma once

#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "SFML/Graphics.hpp"

class Command;
class Territory;
class TurnHandler;

class Player
{
public:
	Player(int index, bool isAi, sf::Color color);
	~Player();

	static const sf::Color NEUTRAL_COLOR;
	static const sf::Color FOG_COLOR;

	const bool IS_AI;
	const int INDEX;
	const sf::Color COLOR;

	static void setBaseIncome(int base);

	Command* createDeploymentCommand(Territory* territory, int numArmies);
	Command* createMovementCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false, bool canAttackTeammates = false, bool canAttack = true, bool canTransfer = true);
	Command* createAirliftCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false);
	Command* createBlockadeCommand(Territory* territory);
	Command* createGiftCommand(Territory* territory, Player* newOwner);
	Command* addArmiesToTerritory(Territory* territory, int numArmies);
	bool hasTeammate(Player* player);
	bool isAlive();
	bool hasSubmittedCommands();
	bool hasCapturedNewTerritoryThisTurn();
	bool isInDeployPhase();
	void loseGeneral();
	void setNumGenerals(int num);
	void setHasSubmittedCommands(bool areSubmitted);
	void setPhase(bool isDeployPhase);
	void addIncome(int amount);
	void addTeammate(Player* player);
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);
	void clearCommands();
	void moveCommand(int from, int to, std::vector<Command*>* vector);
	void removeCommand(int index, std::vector<Command*>* vector);
	void gainCard();
	void surrender();
	int getNumGenerals();
	int getNumTerritories();
	int getRemainingIncome();
	int getAvailableArmies(Territory* territory);
	int getDeployedArmies(Territory* territory);
	int getMovedArmies(Territory* source, Territory* destination);
	std::vector<Command*>* getCommandsInBracket(int bracket);
	std::unordered_set<Territory*>& getTerritories();

private:
	static int baseIncome;
	static std::random_device seed;
	static std::mt19937 rng;

	std::unordered_set<Territory*> territories;
	std::unordered_map<Territory*, int> availableArmies;
	std::unordered_set<Command*> commands;
	std::vector<Command*>* commandBrackets[10];
	std::unordered_set<Player*> teammates;
	int cardPieces[10];
	int income;
	int usedIncome;
	int numGenerals;
	bool isStillAlive;
	bool areCommandsSubmitted;
	bool isDeploying;

	void lose();
};