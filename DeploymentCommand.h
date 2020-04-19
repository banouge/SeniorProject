#pragma once

#include "Command.h"
#include "Territory.h"
#include "TextButton.h"

class DeploymentCommand : public Command
{
public:
	DeploymentCommand(Player* commander, Territory* territory, int numArmies);

	static int getBracket();

	void resolve();
	void draw(sf::RenderWindow* window);
	void setButtonPosition(float x, float y);
	void setButtonSize(float x, float y);

private:
	TextButton button;
};