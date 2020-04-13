#include "TextButton.h"
#include "Resources.h"

const double RADIANS_PER_DEGREE = 0.01745329251994329577;

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

void TextButton::setRotation(float theta)
{
	float s = sinf(theta * RADIANS_PER_DEGREE);
	float c = cosf(theta * RADIANS_PER_DEGREE);

	button.setRotation(theta);
	text.setRotation(theta);
	text.setPosition(button.getPosition().x + 0.5f * (button.getSize().x * c - button.getSize().y * s), button.getPosition().y + 0.5f * (button.getSize().x * s + button.getSize().y * c));
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