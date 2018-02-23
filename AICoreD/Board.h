#pragma once

#include "Move.h"
#include "Constants.h"

class Board
{
public:
	short blackCount = 16;
	short whiteCount = 16;

	unsigned long long black;
	unsigned long long white;


	//unsigned int blackRows[8];
	//unsigned int whiteRows[8];
	//const unsigned int COLUMNS[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };	//A-H
	//const unsigned int COLUMNS[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };	//A-H



	Board();
	Board(int newBlackCount, int newWhiteCount, unsigned long long newBlack, unsigned long long newWhite);
	void fromBoard(Board& oldBoard);
	void setParameters(int newBlackCount, int newWhiteCount, unsigned long long newBlack, unsigned long long newWhite);
	bool makeMove(bool playerWhite, int oldRow, int oldColumn, int row, int column);

	bool isGameOver();
	//char getPieceAt(int row, int column);

	bool gameOver = false;
};

bool operator== (const Board &b1, const Board &b2);