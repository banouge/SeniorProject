#include "Map.h"
#include "Player.h"

int main()
{
	Map m("Map 2");
	Territory* t1 = m.getTerritoryAtPoint(sf::Vector2i(1300, 500));
	Territory* t2 = m.getTerritoryAtPoint(sf::Vector2i(1500, 700));
	Player p1(false);
	Player p2(true);

	t1->setOwner(&p1);
	t1->setTotalArmies(30);
	t1->addGeneral();
	t1->rejuvenateArmies();
	t2->setOwner(&p2);
	t2->setTotalArmies(30);
	t2->addGeneral();
	t2->rejuvenateArmies();

	p1.createMovementCommand(t1, t2, 25, true)->resolve();
	p2.createMovementCommand(t2, t1, 25, true)->resolve();

	t1->setTotalArmies(50);
	t1->rejuvenateArmies();

	p1.createMovementCommand(t1, t2, 10)->resolve();
	p1.createMovementCommand(t1, t2, 30, true)->resolve();
	p1.createMovementCommand(t1, t2, 10)->resolve();

	p1.clearCommands(true);
	p2.clearCommands(true);
}