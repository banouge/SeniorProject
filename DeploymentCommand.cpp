#include "ArmyHandler.h"
#include "DeploymentCommand.h"
#include "Player.h"

DeploymentCommand::DeploymentCommand(Player* commander, Territory* territory, int numArmies) : Command(commander, 1, numArmies, territory), button("", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255))
{
	if (numArmies == 1)
	{
		button.setString(territory->getName() + " gains 1 army.");
	}
	else
	{
		button.setString(territory->getName() + " gains " + std::to_string(numArmies) + " armies.");
	}
}

int DeploymentCommand::getBracket()
{
	return 1;
}

void DeploymentCommand::resolve()
{
	TERRITORY->addArmies(NUM_ARMIES);
	TERRITORY->rejuvenateArmies();
}

void DeploymentCommand::draw(sf::RenderWindow* window)
{
	button.draw(window);
}

void DeploymentCommand::setButtonPosition(float x, float y)
{
	button.setPosition(x, y);
}

void DeploymentCommand::setButtonSize(float x, float y)
{
	button.setSize(x, y, 12);
}