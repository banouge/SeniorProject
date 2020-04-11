#include "Map.h"
#include "Player.h"
#include "TurnHandler.h"

int main()
{
	Map m("Map 2");
	Territory* t1 = m.getTerritoryAtPoint(sf::Vector2i(66, 850));
	Territory* t2 = m.getTerritoryAtPoint(sf::Vector2i(1020, 100));
	Territory* t3 = m.getTerritoryAtPoint(sf::Vector2i(47, 489));
	Territory* t4 = m.getTerritoryAtPoint(sf::Vector2i(8, 740));
	Player p1(1, false);
	Player p2(2, true);

	p1.setNumGenerals(1);
	p2.setNumGenerals(1);

	t1->setOwner(&p1);
	t1->setTotalArmies(30);
	t1->addGeneral();
	t2->setOwner(&p2);
	t2->setTotalArmies(2);
	t2->addGeneral();
	t3->setOwner(&p1);
	t3->setTotalArmies(1);
	t4->setOwner(&p1);
	t4->setTotalArmies(1);

	p1.clearCommands();
	p2.clearCommands();

	p2.createDeploymentCommand(t2, 3);
	p2.createDeploymentCommand(t2, 10);
	p2.createMovementCommand(t2, t4, 1);
	p2.createMovementCommand(t2, t3, 1);
	p2.createMovementCommand(t2, t1, 0, true);

	p1.createDeploymentCommand(t1, 5);
	p1.createMovementCommand(t1, t2, 24);
	p1.createMovementCommand(t1, t3, 5);
	p1.createMovementCommand(t1, t2, 29, true);
	p1.createAirliftCommand(t3, t1, 1);
	p1.createGiftCommand(t3, &p2);
	p1.createBlockadeCommand(t4);

	TurnHandler::submitCommands(&p1);
	TurnHandler::submitCommands(&p2);

	TurnHandler::resolveTurn();
	//TODO: restrict move to neighbors, restrict cards based on cards
}