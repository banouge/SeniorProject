#pragma once

#include "Command.h"
#include "Player.h"
#include "Territory.h"

class CardCommand : public Command
{
public:
	CardCommand(Player* commander, int bracket, int numArmies, Territory* territory, int cost, int index);

	const int COST;
	const int INDEX;

	virtual void resolve() = 0;
	virtual void draw(sf::RenderWindow* window) = 0;
	virtual void setButtonPosition(float x, float y) = 0;
	virtual void setButtonSize(float x, float y) = 0;
};