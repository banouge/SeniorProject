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

}

bool TurnHandler::comparePlayerPtr(Player* a, Player* b)
{
	return a->INDEX < b->INDEX;
}