#include "MapGenerator.h"
#include "PlayScreen.h"
#include "Resources.h"

int main()
{
	sf::RenderWindow window;

	Resources::load();
	
	window.create(sf::VideoMode::getDesktopMode(), "Shot in the Dark", sf::Style::Fullscreen);

	std::unordered_set<std::unordered_set<Player*>*>* teams = new std::unordered_set<std::unordered_set<Player*>*>();
	std::unordered_set<Player*>* teamA = new std::unordered_set<Player*>();
	std::unordered_set<Player*>* teamB = new std::unordered_set<Player*>();
	Player* player1 = new Player(1, false, Map::getRandomColor());
	Player* player2 = new Player(2, true, Map::getRandomColor());
	Player* player3 = new Player(3, true, Map::getRandomColor());
	Player* player4 = new Player(4, true, Map::getRandomColor());

	teams->emplace(teamA);
	teams->emplace(teamB);
	teamA->emplace(player1);
	teamB->emplace(player2);
	teamA->emplace(player3);
	teamB->emplace(player4);

	PlayScreen::run(&window, "Map 3", teams);

	window.close();
}