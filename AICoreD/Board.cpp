
#include "Board.h"




Board::Board()
{
	//Initialize parameters
	white = 0;
	black = 0;
	blackWinConditions = ROWS[0];
	whiteWinConditions = ROWS[7];
}


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


bool Board::makeMove(bool playerWhite, int oldRow, int oldColumn, int row, int column)
{
	//to do:the array values should be put into temp variable so they dont have to be accessed again


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

		//TODO !!!!!!
		//Update the victory spots for black
		//if we're moving out of a black victory position
		if (SQUARES[oldRow][oldColumn] & blackWinConditions)
		{
			//check for new win conditions
			//do this /*recursively*/ for each move where the win condition is added

			//1. check 2 to the right for pieces and 1s, if no piece and a 1 -> 
				//if starting square is cleared
					//put a 1 in 1down 1 to the right
							
			//2. check 2 to the left for pieces and 1s, if no piece and a 1 ->
				//if starting square is cleared
					//put a 1down 1 to the left

			//3. if left and right are 1s, and there are no pieces to the left and right, then we'd put a 1 below

			//if  1, 2, and 3 are 1s and clear of pieces, then add a 1 condition two below the root spot

		}

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
