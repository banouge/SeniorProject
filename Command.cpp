#include "Command.h"
#include "Player.h"

Command::Command(Player* commander, int bracket, int numArmies, Territory* territory) : COMMANDER(commander), BRACKET(bracket), NUM_ARMIES(numArmies), TERRITORY(territory)
{
}