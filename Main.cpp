#include "Map.h"
#include "Player.h"

int main()
{
	Map m("Map 2");
	Territory* t1 = m.getTerritoryAtPoint(sf::Vector2i(1300, 500));
	Territory* t2 = m.getTerritoryAtPoint(sf::Vector2i(1500, 700));
	Territory* t3 = m.getTerritoryAtPoint(sf::Vector2i(1260, 540));
	Territory* t4 = m.getTerritoryAtPoint(sf::Vector2i(1020, 100));
	Player p1(false);
	Player p2(true);

	p1.setNumGenerals(1);
	p2.setNumGenerals(1);

	t1->setOwner(&p1);
	t1->setTotalArmies(30);
	t1->addGeneral();
	t1->rejuvenateArmies();
	t2->setOwner(&p2);
	t2->setTotalArmies(30);
	t2->addGeneral();
	t2->rejuvenateArmies();

	p1.clearCommands(true);
	p2.clearCommands(true);

	p1.createMovementCommand(t1, t2, 1);
	p1.createMovementCommand(t1, t2, 2);
	p1.createMovementCommand(t1, t2, 3);

	p1.moveCommand(0, 2, p1.getMovementCommands());
	p1.moveCommand(1, 0, p1.getMovementCommands());

	p1.removeCommand(1, p1.getMovementCommands());
}