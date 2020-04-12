#include "Player.h"
#include "TurnHandler.h"

std::unordered_map<Player*, std::unordered_map<int, std::queue<Command*>*>*> TurnHandler::playerMap = std::unordered_map<Player*, std::unordered_map<int, std::queue<Command*>*>*>();

void TurnHandler::submitCommands(Player* player)
{
	std::unordered_map<int, std::queue<Command*>*>* queueMap = new std::unordered_map<int, std::queue<Command*>*>();
	playerMap.emplace(player, queueMap);

	for (int b = 0; b < 10; ++b)
	{
		if (player->getCommandsInBracket(b)->size())
		{
			std::queue<Command*>* queue = new std::queue<Command*>();
			queueMap->emplace(player->getCommandsInBracket(b)->at(0)->BRACKET, queue);

			for (Command* command : *player->getCommandsInBracket(b))
			{
				queue->push(command);
			}
		}
	}

	player->setHasSubmittedCommands(true);
}

void TurnHandler::resolveTurn()
{
	std::vector<Player*> players;
	
	for (std::pair<Player*, std::unordered_map<int, std::queue<Command*>*>*> pair : playerMap)
	{
		players.push_back(pair.first);
	}

	std::sort(players.begin(), players.end(), comparePlayerPtr);

	for (int b = 0; b < 10; ++b)
	{
		bool isForward = true;
		bool isEmpty = true;

		do
		{
			isForward = !isForward;
			isEmpty = true;

			for (int p = 0; p < players.size(); ++p)
			{
				Player* player = players.at((isForward) ? (p) : (players.size() - p - 1));
				
				if (playerMap.at(player)->count(b) && player->isAlive())
				{
					std::queue<Command*>* queue = playerMap.at(player)->at(b);

					if (!queue->empty())
					{
						isEmpty = false;
						queue->front()->resolve();
						queue->pop();
					}
				}
			}
		}
		while (!isEmpty);
	}

	for (Player* player : players)
	{
		if (player->hasCapturedNewTerritoryThisTurn())
		{
			player->gainCard();
		}

		player->clearCommands();
	}
}

bool TurnHandler::comparePlayerPtr(Player* a, Player* b)
{
	return a->INDEX < b->INDEX;
}