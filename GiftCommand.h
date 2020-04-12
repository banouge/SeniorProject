#pragma once

#include "CardCommand.h"

class GiftCommand : public CardCommand
{
public:
	GiftCommand(Player* commander, Territory* territory, Player* newOwner);

	static const int INDEX;

	Player* const NEW_OWNER;

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
