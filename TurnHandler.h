#pragma once

#include <unordered_map>
#include <queue>
#include "Command.h"

class Player;

class TurnHandler
{
public:
	static void submitCommands(Player* player);
	static void resolveTurn();
	static void cleanUp();

private:
	static std::unordered_map<Player*, std::unordered_map<int, std::queue<Command*>*>*> playerMap;

	static bool comparePlayerPtr(Player* a, Player* b);
};