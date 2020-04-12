#include "TextButton.h"
#include "Resources.h"

TextButton::TextButton(std::string string, sf::Color buttonColor, sf::Color buttonOutlineColor, sf::Color textColor, sf::Color textOutlineColor)
{
	this->string = string;

	button.setOutlineThickness(3.0f);
	button.setOutlineColor(buttonOutlineColor);
	button.setFillColor(buttonColor);

	text.setOutlineThickness(3.0f);
	text.setOutlineColor(textOutlineColor);
	text.setFillColor(textColor);
	text.setFont(Resources::arialFont);
}

void TextButton::setSize(float width, float height)
{
	button.setSize(sf::Vector2f(width, height));

	text.setString(string);
	text.setCharacterSize(30);
	text.setOrigin(0.5f * text.getLocalBounds().width, 0.5f * text.getLocalBounds().height);
}

void TextButton::setPosition(float x, float y)
{
	button.setPosition(x, y);
	text.setPosition(0.5f * button.getSize().x + x, 0.5f * button.getSize().y + y);
}

void TextButton::draw(sf::RenderWindow* window)
{
	window->draw(button);
	window->draw(text);
}

bool TextButton::doesContainPoint(sf::Vector2i point)
{
	return button.getGlobalBounds().contains(point.x, point.y);
}