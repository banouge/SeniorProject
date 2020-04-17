#pragma once

#include <string>
#include "SFML/Graphics.hpp"

class TextButton
{
public:
	TextButton(std::string string, sf::Color buttonColor, sf::Color buttonOutlineColor, sf::Color textColor, sf::Color textOutlineColor, bool canTypeDigits = false);

	const bool CAN_TYPE_DIGITS;

	void setSize(float width, float height);
	void setPosition(float x, float y);
	void setRotation(float theta);
	void addCharacter(char character);
	void draw(sf::RenderWindow* window);
	bool doesContainPoint(sf::Vector2i point);
	std::string getString();

private:
	sf::RectangleShape button;
	sf::Text text;
	std::string string;
};