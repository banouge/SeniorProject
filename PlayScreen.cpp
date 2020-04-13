#include "MapGenerator.h"
#include "PlayScreen.h"

sf::RenderWindow* PlayScreen::window = nullptr;
TextButton PlayScreen::quitButton = TextButton("QUIT GAME", sf::Color(255, 0, 0, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::endTurnButton = TextButton("END TURN", sf::Color(255, 0, 0, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::deployPhaseButton = TextButton("DEPLOY\nPHASE", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::commandPhaseButton = TextButton("COMMAND\nPHASE", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::cardsButton = TextButton("CARDS", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::analyzeButton = TextButton("ANALYZE\nATTACK", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
sf::RectangleShape PlayScreen::commandsFrame;
std::unordered_set<TextButton*> PlayScreen::playersButtons;
bool PlayScreen::hasInitialized = false;
void* PlayScreen::button = nullptr;
Map* PlayScreen::map = nullptr;
std::unordered_set<std::unordered_set<Player*>*>* PlayScreen::teams = nullptr;
std::unordered_set<sf::Keyboard::Key> PlayScreen::keysPressed;
sf::Vector2f PlayScreen::origin;

void PlayScreen::run(sf::RenderWindow* w, std::string mapName, std::unordered_set<std::unordered_set<Player*>*>* teams)
{
	window = w;

	if (!hasInitialized)
	{
		initialize();
	}

	map = new Map(mapName);
	PlayScreen::teams = teams;
	start();

	while (window->isOpen())
	{
		sf::Event ev;

		while (window->pollEvent(ev))
		{
			switch (ev.type)
			{
			case sf::Event::MouseButtonPressed:
				mouseDown();
				break;
			case sf::Event::MouseButtonReleased:
				mouseUp();
				break;
			case sf::Event::KeyPressed:
				keyDown(ev.key.code);
				break;
			case sf::Event::KeyReleased:
				keyUp(ev.key.code);
				break;
			default:
				break;
			}
		}

		update();
		draw();
	}

	cleanUp();
}

void PlayScreen::initialize()
{
	hasInitialized = true;

	quitButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	quitButton.setPosition(0.0f, 0.9f * window->getSize().y);

	endTurnButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	endTurnButton.setPosition(0.0f, 0.4f * window->getSize().y);

	deployPhaseButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	deployPhaseButton.setPosition(0.0f, 0.0f);

	commandPhaseButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	commandPhaseButton.setPosition(0.0f, 0.1f * window->getSize().y);

	cardsButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	cardsButton.setPosition(0.0f, 0.2f * window->getSize().y);

	analyzeButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	analyzeButton.setPosition(0.0f, 0.3f * window->getSize().y);

	commandsFrame.setPosition(0.9f * window->getSize().x, 0.0f);
	commandsFrame.setSize(sf::Vector2f(0.1f * window->getSize().x, window->getSize().y));
	commandsFrame.setOutlineThickness(3.0f);
	commandsFrame.setOutlineColor(sf::Color(0, 0, 0, 255));
	commandsFrame.setFillColor(sf::Color(128, 128, 128, 255));
}

void PlayScreen::start()
{
	int teamIndex = 0;
	int playerIndex = 0;
	int numPlayers = 0;
	float y = 0.5f * window->getSize().y;

	for (std::unordered_set<Player*>* team : *teams)
	{
		for (Player* player : *team)
		{
			++numPlayers;
		}
	}

	float height = 0.4f * window->getSize().y / (float)numPlayers;

	for (std::unordered_set<Player*>* team : *teams)
	{
		for (Player* player : *team)
		{
			TextButton* playerButton = new TextButton("Player " + std::to_string(player->INDEX), player->COLOR, sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
			playersButtons.emplace(playerButton);
			playerButton->setSize(0.075f * window->getSize().x, height);
			playerButton->setPosition(0.025f * window->getSize().x, y);
			y += height;
		}

		TextButton* teamButton = new TextButton("Team " + MapGenerator::convertNumberToLetters(++teamIndex), sf::Color(0, 192, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
		float width = 0.4f * window->getSize().y * team->size() / (float)numPlayers;
		playersButtons.emplace(teamButton);
		teamButton->setSize(width, 0.025f * window->getSize().x);
		teamButton->setPosition(0.0f, y);
		teamButton->setRotation(-90.0f);
	}
}

void PlayScreen::update()
{
	sf::Vector2f offset;

	if (keysPressed.count(sf::Keyboard::Up) || keysPressed.count(sf::Keyboard::W))
	{
		offset -= sf::Vector2f(0.0f, 1.0f);
	}
	else if (keysPressed.count(sf::Keyboard::Down) || keysPressed.count(sf::Keyboard::S))
	{
		offset += sf::Vector2f(0.0f, 1.0f);
	}

	if (keysPressed.count(sf::Keyboard::Right) || keysPressed.count(sf::Keyboard::D))
	{
		offset += sf::Vector2f(1.0f, 0.0f);
	}
	else if (keysPressed.count(sf::Keyboard::Left) || keysPressed.count(sf::Keyboard::A))
	{
		offset -= sf::Vector2f(1.0f, 0.0f);
	}

	if (offset != sf::Vector2f())
	{
		origin += offset;
		map->setOrigin(origin);
	}
}

void PlayScreen::cleanUp()
{
	for (std::unordered_set<Player*>* team : *teams)
	{
		for (Player* player : *team)
		{
			delete player;
		}

		delete team;
	}

	delete teams;
	delete map;

	playersButtons.clear();
	keysPressed.clear();
}

void PlayScreen::draw()
{
	window->clear();
	map->draw(window);
	quitButton.draw(window);
	endTurnButton.draw(window);
	deployPhaseButton.draw(window);
	commandPhaseButton.draw(window);
	cardsButton.draw(window);
	analyzeButton.draw(window);
	window->draw(commandsFrame);

	for (TextButton* textButton : playersButtons)
	{
		textButton->draw(window);
	}

	window->display();
}

void PlayScreen::mouseDown()
{
	if (quitButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &quitButton;
	}
}

void PlayScreen::mouseUp()
{
	if (button == &quitButton)
	{
		if (quitButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			window->close();
		}
	}

	button = nullptr;
}

void PlayScreen::keyDown(sf::Keyboard::Key key)
{
	keysPressed.emplace(key);
}

void PlayScreen::keyUp(sf::Keyboard::Key key)
{
	keysPressed.erase(key);
}