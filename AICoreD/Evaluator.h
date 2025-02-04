#pragma once
#include "Board.h"
#include "Node.h"


//Weights
const double MASTER_WEIGHT = 1;
const double TAKE_PIECE_WEIGHT = 1; //1.05
const double LOSE_PIECE_WEIGHT = 1; //0.8

const double DOUBLE_BACKUP_WEIGHT = 1; //1.07
const double BACKUP_WEIGHT = 1; //1.05

const double SIDE_WEIGHT = 1;

const double DOUBLE_DEFENDED_WEIGHT = 1; //0.9
const double DEFENDED_WEIGHT = 1; //0.8
const double BLOCKED_WEIGHT = 1;



class Evaluator
{
public:	
	static double evaluate(Node* move, Board boardBeforeMove, bool playingForWhite);

private:
	static bool backupLeft(Board board, int row, int col, bool playingForWhite);
	static bool backupRight(Board board, int row, int col, bool playingForWhite);
	static bool defendedLeft(Board board, int row, int col, bool playingForWhite);
	static bool defendedRight(Board board, int row, int col, bool playingForWhite);
	static bool blocked(Board board, int row, int col, bool playingForWhite);
};