#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

class Command;
class Territory;
class TurnHandler;

class Player
{
public:
	Player(bool isAi);
	~Player();

	bool IS_AI;

	Command* createMovementCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false, bool canAttackTeammates = false, bool canAttack = true, bool canTransfer = true);
	Command* createAirliftCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false);
	Command* createBlockadeCommand(Territory* territory);
	Command* createGiftCommand(Territory* territory, Player* newOwner);
	bool hasTeammate(Player* player);
	bool isAlive();
	void loseGeneral();
	void setNumGenerals(int num);
	void addTeammate(Player* player);
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);
	void clearCommands(bool haveCommandsResolved);
	void moveCommand(int from, int to, std::vector<Command*>& vector);
	void removeCommand(int index, std::vector<Command*>& vector);
	int getNumGenerals();
	int getNumTerritories();
	std::vector<Command*>& getMovementCommands();
	std::vector<Command*>& getAirliftCommands();
	std::vector<Command*>& getOwnershipCommands();

private:
	std::unordered_set<Territory*> territories;
	std::unordered_map<Territory*, int> availableArmies;
	std::unordered_set<Command*> commands;
	std::vector<Command*> movementCommands;
	std::vector<Command*> airliftCommands;
	std::vector<Command*> ownershipCommands;
	std::unordered_set<Player*> teammates;
	int numGenerals;
	bool isStillAlive;

	void lose();
};