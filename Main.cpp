#include "ArmyHandler.h"

int main()
{
	ArmyHandler::setIsWeightedRound(true);
	int a = ArmyHandler::getNumArmiesKilled(10, 60);
	ArmyHandler::setLuckRate(10);
	a = ArmyHandler::getNumArmiesKilled(10, 60);
	a = ArmyHandler::getNumArmiesKilled(10, 60);
	a = ArmyHandler::getNumArmiesKilled(10, 60);
}