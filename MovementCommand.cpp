#include "ArmyHandler.h"
#include "MovementCommand.h"
#include "Player.h"

MovementCommand::MovementCommand(Player* commander, Territory* source, Territory* destination, int numArmies, bool hasGeneral, bool canAttackTeammates, bool canAttack, bool canTransfer) : Command(commander, 5, numArmies, source), DESTINATION(destination), HAS_GENERAL(hasGeneral), CAN_ATTACK_TEAMMATES(canAttackTeammates), CAN_ATTACK(canAttack), CAN_TRANSFER(canTransfer), button("", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255))
{
	std::string verbSingular = "moves";
	std::string verbPlural = "move";
	std::string suffix = (canAttackTeammates) ? (", attacking teammates.") : (".");

	if (!canAttack)
	{
		verbSingular = "reinforces";
		verbPlural = "reinforce";
	}
	else if (!canTransfer)
	{
		verbSingular = "attacks";
		verbPlural = "attack";
	}

	if (numArmies)
	{
		if (numArmies == 1)
		{
			if (hasGeneral)
			{
				button.setString("1 army and a general " + verbPlural + " from " + source->getName() + " to " + destination->getName() + suffix);
			}
			else
			{
				button.setString("1 army " + verbSingular + " from " + source->getName() + " to " + destination->getName() + suffix);
			}
		}
		else
		{
			if (hasGeneral)
			{
				button.setString(std::to_string(numArmies) + " armies and a general " + verbPlural + " from " + source->getName() + " to " + destination->getName() + suffix);
			}
			else
			{
				button.setString(std::to_string(numArmies) + " armies " + verbPlural + " from " + source->getName() + " to " + destination->getName() + suffix);
			}
		}
	}
	else if (hasGeneral)
	{
		button.setString("A general " + verbSingular + " from " + source->getName() + " to " + destination->getName() + suffix);
	}
}

int MovementCommand::getBracket()
{
	return 5;
}

void MovementCommand::resolve()
{
	if (TERRITORY->getOwner() == COMMANDER)
	{
		if (DESTINATION->getOwner() == COMMANDER)
		{
			if (CAN_TRANSFER)
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
		}
		else if (COMMANDER->hasTeammate(DESTINATION->getOwner()))
		{
			if (CAN_ATTACK_TEAMMATES)
			{
				if (CAN_ATTACK)
				{
					attack();
				}
			}
			else if (CAN_TRANSFER)
			{
				int numArmies = (TERRITORY->getNumArmies() < NUM_ARMIES) ? (TERRITORY->getNumArmies()) : (NUM_ARMIES);
				TERRITORY->removeArmies(numArmies, false);
				DESTINATION->addArmies(numArmies);
			}
		}
		else if (CAN_ATTACK)
		{
			attack();
		}
	}
}

void MovementCommand::draw(sf::RenderWindow* window)
{
	button.draw(window);
}

void MovementCommand::attack()
{
	bool hasGeneral = HAS_GENERAL && TERRITORY->hasGeneral() && !TERRITORY->isGeneralExhausted();
	bool hasUninvolvedGeneral = !hasGeneral && TERRITORY->hasGeneral();
	int numArmies = (TERRITORY->getNumArmies() < NUM_ARMIES) ? (TERRITORY->getNumArmies()) : (NUM_ARMIES);
	int numAttackersLost = ArmyHandler::getNumAttackersKilled(DESTINATION->getTotalArmies(), DESTINATION->hasGeneral());
	int numDefendersLost = ArmyHandler::getNumDefendersKilled(numArmies, hasGeneral);

	TERRITORY->removeArmies(numArmies, false);

	if (hasGeneral)
	{
		TERRITORY->removeGeneral();
	}

	if (numDefendersLost > DESTINATION->getTotalArmies())
	{
		numDefendersLost -= DESTINATION->getTotalArmies();
		DESTINATION->setTotalArmies(0);

		if (DESTINATION->hasGeneral() && numDefendersLost >= ArmyHandler::getGeneralValue())
		{
			DESTINATION->removeGeneral();
			DESTINATION->getOwner()->loseGeneral();
		}
	}
	else
	{
		DESTINATION->removeArmies(numDefendersLost, true);
	}

	if (numAttackersLost > numArmies)
	{
		numAttackersLost -= numArmies;
		numArmies = 0;

		if (hasGeneral && numAttackersLost >= ArmyHandler::getGeneralValue())
		{
			hasGeneral = false;
			COMMANDER->loseGeneral();
		}
	}
	else
	{
		numArmies -= numAttackersLost;
	}

	if (!DESTINATION->getTotalArmies() && !DESTINATION->hasGeneral() && (numArmies || hasGeneral))
	{
		//success
		DESTINATION->setOwner(COMMANDER);
		DESTINATION->setTotalArmies(numArmies);
		
		if (hasGeneral)
		{
			DESTINATION->addGeneral();
		}
	}
	else
	{
		//failure
		TERRITORY->addArmies(numArmies);
		
		if (hasGeneral)
		{
			TERRITORY->addGeneral();
		}
	}
}

void MovementCommand::setButtonPosition(float x, float y)
{
	button.setPosition(x, y);
}

void MovementCommand::setButtonSize(float x, float y)
{
	button.setSize(x, y, 12);
}