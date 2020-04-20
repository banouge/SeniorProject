#pragma once

#include "Map.h"
#include "TextButton.h"

class PlayScreen
{
public:
	static void run(sf::RenderWindow* w, std::string mapName, std::unordered_set<std::unordered_set<Player*>*>* teams, Player* player);

private:
	static sf::RenderWindow* window;
	static TextButton quitButton;
	static TextButton endTurnButton;
	static TextButton deployPhaseButton;
	static TextButton commandPhaseButton;
	static TextButton cardsButton;
	static TextButton analyzeButton;
	static TextButton inputButton;
	static TextButton acceptButton;
	static TextButton airliftButton;
	static TextButton blockadeButton;
	static TextButton giftButton;
	static sf::RectangleShape commandsFrame;
	static std::unordered_set<TextButton*> playersButtons;
	static bool hasInitialized;
	static bool areCardsVisible;
	static TextButton* button;
	static Map* map;
	static std::unordered_set<std::unordered_set<Player*>*>* teams;
	static std::unordered_set<sf::Keyboard::Key> keysPressed;
	static sf::Vector2f origin;
	static std::vector<Player*> players;
	static Territory* territory1;
	static Territory* territory2;
	static Player* player;
	static int cardIndex;

	static void initialize();
	static void start();
	static void handleTurn();
	static void updateCards();
	static void createCommand();
	static void createAirlift();
	static void createBlockade();
	static void createGift();
	static void pan();
	static void cleanUp();
	static void draw();
	static void mouseDown();
	static void mouseUp();
	static void handleText(char character);
	static void keyDown(sf::Keyboard::Key key);
	static void keyUp(sf::Keyboard::Key key);
};