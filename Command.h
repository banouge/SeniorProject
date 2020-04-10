#pragma once

class Player;

class Command
{
public:
	Command(Player* commander, int bracket);

	const int BRACKET;
	const Player* COMMANDER;

	virtual void resolve() = 0;
};