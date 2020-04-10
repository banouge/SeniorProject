#include "Map.h"
#include "MapGenerator.h"

int main()
{
	Map m("Map 2");
	Territory* t = m.getTerritoryAtPoint(sf::Vector2i(1300, 500));
}