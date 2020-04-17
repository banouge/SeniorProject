#include "ArmyHandler.h"

bool ArmyHandler::isWeightedRound = false;
double ArmyHandler::luckRate = 0.0;
std::normal_distribution<double> ArmyHandler::normalDistribution = std::normal_distribution<double>(0.0, 1.0);
std::uniform_real_distribution<double> ArmyHandler::uniformDistribution = std::uniform_real_distribution<double>(0.0, 1.0);
std::random_device ArmyHandler::seed;
int ArmyHandler::generalValue = 10;
int ArmyHandler::offensiveKillRate = 60;
int ArmyHandler::defensiveKillRate = 70;
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

void ArmyHandler::setGeneralValue(int value)
{
	generalValue = value;
}

int ArmyHandler::getGeneralValue()
{
	return generalValue;
}

int ArmyHandler::getNumArmiesKilled(int numEnemies, int killRate, bool hasGeneral)
{
	//get mean
	double numArmies = (hasGeneral) ? ((numEnemies + generalValue) * killRate / 100.0) : (numEnemies * killRate / 100.0);

	//add offset
	numArmies += numArmies * luckRate * normalDistribution(rng);

	//set to 0 if negative
	numArmies = (numArmies < 0.0) ? (0.0) : (numArmies);

	//round
	return (isWeightedRound) ? (weightedRound(numArmies)) : ((int)round(numArmies));
}

int ArmyHandler::getNumAttackersKilled(int numDefenders, bool doDefendersHaveGeneral)
{
	return getNumArmiesKilled(numDefenders, defensiveKillRate, doDefendersHaveGeneral);
}

int ArmyHandler::getNumDefendersKilled(int numAttackers, bool doAttackersHaveGeneral)
{
	return getNumArmiesKilled(numAttackers, offensiveKillRate, doAttackersHaveGeneral);
}

int ArmyHandler::getNumArmiesNeeded(int numEnemies, int killRate, bool hasGeneral)
{
	return (int)round((hasGeneral) ? ((numEnemies + generalValue) * 100.0 / killRate) : (numEnemies * 100.0 / killRate));
}

//how many attackers are needed to kill n defenders?
int ArmyHandler::getNumAttackersNeeded(int numDefenders, bool doDefendersHaveGeneral)
{
	return getNumArmiesNeeded(numDefenders, offensiveKillRate, doDefendersHaveGeneral);
}

//how many defenders are needed to kill n attackers?
int ArmyHandler::getNumDefendersNeeded(int numAttackers, bool doAttackersHaveGeneral)
{
	return (defensiveKillRate) ? (getNumArmiesNeeded(numAttackers, defensiveKillRate, doAttackersHaveGeneral)) : (-1);
}

//how many attackers are needed to capture a territory with n defenders?
int ArmyHandler::getNumAttackersForCapture(int numDefenders, bool doDefendersHaveGeneral)
{
	if (numDefenders || doDefendersHaveGeneral)
	{
		if (offensiveKillRate)
		{
			int numNeeded = getNumAttackersNeeded(numDefenders, doDefendersHaveGeneral);
			int numLost = getNumAttackersKilled(numDefenders, doDefendersHaveGeneral);

			return (numNeeded > numLost) ? (numNeeded) : (numLost + 1);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 1;
	}
}

int ArmyHandler::weightedRound(double x)
{
	double floorX = floor(x);
	return (uniformDistribution(rng) < x - floorX) ? ((int)ceil(x)) : ((int)floorX);
}