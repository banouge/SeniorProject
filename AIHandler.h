#pragma once
#include "Map.h"
#include "Player.h"

class AIHandler
{
public:
	static void initialize(Map* map, std::unordered_set<std::unordered_set<Player*>*>* teams);
	static void cleanUp();
	static void createCommands(Player* player);

private:
	static Map* map;
	static std::unordered_set<std::unordered_set<Player*>*>* teams;
	static std::unordered_map<Player*, std::unordered_set<Territory*>*> playerBorders;
	static std::unordered_map<Player*, std::unordered_set<Territory*>*> playerTerritories;
};