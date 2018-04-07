#include "State.h"


State::State()
{
}


int State::aiPieceCount(bool AIIsWhite)
{
	if (AIIsWhite)
		return board.whiteCount;
	else
		return board.blackCount;
}

int State::enemyPieceCount(bool AIIsWhite)
{
	if (AIIsWhite)
		return board.blackCount;
	else
		return board.whiteCount;

}