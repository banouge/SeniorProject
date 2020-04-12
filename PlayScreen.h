#pragma once

#include "TextButton.h"

class PlayScreen
{
public:
	static void run(sf::RenderWindow* w);

private:
	static sf::RenderWindow* window;
	static TextButton quitButton;
	static bool hasInitialized;
	static void* button;

	static void initialize();
	static void draw();
	static void mouseDown();
	static void mouseUp();
};