#pragma once

#include "Map.h"
#include "TextButton.h"

class PlayScreen
{
public:
	static void run(sf::RenderWindow* w, std::string mapName);

private:
	static sf::RenderWindow* window;
	static TextButton quitButton;
	static bool hasInitialized;
	static void* button;
	static Map* map;

	static void initialize();
	static void draw();
	static void mouseDown();
	static void mouseUp();
};