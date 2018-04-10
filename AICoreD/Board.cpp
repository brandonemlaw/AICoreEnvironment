
#include "Board.h"



/*
Board

Creates a default board.
*/
Board::Board()
{
	//Initialize parameters
	white = 0;
	black = 0;
	blackWinConditions = ROWS[0];
	whiteWinConditions = ROWS[7];
}

/*
Board

Creates a board based on a set of initial game parameters.
*/
Board::Board(int newBlackCount, int newWhiteCount, unsigned long long newBlack, unsigned long long newWhite)
{
	//load the new info
	blackCount = newBlackCount;
	whiteCount = newWhiteCount;
	black = newBlack;
	white = newWhite;

	//load the default info
	blackWinConditions = ROWS[0];
	whiteWinConditions = ROWS[7];

}

/*
fromBoard

Creates a board with the parameters of another board. This is effectively a copy operation.
*/
void Board::fromBoard(Board & oldBoard)
{
	//load the new info
	blackCount = oldBoard.blackCount;
	whiteCount = oldBoard.whiteCount;
	black = oldBoard.black;
	white = oldBoard.white;

	blackWinConditions = oldBoard.blackWinConditions;
	whiteWinConditions = oldBoard.whiteWinConditions;
}

/*
setParameters

Sets the parameters of the board to match those of another.
*/
void Board::setParameters(int newBlackCount, int newWhiteCount, unsigned long long newBlack, unsigned long long newWhite)
{
	//load the new info
	blackCount = newBlackCount;
	whiteCount = newWhiteCount;
	black = newBlack;
	white = newWhite;

	//load the default info
	blackWinConditions = ROWS[0];
	whiteWinConditions = ROWS[7];
}

/*
makeMove
-is the move made by white?
-the row (vertical on a regular board) the peice being moved starts at
-the column (horizontal on a regular boarD) the peice being moved starts at
-the row the peice is being moved to
-the column the peice is being moved to

Creates a board with the parameters of another board. This is effectively a copy operation.
*/
bool Board::makeMove(bool playerWhite, int oldRow, int oldColumn, int row, int column)
{
	//load squares filter
	unsigned long long targetSquareCheckFilter = SQUARES[row][column];

	//bound checking
	/*if (oldRow < 0 || oldRow > 7 || oldColumn < 0 || oldColumn > 7)
	{
		return false;
	}
	*/
	if (row < 0 || row > 7 || column < 0 || column > 7)
	{
		return false;
	}
	/*//only allows diagonal move or forward move
	if (column < oldColumn - 1 || column > oldColumn + 1)
	{
		return false;
	}*/

	if (playerWhite)
	{
		/*if (row != oldRow + 1)
		{
			return false;
		}*/

		/*if (whiteRows[oldRow] | COLUMNS[oldColumn] != 0)
		{
			return false;
		}*/

		if (white & targetSquareCheckFilter)
		{
			return false;
		}

		if (column == oldColumn && (black & targetSquareCheckFilter))
		{
			return false;
		}

		if (black & targetSquareCheckFilter)
		{
			black -= targetSquareCheckFilter;
			blackCount--;
		}

		white -= SQUARES[oldRow][oldColumn];
		white += targetSquareCheckFilter;

		/*//if white is in a victory location
		unsigned long long check = 0;
		int r = 0;
		//loop through each possible spot in each direction
		while (row + r < 7)
		{
			int c = 0;
			while ((column + c < 7) || (column - c > 0))
			{
				if (column + c < 7)
				{
					check += SQUARES[row + r][column + c];
				}
				check += SQUARES[row + r][column - c]
			}
		}


		if (!(black & 
			(SQUARES[row + 1][column - 1] 
				& SQUARES[row + 1][column + 1] 
				& SQUARES[row + 2][column - 2]

				)))
		{
			victoryLocation = 1;
		}*/

		/***********************/
		//Update the victory spots for black
		//if we're moving out of a black victory position
		if (SQUARES[oldRow][oldColumn] & blackWinConditions)
		{
			//check for new win conditions
			int conditions = 0;

			//1. check 2 to the right for pieces and 1s, if no piece and a 1 -> 
			if (oldColumn + 2 < 7)
			{
				if (SQUARES[oldRow][oldColumn + 2] & blackWinConditions & (~white))
				{
					//put a 1 in 1down 1 to the right
					if (oldRow + 1 > 0)
					{
						blackWinConditions += SQUARES[oldRow + 1][oldColumn + 1];
					}
				}
			}
							
			//2. check 2 to the left for pieces and 1s, if no piece and a 1 ->
			if (oldColumn - 2 > 0)
			{
				if (SQUARES[oldRow][oldColumn - 2] & blackWinConditions & (~white))
				{
					//put a 1down 1 to the left
					if (oldRow + 1 > 0)
					{
						blackWinConditions += SQUARES[oldRow + 1][oldColumn - 1];
					}
				}
			}


		//3. if left and right are 1s, and there are no pieces to the left and right
			if ((SQUARES[oldRow][oldColumn] & blackWinConditions & (~white)) &&
				(oldColumn + 1 > 7 || (SQUARES[oldRow][oldColumn + 1] & blackWinConditions & (~white)))
				&& (oldColumn - 1 < 0 || (SQUARES[oldRow][oldColumn - 1] & blackWinConditions & (~white)))
				)
			{
				//then we'd put a 1 below
				blackWinConditions += SQUARES[oldRow + 1][oldColumn];
			}
		}
		/***********************/

		//If white is in a victory position
		if (black & blackWinConditions || whiteCount <= 0)
		{
			victoryLocation = 1;
		}

		//if white completely wins
		if (row == 7 || blackCount <= 0)
		{
			gameOver = true;
		}
		

	}

	if (!playerWhite)
	{
		//load squares filter


		/*//allows only move down one row at time
		if (row != oldRow - 1)
		{
			return false;
		}*/
		//is there a black piece already at new position
		if ((black & targetSquareCheckFilter))
		{
			return false;
		}
		/*//is there a black piece to be moved from old position
		if (blackRows[oldRow] | COLUMNS[oldColumn] != 0)
		{
			return false;
		}*/
		//prevent piece from moving forward if white piece is in way
		if (column == oldColumn && (white & targetSquareCheckFilter))
		{
			return false;
		}
		//if piece of white was taken
		if (white & targetSquareCheckFilter)
		{
			white -= SQUARES[row][column];
			whiteCount--;
		}

		black -= SQUARES[oldRow][oldColumn]; //remove piece at old position
		black += targetSquareCheckFilter; //add piece at new position

		/***********************/
		//Update the victory spots for white
		//if we're moving out of a white victory position
		if (SQUARES[oldRow][oldColumn] & whiteWinConditions)
		{
			//1. check 2 to the right for pieces and 1s, if no piece and a 1 -> 
			if (oldColumn + 2 < 7)
			{
				if (SQUARES[oldRow][oldColumn + 2] & whiteWinConditions & (~black))
				{
					//put a 1 in 1down 1 to the right
					if (oldRow + 1 > 0)
					{
						whiteWinConditions += SQUARES[oldRow - 1][oldColumn + 1];
					}
				}
			}

			//2. check 2 to the left for pieces and 1s, if no piece and a 1 ->
			if (oldColumn - 2 > 0)
			{
				if (SQUARES[oldRow][oldColumn - 2] & whiteWinConditions & (~black))
				{
					//put a 1down 1 to the left
					if (oldRow + 1 > 0)
					{
						whiteWinConditions += SQUARES[oldRow - 1][oldColumn - 1];
					}
				}
			}


			//3. if left and right are 1s, and there are no pieces to the left and right
			if ((SQUARES[oldRow][oldColumn] & whiteWinConditions & (~black)) &&
				(oldColumn + 1 > 7 || (SQUARES[oldRow][oldColumn + 1] & whiteWinConditions & (~black)))
				&& (oldColumn - 1 < 0 || (SQUARES[oldRow][oldColumn - 1] & whiteWinConditions & (~black)))
				)
			{
				//then we'd put a 1 below
				whiteWinConditions += SQUARES[oldRow - 1][oldColumn];
			}
		}
		/***********************/

		//If black is in a victory position
		if (black & blackWinConditions || whiteCount <= 0)
		{
			victoryLocation = 2;
		}

		//if black completely wins
		if (row == 0 || whiteCount <= 0)
		{
			gameOver = true;
		}
	}


	return true;
}


bool Board::isGameOver()
{
	if (gameOver)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*char Board::getPieceAt(int row, int column)
{
	if (white & SQUARES[row][column])
	{
		return 'W';
	}
	else if (black & SQUARES[row][column])
	{
		return 'B';
	}
	else
	{
		return 'S';
	}

}*/



bool operator== (const Board &b1, const Board &b2)
{
	return (
		b1.blackCount == b2.blackCount &&
		b1.whiteCount == b2.whiteCount &&
		b1.black == b2.black && 
		b1.white == b2.white && 
		b1.gameOver == b2.gameOver
		);
}
