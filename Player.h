#pragma once

#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Command;
class Territory;
class TurnHandler;

class Player
{
public:
	Player(int index, bool isAi);
	~Player();

	bool IS_AI;
	int INDEX;

	static void setBaseIncome(int base);

	Command* createDeploymentCommand(Territory* territory, int numArmies);
	Command* createMovementCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false, bool canAttackTeammates = false, bool canAttack = true, bool canTransfer = true);
	Command* createAirliftCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false);
	Command* createBlockadeCommand(Territory* territory);
	Command* createGiftCommand(Territory* territory, Player* newOwner);
	bool hasTeammate(Player* player);
	bool isAlive();
	bool hasSubmittedCommands();
	bool hasCapturedNewTerritoryThisTurn();
	void loseGeneral();
	void setNumGenerals(int num);
	void setHasSubmittedCommands(bool areSubmitted);
	void addIncome(int amount);
	void addTeammate(Player* player);
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);
	void clearCommands();
	void moveCommand(int from, int to, std::vector<Command*>* vector);
	void removeCommand(int index, std::vector<Command*>* vector);
	void gainCard();
	int getNumGenerals();
	int getNumTerritories();
	std::vector<Command*>* getCommandsInBracket(int bracket);

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

	void lose();
};