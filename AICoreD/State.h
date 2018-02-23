#pragma once

#include "Board.h"
#include "Move.h"
#include <vector>

class State
{
public:
	Board board;
	short visits;
	short wins;
	Move sourceMove;
	bool isWhitesTurn;  //White will move from this state


	//std::vector<Move> allMoves;
	
	//void generateAllMoves();
	State::State();

	int aiPieceCount(bool AIIsWhite);
	int enemyPieceCount(bool AIIsWhite);

};