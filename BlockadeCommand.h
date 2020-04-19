#pragma once

#include "CardCommand.h"
#include "TextButton.h"

class BlockadeCommand : public CardCommand
{
public:
	BlockadeCommand(Player* commander, Territory* territory);

	static const int INDEX;

	static void setBracket(bool isEarly);
	static void setMultiplier(float m);
	static int getBracket();
	static void setWeight(int w);
	static int getWeight();
	static void setNumPieces(int n);
	static int getNumPieces();
	static void setMinPiecesPerTurn(int n);
	static int getMinPiecesPerTurn();

	void resolve();
	void draw(sf::RenderWindow* window);
	void setButtonPosition(float x, float y);
	void setButtonSize(float x, float y);

private:
	static int bracket;
	static int weight;
	static int numPieces;
	static int minPiecesPerTurn;
	static float multiplier;

	TextButton button;
};