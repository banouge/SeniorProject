#pragma once

#include <random>

class ArmyHandler
{
public:
	ArmyHandler();

	static void setIsWeightedRound(bool isWeighted);
	static void setLuckRate(int luckFactor);
	static int getNumArmiesKilled(int numEnemies, int killRate);
	static int weightedRound(double x);

private:
	static bool isWeightedRound;
	static double luckRate;
	static std::normal_distribution<double> normalDistribution;
	static std::uniform_real_distribution<double> uniformDistribution;
	static std::random_device seed;
	static std::mt19937 rng;
};