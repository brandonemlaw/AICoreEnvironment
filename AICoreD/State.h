#pragma once

#include "Board.h"
#include "Move.h"
#include <vector>

class State
{
public:
	Board board;
	bool isWhitesTurn;  //White will move from this state
	int visits;
	double wins;
	Move sourceMove;

	//std::vector<Move> allMoves;
	
	//void generateAllMoves();
	State::State();

	int aiPieceCount(bool AIIsWhite);
	int enemyPieceCount(bool AIIsWhite);

};