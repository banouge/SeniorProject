#include "AirliftCommand.h"

const int AirliftCommand::INDEX = 0;
int AirliftCommand::bracket = 2;
int AirliftCommand::weight = 0;
int AirliftCommand::numPieces = 1;
int AirliftCommand::minPiecesPerTurn = 0;

AirliftCommand::AirliftCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral) : CardCommand(commander, bracket, numArmies, source, numPieces, INDEX), DESTINATION(destination), HAS_GENERAL(hasGeneral), button("", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255))
{
	if (numArmies)
	{
		if (numArmies == 1)
		{
			if (hasGeneral)
			{
				button.setString("1 army and a general fly from " + source->getName() + " to " + destination->getName() + ".");
			}
			else
			{
				button.setString("1 army flies from " + source->getName() + " to " + destination->getName() + ".");
			}
		}
		else
		{
			if (hasGeneral)
			{
				button.setString(std::to_string(numArmies) + " armies and a general fly from " + source->getName() + " to " + destination->getName() + ".");
			}
			else
			{
				button.setString(std::to_string(numArmies) + " armies fly from " + source->getName() + " to " + destination->getName() + ".");
			}
		}
	}
	else if (hasGeneral)
	{
		button.setString("A general flies from " + source->getName() + " to " + destination->getName() + ".");
	}
}

void AirliftCommand::setBracket(bool isEarly)
{
	bracket = (isEarly) ? (2) : (6);
}

int AirliftCommand::getBracket()
{
	return bracket;
}

void AirliftCommand::setWeight(int w)
{
	weight = w;
}

int AirliftCommand::getWeight()
{
	return weight;
}

void AirliftCommand::setNumPieces(int n)
{
	numPieces = n;
}

int AirliftCommand::getNumPieces()
{
	return numPieces;
}

void AirliftCommand::setMinPiecesPerTurn(int n)
{
	minPiecesPerTurn = n;
}

int AirliftCommand::getMinPiecesPerTurn()
{
	return minPiecesPerTurn;
}

void AirliftCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER)
	{
		if (DESTINATION->getOwner() == COMMANDER)
		{
			int numArmies = (TERRITORY->getNumArmies() < NUM_ARMIES) ? (TERRITORY->getNumArmies()) : (NUM_ARMIES);
			TERRITORY->removeArmies(numArmies, false);
			DESTINATION->addArmies(numArmies);

			if (HAS_GENERAL && TERRITORY->hasGeneral() && !TERRITORY->isGeneralExhausted() && !DESTINATION->hasGeneral())
			{
				TERRITORY->removeGeneral();
				DESTINATION->addGeneral();
			}
		}
		else if (COMMANDER->hasTeammate(DESTINATION->getOwner()))
		{
			int numArmies = (TERRITORY->getNumArmies() < NUM_ARMIES) ? (TERRITORY->getNumArmies()) : (NUM_ARMIES);
			TERRITORY->removeArmies(numArmies, false);
			DESTINATION->addArmies(numArmies);
		}
	}
}

void AirliftCommand::draw(sf::RenderWindow* window)
{
	button.draw(window);
}

void AirliftCommand::setButtonPosition(float x, float y)
{
	button.setPosition(x, y);
}

void AirliftCommand::setButtonSize(float x, float y)
{
	button.setSize(x, y, 12);
}