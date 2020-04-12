#include "PlayScreen.h"

sf::RenderWindow* PlayScreen::window = nullptr;
TextButton PlayScreen::quitButton = TextButton("QUIT GAME", sf::Color(255, 0, 0, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
bool PlayScreen::hasInitialized = false;
void* PlayScreen::button = nullptr;

void PlayScreen::run(sf::RenderWindow* w)
{
	window = w;

	if (!hasInitialized)
	{
		initialize();
	}

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
			default:
				break;
			}
		}

		draw();
	}
}

void PlayScreen::draw()
{
	window->clear();
	quitButton.draw(window);
	window->display();
}

void PlayScreen::initialize()
{
	hasInitialized = true;

	quitButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	quitButton.setPosition(0.0f, 0.9f * window->getSize().y);
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