#pragma once

#include <unordered_set>
#include <vector>
#include "Command.h"

class Territory;
class TurnHandler;

class Player
{
public:
	Player(bool isAi);
	~Player();

	bool IS_AI;

	Command* createMovementCommand(Territory* source, Territory* destination, int numArmies, bool hasGeneral = false, bool canAttackTeammates = false, bool canAttack = true, bool canTransfer = true);
	bool hasTeammate(Player* player);
	void loseGeneral();
	void setNumGenerals(int num);
	void addTeammate(Player* player);
	void clearCommands(bool haveCommandsResolved);
	int getNumGenerals();

private:
	std::unordered_set<Command*> commands;
	std::vector<Command*> movementCommands;
	std::unordered_set<Player*> teammates;
	int numGenerals;
};