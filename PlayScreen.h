#pragma once

#include "Map.h"
#include "TextButton.h"

class PlayScreen
{
public:
	static void run(sf::RenderWindow* w, std::string mapName, std::unordered_set<std::unordered_set<Player*>*>* teams);

private:
	static sf::RenderWindow* window;
	static TextButton quitButton;
	static TextButton endTurnButton;
	static TextButton deployPhaseButton;
	static TextButton commandPhaseButton;
	static TextButton cardsButton;
	static TextButton analyzeButton;
	static sf::RectangleShape commandsFrame;
	static std::unordered_set<TextButton*> playersButtons;
	static bool hasInitialized;
	static void* button;
	static Map* map;
	static std::unordered_set<std::unordered_set<Player*>*>* teams;
	static std::unordered_set<sf::Keyboard::Key> keysPressed;
	static sf::Vector2f origin;
	static std::vector<Player*> players;

	static void initialize();
	static void start();
	static void handleTurn();
	static void pan();
	static void cleanUp();
	static void draw();
	static void mouseDown();
	static void mouseUp();
	static void keyDown(sf::Keyboard::Key key);
	static void keyUp(sf::Keyboard::Key key);
};