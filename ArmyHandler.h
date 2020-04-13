#pragma once

#include <random>

class ArmyHandler
{
public:
	ArmyHandler();

	static void setIsWeightedRound(bool isWeighted);
	static void setLuckRate(int luckFactor);
	static void setGeneralValue(int value);
	static int getGeneralValue();
	static int getNumArmiesKilled(int numEnemies, int killRate, bool hasGeneral = false);
	static int getNumAttackersKilled(int numDefenders, bool doDefendersHaveGeneral = false);
	static int getNumDefendersKilled(int numAttackers, bool doAttackersHaveGeneral = false);
	static int getNumArmiesNeeded(int numEnemies, int killRate, bool hasGeneral = false);
	static int getNumAttackersNeeded(int numDefenders, bool doDefendersHaveGeneral = false);
	static int getNumDefendersNeeded(int numAttackers, bool doAttackersHaveGeneral = false);
	static int weightedRound(double x);

private:
	static bool isWeightedRound;
	static double luckRate;
	static int generalValue;
	static int offensiveKillRate;
	static int defensiveKillRate;
	static std::normal_distribution<double> normalDistribution;
	static std::uniform_real_distribution<double> uniformDistribution;
	static std::random_device seed;
	static std::mt19937 rng;
};