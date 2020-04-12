#pragma once

#include "CardCommand.h"

class AirliftCommand : public CardCommand
{
public:
	AirliftCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral);

	static const int INDEX;

	Territory* const DESTINATION;
	const bool HAS_GENERAL;

	static void setBracket(bool isEarly);
	static int getBracket();
	static void setWeight(int w);
	static int getWeight();
	static void setNumPieces(int n);
	static int getNumPieces();
	static void setMinPiecesPerTurn(int n);
	static int getMinPiecesPerTurn();

	void resolve();

private:
	static int bracket;
	static int weight;
	static int numPieces;
	static int minPiecesPerTurn;
};