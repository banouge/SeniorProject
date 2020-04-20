#include "AIHandler.h"
#include "AirliftCommand.h"
#include "BlockadeCommand.h"
#include "GiftCommand.h"
#include "MapGenerator.h"
#include "PlayScreen.h"
#include "TurnHandler.h"

sf::RenderWindow* PlayScreen::window = nullptr;
TextButton PlayScreen::quitButton = TextButton("QUIT GAME", sf::Color(255, 0, 0, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::endTurnButton = TextButton("END TURN", sf::Color(255, 0, 0, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::deployPhaseButton = TextButton("DEPLOY\nPHASE", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::commandPhaseButton = TextButton("COMMAND\nPHASE", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::cardsButton = TextButton("CARDS", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::analyzeButton = TextButton("ANALYZE\nATTACK", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::inputButton = TextButton("", sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255), true, true);
TextButton PlayScreen::acceptButton = TextButton("O", sf::Color(0, 255, 0, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::airliftButton = TextButton("Airlift\n0/0", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::blockadeButton = TextButton("Scorch\n0/0", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
TextButton PlayScreen::giftButton = TextButton("Donate\n0/0", sf::Color(0, 128, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
sf::RectangleShape PlayScreen::commandsFrame;
std::unordered_set<TextButton*> PlayScreen::playersButtons;
bool PlayScreen::hasInitialized = false;
bool PlayScreen::areCardsVisible = false;
TextButton* PlayScreen::button = nullptr;
Map* PlayScreen::map = nullptr;
std::unordered_set<std::unordered_set<Player*>*>* PlayScreen::teams = nullptr;
std::unordered_set<sf::Keyboard::Key> PlayScreen::keysPressed;
sf::Vector2f PlayScreen::origin;
std::vector<Player*> PlayScreen::players;
Territory* PlayScreen::territory1 = nullptr;
Territory* PlayScreen::territory2 = nullptr;
Player* PlayScreen::player = nullptr;
int PlayScreen::cardIndex = -1;

void PlayScreen::run(sf::RenderWindow* w, std::string mapName, std::unordered_set<std::unordered_set<Player*>*>* teams, Player* player)
{
	window = w;

	if (!hasInitialized)
	{
		initialize();
	}

	map = new Map(mapName);
	PlayScreen::teams = teams;
	PlayScreen::player = player;
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
			case sf::Event::TextEntered:
				handleText(ev.text.unicode);
				break;
			default:
				break;
			}
		}

		handleTurn();
		pan();
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

	inputButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	inputButton.setPosition(0.0f, window->getSize().y + 100.0f);

	acceptButton.setSize(0.05f * window->getSize().x, 0.1f * window->getSize().y);
	acceptButton.setPosition(0.0f, window->getSize().y + 100.0f);

	airliftButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	airliftButton.setPosition(0.0f, window->getSize().y + 100.0f);

	blockadeButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	blockadeButton.setPosition(0.0f, window->getSize().y + 100.0f);

	giftButton.setSize(0.1f * window->getSize().x, 0.1f * window->getSize().y);
	giftButton.setPosition(0.0f, window->getSize().y + 100.0f);

	commandsFrame.setPosition(0.9f * window->getSize().x, 0.0f);
	commandsFrame.setSize(sf::Vector2f(0.1f * window->getSize().x, window->getSize().y));
	commandsFrame.setOutlineThickness(3.0f);
	commandsFrame.setOutlineColor(sf::Color(0, 0, 0, 255));
	commandsFrame.setFillColor(sf::Color(128, 128, 128, 255));
}

void PlayScreen::start()
{
	AIHandler::initialize(map, teams);
	updateCards();

	for (std::unordered_set<Player*>* team : *teams)
	{
		for (Player* player : *team)
		{
			players.push_back(player);
			player->setWindow(window);
		}
	}

	int teamIndex = 0;
	int numPlayers = players.size();
	float y = 0.5f * window->getSize().y;
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

			for (Player* teammate : *team)
			{
				if (teammate != player)
				{
					player->addTeammate(teammate);
				}
			}
		}

		TextButton* teamButton = new TextButton("Team " + MapGenerator::convertNumberToLetters(++teamIndex), sf::Color(0, 192, 255, 255), sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255, 255), sf::Color(0, 0, 0, 255));
		float width = 0.4f * window->getSize().y * team->size() / (float)numPlayers;
		playersButtons.emplace(teamButton);
		teamButton->setSize(width, 0.025f * window->getSize().x);
		teamButton->setPosition(0.0f, y);
		teamButton->setRotation(-90.0f);
	}

	int playerIndex = 0;

	for (std::pair<std::string, Region*> pair : map->getRegions())
	{
		for (Territory* territory : pair.second->getTerritories())
		{
			territory->setTotalArmies(Territory::getBaseNeutralArmies());
		}

		if (playerIndex < players.size())
		{
			Territory* territory = *pair.second->getTerritories().begin();
			territory->setOwner(players.at(playerIndex));
			territory->setTotalArmies(Territory::getBaseFriendlyArmies());
			territory->addGeneral();
			players.at(playerIndex)->setNumGenerals(1);
			players.at(playerIndex++)->clearCommands();
		}
	}
}

void PlayScreen::handleTurn()
{
	for (Player* player : players)
	{
		if (!player->hasSubmittedCommands() && player->isAlive())
		{
			if (player->IS_AI)
			{
				AIHandler::createCommands(player);
			}
		}
	}
}

void PlayScreen::updateCards()
{
	airliftButton.setString("Airlift\n" + std::to_string(player->getNumCardPieces(AirliftCommand::INDEX)) + "/" + std::to_string(AirliftCommand::getNumPieces()));
	blockadeButton.setString("Scorch\n" + std::to_string(player->getNumCardPieces(BlockadeCommand::INDEX)) + "/" + std::to_string(BlockadeCommand::getNumPieces()));
	giftButton.setString("Donate\n" + std::to_string(player->getNumCardPieces(GiftCommand::INDEX)) + "/" + std::to_string(GiftCommand::getNumPieces()));
}

void PlayScreen::createCommand()
{
	Territory* clickedTerritory = map->getTerritoryAtPoint(sf::Mouse::getPosition(*window));

	if (clickedTerritory)
	{
		//a territory was clicked
		if (player->isInDeployPhase())
		{
			//deploying
			if (territory1 == clickedTerritory && territory1->getOwner() == player)
			{
				//can deploy
				button = &inputButton;
				inputButton.setString(std::to_string(player->getDeployedArmies(territory1)));
				inputButton.setPosition(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);
				acceptButton.setPosition(sf::Mouse::getPosition(*window).x + 0.1f * window->getSize().x, sf::Mouse::getPosition(*window).y);
			}
			else
			{
				//can't deploy
				territory1 = nullptr;
				territory2 = nullptr;
			}
		}
		else
		{
			//moving
			if (territory2 == clickedTerritory)
			{
				//destination was clicked
				if (territory1->hasNeighbor(territory2))
				{
					//can move
					button = &inputButton;
					inputButton.setString(std::to_string(player->getMovedArmies(territory1, territory2)));
					inputButton.setPosition(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);
					acceptButton.setPosition(sf::Mouse::getPosition(*window).x + 0.1f * window->getSize().x, sf::Mouse::getPosition(*window).y);
				}
				else
				{
					//can't move
					if (territory2->getOwner() == player)
					{
						//picked new source
						territory1 = territory2;
						territory2 = nullptr;
					}
					else
					{
						//didn't pick new source
						territory1 = nullptr;
						territory2 = nullptr;
					}
				}
			}
			else if (territory1 == clickedTerritory)
			{
				//source was clicked
				if (territory1->getOwner() != player)
				{
					//source is invalid
					territory1 = nullptr;
					territory2 = nullptr;
				}
			}
			else
			{
				//dragged
				if (territory2)
				{
					//changed mind about destination
					territory2 = nullptr;
				}
				else
				{
					//changed mind about source
					territory1 = nullptr;
				}
			}
		}
	}
	else
	{
		//no territory was clicked
		territory1 = nullptr;
		territory2 = nullptr;
	}
}

void PlayScreen::createAirlift()
{
	Territory* clickedTerritory = map->getTerritoryAtPoint(sf::Mouse::getPosition(*window));

	if (clickedTerritory)
	{
		//a territory was clicked
		if (territory2 == clickedTerritory)
		{
			//destination was clicked
			if (territory2->getOwner() == player || player->hasTeammate(territory2->getOwner()))
			{
				//can airlift
				button = &inputButton;
				inputButton.setString(std::to_string(player->getAirliftedArmies(territory1, territory2)));
				inputButton.setPosition(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);
				acceptButton.setPosition(sf::Mouse::getPosition(*window).x + 0.1f * window->getSize().x, sf::Mouse::getPosition(*window).y);
			}
			else
			{
				//can't airlift
				territory1 = nullptr;
				territory2 = nullptr;
				cardIndex = -1;
			}
		}
		else if (territory1 == clickedTerritory)
		{
			//source was clicked
			if (territory1->getOwner() != player)
			{
				//source is invalid
				territory1 = nullptr;
				territory2 = nullptr;
				cardIndex = -1;
			}
		}
		else
		{
			//dragged
			if (territory2)
			{
				//changed mind about destination
				territory2 = nullptr;
			}
			else
			{
				//changed mind about source
				territory1 = nullptr;
			}
		}
	}
	else
	{
		//no territory was clicked
		territory1 = nullptr;
		territory2 = nullptr;
		cardIndex = -1;
	}
}

void PlayScreen::createBlockade()
{
	Territory* clickedTerritory = map->getTerritoryAtPoint(sf::Mouse::getPosition(*window));

	if (clickedTerritory)
	{
		//a territory was clicked
		if (territory1 == clickedTerritory && territory1->getOwner() == player)
		{
			//can blockade
			button = &inputButton;
			inputButton.setString("Scorch");
			inputButton.setPosition(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);
			acceptButton.setPosition(sf::Mouse::getPosition(*window).x + 0.1f * window->getSize().x, sf::Mouse::getPosition(*window).y);
		}
		else
		{
			//can't blockade
			territory1 = nullptr;
			territory2 = nullptr;
			cardIndex = -1;
		}
	}
	else
	{
		//no territory was clicked
		territory1 = nullptr;
		territory2 = nullptr;
		cardIndex = -1;
	}
}

void PlayScreen::createGift()
{
	Territory* clickedTerritory = map->getTerritoryAtPoint(sf::Mouse::getPosition(*window));

	if (clickedTerritory)
	{
		//a territory was clicked
		if (territory2 == clickedTerritory)
		{
			//destination was clicked
			if (territory2->getOwner() && territory2->getOwner() != player)
			{
				//can gift
				button = &inputButton;
				inputButton.setString("Donate");
				inputButton.setPosition(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);
				acceptButton.setPosition(sf::Mouse::getPosition(*window).x + 0.1f * window->getSize().x, sf::Mouse::getPosition(*window).y);
			}
			else
			{
				//can't gift
				territory1 = nullptr;
				territory2 = nullptr;
				cardIndex = -1;
			}
		}
		else if (territory1 == clickedTerritory)
		{
			//source was clicked
			if (territory1->getOwner() != player)
			{
				//source is invalid
				territory1 = nullptr;
				territory2 = nullptr;
				cardIndex = -1;
			}
		}
		else
		{
			//dragged
			if (territory2)
			{
				//changed mind about destination
				territory2 = nullptr;
			}
			else
			{
				//changed mind about source
				territory1 = nullptr;
			}
		}
	}
	else
	{
		//no territory was clicked
		territory1 = nullptr;
		territory2 = nullptr;
		cardIndex = -1;
	}
}

void PlayScreen::pan()
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
	players.clear();

	AIHandler::cleanUp();
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
	airliftButton.draw(window);
	blockadeButton.draw(window);
	giftButton.draw(window);
	window->draw(commandsFrame);

	for (TextButton* textButton : playersButtons)
	{
		textButton->draw(window);
	}

	for (Player* player : players)
	{
		if (!player->hasSubmittedCommands() && player->isAlive() && !player->IS_AI)
		{
			player->drawCommands();
		}
	}

	inputButton.draw(window);
	acceptButton.draw(window);
	window->display();
}

void PlayScreen::mouseDown()
{
	button = nullptr;

	if (inputButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &inputButton;
	}
	else if (acceptButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &acceptButton;
	}
	else if (deployPhaseButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &deployPhaseButton;
	}
	else if (commandPhaseButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &commandPhaseButton;
	}
	else if (cardsButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &cardsButton;
	}
	else if (airliftButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &airliftButton;
	}
	else if (blockadeButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &blockadeButton;
	}
	else if (giftButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &giftButton;
	}
	else if (endTurnButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &endTurnButton;
	}
	else if (quitButton.doesContainPoint(sf::Mouse::getPosition(*window)))
	{
		button = &quitButton;
	}
	else if (territory1)
	{
		territory2 = map->getTerritoryAtPoint(sf::Mouse::getPosition(*window));
	}
	else
	{
		territory1 = map->getTerritoryAtPoint(sf::Mouse::getPosition(*window));
	}
}

void PlayScreen::mouseUp()
{
	bool isCreatingCommand = false;

	if (button == &inputButton)
	{
		if (inputButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			isCreatingCommand = true;
		}
	}
	else if (button == &acceptButton)
	{
		if (acceptButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			if (cardIndex == AirliftCommand::INDEX)
			{
				player->createAirliftCommand(territory1, territory2, std::stoi(inputButton.getString()));
			}
			else if (cardIndex == BlockadeCommand::INDEX)
			{
				player->createBlockadeCommand(territory1);
			}
			else if (cardIndex == GiftCommand::INDEX)
			{
				player->createGiftCommand(territory1, territory2->getOwner());
			}
			else if (territory2)
			{
				player->createMovementCommand(territory1, territory2, std::stoi(inputButton.getString()));
			}
			else
			{
				player->createDeploymentCommand(territory1, std::stoi(inputButton.getString()));
			}

			territory1 = nullptr;
			territory2 = nullptr;
		}
	}
	else if (button == &deployPhaseButton)
	{
		if (deployPhaseButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			player->setPhase(true);
		}
	}
	else if (button == &commandPhaseButton)
	{
		if (commandPhaseButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			player->setPhase(false);
		}
	}
	else if (button == &cardsButton)
	{
		if (cardsButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			if (areCardsVisible)
			{
				areCardsVisible = false;
				airliftButton.setPosition(0.0f, window->getSize().y + 100.0f);
				blockadeButton.setPosition(0.0f, window->getSize().y + 100.0f);
				giftButton.setPosition(0.0f, window->getSize().y + 100.0f);
			}
			else
			{
				areCardsVisible = true;
				airliftButton.setPosition(0.1f * window->getSize().x, 0.2f * window->getSize().y);
				blockadeButton.setPosition(0.2f * window->getSize().x, 0.2f * window->getSize().y);
				giftButton.setPosition(0.3f * window->getSize().x, 0.2f * window->getSize().y);
			}
		}
	}
	else if (button == &airliftButton)
	{
		if (airliftButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			territory1 = nullptr;
			territory2 = nullptr;
			isCreatingCommand = true;
			cardIndex = AirliftCommand::INDEX;
		}
	}
	else if (button == &blockadeButton)
	{
		if (blockadeButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			territory1 = nullptr;
			territory2 = nullptr;
			cardIndex = BlockadeCommand::INDEX;
		}
	}
	else if (button == &giftButton)
	{
		if (giftButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			territory1 = nullptr;
			territory2 = nullptr;
			cardIndex = GiftCommand::INDEX;
		}
	}
	else if (button == &endTurnButton)
	{
		if (endTurnButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			for (Player* player : players)
			{
				if (!player->hasSubmittedCommands() && !player->IS_AI)
				{
					TurnHandler::submitCommands(player);
				}
			}

			TurnHandler::resolveTurn();
			updateCards();
		}
	}
	else if (button == &quitButton)
	{
		if (quitButton.doesContainPoint(sf::Mouse::getPosition(*window)))
		{
			window->close();
		}
	}
	else
	{
		isCreatingCommand = true;
		
		if (cardIndex == AirliftCommand::INDEX)
		{
			createAirlift();
		}
		else if (cardIndex == BlockadeCommand::INDEX)
		{
			createBlockade();
		}
		else if (cardIndex == GiftCommand::INDEX)
		{
			createGift();
		}
		else
		{
			createCommand();
		}
	}

	if (!isCreatingCommand)
	{
		inputButton.setPosition(0.0f, window->getSize().y + 100.0f);
		acceptButton.setPosition(0.0f, window->getSize().y + 100.0f);
		territory1 = nullptr;
		territory2 = nullptr;
	}
}

void PlayScreen::handleText(char character)
{
	if (button)
	{
		button->addCharacter(character);
	}
}

void PlayScreen::keyDown(sf::Keyboard::Key key)
{
	keysPressed.emplace(key);
}

void PlayScreen::keyUp(sf::Keyboard::Key key)
{
	keysPressed.erase(key);
}