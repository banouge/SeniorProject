#include "CardCommand.h"

CardCommand::CardCommand(Player* commander, int bracket, int numArmies, Territory* territory, int cost, int index) : Command(commander, bracket, numArmies, territory), COST(cost), INDEX(index)
{
}