#include "Evaluator.h"

//Evaluate how good a move is
double Evaluator::evaluate(Node* move, Board boardBeforeMove, bool playingForWhite)
{
	//Start with the win score from the tree search process
	double score = (move->state.wins / move->state.visits) * MASTER_WEIGHT;

	//***SETUP PARAMETER***///




	//Check for if the move leads to an immediate victory 
	if (move->state.board.isGameOver())
	{
		//If we win with this move, Return the maximum possible score
		return DBL_MAX;
	}

	//Check for if the move allows the enemy any moves that make us lose
	//For each of the enemy's moves
	unsigned size = move->childCount;
	for (unsigned i = 0; i < size; i++)
	{
		//If their move leads to the game ending (meaning they won)
		if (move->child[i]->state.board.isGameOver())
		{
			//We would lose. Since the move we're looking at allows them a move that will let them win, it is an awful move.
			//Return the minimum score possible.
			return DBL_MIN;
		}
	}


	//If we can take a piece
	bool areTakingAPiece = false;
	if (move->parent->state.enemyPieceCount(playingForWhite) > move->state.enemyPieceCount(playingForWhite))
	{
		areTakingAPiece = true;
	}

	if (areTakingAPiece)
	{
		score *= TAKE_PIECE_WEIGHT;
	}


	//Calculate how many ways they have to take us with this move
	int waysToTakeUs = 0;

	//For each move they can make
	for (int i = 0; i < move->childCount; i++)
	{
		//If they can take us (our piece count after their move is < our piece count after our move)
		if (move->child[i]->state.aiPieceCount(playingForWhite) < move->state.aiPieceCount(playingForWhite))
		{
			//Add one way to take us
			waysToTakeUs += 1;
		}
	}


	//If they can take us at least one way...
	if (waysToTakeUs > 0)
	{
		score *= LOSE_PIECE_WEIGHT;
	}


	//Check for move formations
	int rowChangeForUs = 1;
	int rowChangeForThem = -1;
	//if we're not white, we're moving down a row instead
	if (!playingForWhite)
	{
		rowChangeForUs = -1;
		rowChangeForThem = 1;
	}


	int startRow = move->state.sourceMove.row;
	int startCol = move->state.sourceMove.col;
	int endRow = startRow + rowChangeForUs;
	int endCol = startCol + move->state.sourceMove.target - 1;

	//****Position analysis****//
	bool onLeft = (endCol == 0);
	bool onRight = (endCol == 7);
	bool bLeft = backupLeft(move->state.board, endRow, endCol, playingForWhite);
	bool bRight = backupRight(move->state.board, endRow, endCol, playingForWhite);
	bool dRight = defendedRight(move->state.board, endRow, endCol, playingForWhite);
	bool dLeft = defendedLeft(move->state.board, endRow, endCol, playingForWhite);
	bool block = blocked(move->state.board, endRow, endCol, playingForWhite);


	//Only increase for backup when determining whether to make an offsensive move
	if (areTakingAPiece || waysToTakeUs > 0)
	{
		//If double backed-up...
		if (bLeft && bRight)
		{
			score *= DOUBLE_BACKUP_WEIGHT;
		}
		//If single backed-up
		else if (bLeft || bRight)
		{
			score *= BACKUP_WEIGHT;
		}

		//If on the left or right side of the board
		if (onLeft || onRight)
		{
			score *= SIDE_WEIGHT;
		}
	}


	//If double defended....
	if (dRight && dLeft)
	{
		score *= DOUBLE_DEFENDED_WEIGHT;
	}
	//If single defended....
	else if (dRight || dLeft)
	{
		score *= DEFENDED_WEIGHT;
	}

	if (block)
	{
		score *= BLOCKED_WEIGHT;
	}



	//move->state.board.getPieceAt(5, 7);



	//Return the current score
	return score;
}


bool Evaluator::backupLeft(Board board, int row, int col, bool playingForWhite)
{
	//Prevent out of bounds errors - only check if the column to the left is on the board
	if (col > 0)
	{
		if (playingForWhite)
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row - 1;

			//Return whether or not their is a piece there
			return (board.white & SQUARES[rowEnemyDefendingFrom][col - 1]);
		}
		else
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row + 1;

			//Return whether or not their is a piece there
			return board.black & SQUARES[rowEnemyDefendingFrom][col - 1];
		}
	}
	else
	{
		return false;
	}
}


bool Evaluator::backupRight(Board board, int row, int col, bool playingForWhite)
{
	//Prevent out of bounds errors - only check if the column to the left is on the board
	if (col > 0)
	{
		if (playingForWhite)
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row - 1;

			//Return whether or not their is a piece there
			return (board.white & SQUARES[rowEnemyDefendingFrom][col + 1]);
		}
		else
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row + 1;

			//Return whether or not their is a piece there
			return (board.black & SQUARES[rowEnemyDefendingFrom][col + 1]);
		}
	}
	else
	{
		return false;
	}
}

bool Evaluator::defendedLeft(Board board, int row, int col, bool playingForWhite)
{
	//Prevent out of bounds errors - only check if the column to the left is on the board
	if (col > 0)
	{
		if (playingForWhite)
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row + 1;

			//Return whether or not their is a piece there
			return (board.black & SQUARES[rowEnemyDefendingFrom][col - 1]) != 0;
		}
		else
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row - 1;

			//Return whether or not their is a piece there
			return (board.white & SQUARES[rowEnemyDefendingFrom][col - 1]) != 0;
		}
	}
	else
	{
		return false;
	}
}

bool Evaluator::defendedRight(Board board, int row, int col, bool playingForWhite)
{
	//Prevent out of bounds errors - only check if the column to the right is on the board
	if (col < 7)
	{
		if (playingForWhite)
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row + 1;

			//Return whether or not their is a piece there
			return (board.black & SQUARES[rowEnemyDefendingFrom][col + 1]) != 0;
		}
		else
		{
			//Find the row the enemy is defending from
			int rowEnemyDefendingFrom = row - 1;

			//Return whether or not their is a piece there
			return (board.white & SQUARES[rowEnemyDefendingFrom][col + 1]) != 0;
		}
	}
	else
	{
		return false;
	}
}

//Return true if the piece is going to be blocked
bool Evaluator::blocked(Board board, int row, int col, bool playingForWhite)
{
	if (playingForWhite)
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row + 1;

		//Return whether or not their is a piece there
		return (board.black & SQUARES[rowEnemyDefendingFrom][col]);
	}
	else
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row - 1;

		//Return whether or not their is a piece there
		return (board.white &SQUARES[rowEnemyDefendingFrom][col]);
	}
}


