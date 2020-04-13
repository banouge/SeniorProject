#include "AIHandler.h"
#include "ArmyHandler.h"
#include "MovementCommand.h"
#include "TurnHandler.h"

Map* AIHandler::map = nullptr;
std::unordered_set<std::unordered_set<Player*>*>* AIHandler::teams = nullptr;
std::unordered_map<Player*, std::unordered_set<Territory*>*> AIHandler::playerBorders;
std::unordered_map<Player*, std::unordered_set<Territory*>*> AIHandler::playerTerritories;

void AIHandler::initialize(Map* map, std::unordered_set<std::unordered_set<Player*>*>* teams)
{
	AIHandler::map = map;
	AIHandler::teams = teams;

	for (std::unordered_set<Player*>* team : *teams)
	{
		for (Player* player : *team)
		{
			playerBorders.emplace(player, new std::unordered_set<Territory*>());
			playerTerritories.emplace(player, new std::unordered_set<Territory*>());
		}
	}
}

void AIHandler::cleanUp()
{
	for (std::pair<Player*, std::unordered_set<Territory*>*> pair : playerBorders)
	{
		delete pair.second;
	}
	
	map = nullptr;
	teams = nullptr;

	playerBorders.clear();
}

void AIHandler::createCommands(Player* player)
{
	std::unordered_set<Territory*>* borders = new std::unordered_set<Territory*>();
	std::unordered_set<Territory*>* territories = new std::unordered_set<Territory*>();
	std::unordered_map<Territory*, std::unordered_set<Territory*>*> hostileToOwned;
	std::unordered_map<Territory*, std::unordered_set<Territory*>*> ownedToHostile;
	std::unordered_set<MovementCommand*> moves;
	bool shouldSurrender = true;

	for (Territory* territory : player->getTerritories())
	{
		territories->emplace(territory);
		
		//condition for surrender: no new territories gained
		shouldSurrender = shouldSurrender && playerTerritories.at(player)->count(territory);

		for (Territory* neighbor : territory->getNeighbors())
		{
			if (!player->getTerritories().count(neighbor) && !player->hasTeammate(neighbor->getOwner()))
			{
				if (!hostileToOwned.count(neighbor))
				{
					std::unordered_set<Territory*>* set = new std::unordered_set<Territory*>();
					hostileToOwned.emplace(neighbor, set);
				}

				if (!ownedToHostile.count(territory))
				{
					std::unordered_set<Territory*>* set = new std::unordered_set<Territory*>();
					ownedToHostile.emplace(territory, set);
				}

				hostileToOwned.at(neighbor)->emplace(territory);
				ownedToHostile.at(territory)->emplace(neighbor);
				borders->emplace(territory);

				//condition for surrender: all borders used to be interior
				shouldSurrender = shouldSurrender && playerTerritories.at(player)->count(territory) && !playerBorders.at(player)->count(territory);
			}
		}
	}

	for (Territory* territory : *playerBorders.at(player))
	{
		//condition for surrender: all former borders were lost
		shouldSurrender = shouldSurrender && !territories->count(territory);
	}

	delete playerTerritories.at(player);
	playerTerritories.erase(player);
	playerTerritories.emplace(player, territories);
	delete playerBorders.at(player);
	playerBorders.erase(player);
	playerBorders.emplace(player, borders);

	if (shouldSurrender)
	{
		player->surrender();
	}
	else
	{
		//deploy and attack
		while (player->getRemainingIncome() && hostileToOwned.size())
		{
			std::pair<Territory*, std::unordered_set<Territory*>*> connection = *hostileToOwned.begin();
			Territory* owned = nullptr;
			int remainingIncome = player->getRemainingIncome();
			int numNeeded = ArmyHandler::getNumAttackersNeeded(connection.first->getTotalArmies(), connection.first->hasGeneral());
			int maxAvailable = numNeeded - 1;

			for (Territory* territory : *connection.second)
			{
				int numAvailable = player->getAvailableArmies(territory) + remainingIncome;

				if (numAvailable > maxAvailable)
				{
					maxAvailable = numAvailable;
					owned = territory;
				}
			}

			if (owned)
			{
				int incomeNeeded = numNeeded - player->getAvailableArmies(owned);

				if (incomeNeeded > 0)
				{
					player->addArmiesToTerritory(owned, incomeNeeded);
				}

				moves.emplace((MovementCommand*)(player->createMovementCommand(owned, connection.first, numNeeded)));
			}

			delete connection.second;
			hostileToOwned.erase(connection.first);
		}

		if (player->getRemainingIncome())
		{
			if (moves.size())
			{
				int extraArmies = (int)ceil((float)player->getRemainingIncome() / (float)moves.size());

				for (MovementCommand* command : moves)
				{
					player->addArmiesToTerritory(command->TERRITORY, extraArmies);
					player->createMovementCommand(command->TERRITORY, command->DESTINATION, command->NUM_ARMIES + player->getAvailableArmies(command->TERRITORY));
				}
			}
			else if (ownedToHostile.size())
			{
				int extraArmies = (int)ceil((float)player->getRemainingIncome() / (float)ownedToHostile.size());

				for (std::pair<Territory*, std::unordered_set<Territory*>*> pair : ownedToHostile)
				{
					player->addArmiesToTerritory(pair.first, extraArmies);
				}
			}
			else
			{
				int extraArmies = (int)ceil((float)player->getRemainingIncome() / (float)territories->size());

				for (Territory* territory : *territories)
				{
					player->addArmiesToTerritory(territory, extraArmies);
				}
			}
		}

		TurnHandler::submitCommands(player);
	}

	for (std::pair<Territory*, std::unordered_set<Territory*>*> pair : hostileToOwned)
	{
		delete pair.second;
	}

	for (std::pair<Territory*, std::unordered_set<Territory*>*> pair : ownedToHostile)
	{
		delete pair.second;
	}
}