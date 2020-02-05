#include "ArmyHandler.h"

bool ArmyHandler::isWeightedRound = false;
double ArmyHandler::luckRate = 0.0;
std::normal_distribution<double> ArmyHandler::normalDistribution = std::normal_distribution<double>(0.0, 1.0);
std::uniform_real_distribution<double> ArmyHandler::uniformDistribution = std::uniform_real_distribution<double>(0.0, 1.0);
std::random_device ArmyHandler::seed;
std::mt19937 ArmyHandler::rng(seed());

ArmyHandler::ArmyHandler()
{
}

void ArmyHandler::setIsWeightedRound(bool isWeighted)
{
	isWeightedRound = isWeighted;
}

void ArmyHandler::setLuckRate(int luckFactor)
{
	luckRate = luckFactor / 100.0;
}

int ArmyHandler::getNumArmiesKilled(int numEnemies, int killRate)
{
	//get mean
	double numArmies = numEnemies * killRate / 100.0;

	//add offset
	numArmies += numArmies * luckRate * normalDistribution(rng);

	//set to 0 if negative
	numArmies = (numArmies < 0.0) ? (0.0) : (numArmies);

	//round
	return (isWeightedRound) ? (weightedRound(numArmies)) : ((int)round(numArmies));
}

int ArmyHandler::weightedRound(double x)
{
	double floorX = floor(x);
	return (uniformDistribution(rng) < x - floorX) ? (ceil(x)) : (floorX);
}