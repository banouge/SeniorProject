#include "Resources.h"

sf::Font Resources::arialFont = sf::Font();

void Resources::load()
{
	arialFont.loadFromFile("Resources/arial.ttf");
}