
#include "Board.h"




Board::Board()
{
	//Initialize parameters
	white = 0;
	black = 0;
}


Board::Board(int newBlackCount, int newWhiteCount, unsigned long long newBlack, unsigned long long newWhite)
{
	//load the new info
	blackCount = newBlackCount;
	whiteCount = newWhiteCount;
	black = newBlack;
	white = newWhite;

}

void Board::fromBoard(Board & oldBoard)
{
	//load the new info
	blackCount = oldBoard.blackCount;
	whiteCount = oldBoard.whiteCount;
	black = oldBoard.black;
	white = oldBoard.white;

}

void Board::setParameters(int newBlackCount, int newWhiteCount, unsigned long long newBlack, unsigned long long newWhite)
{
	//load the new info
	blackCount = newBlackCount;
	whiteCount = newWhiteCount;
	black = newBlack;
	white = newWhite;


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

		if (row == 7 || blackCount == 0)
		{
				gameOver = true;
		}

		white -= SQUARES[oldRow][oldColumn];
		white += targetSquareCheckFilter;
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

		if (row == 0 || whiteCount == 0)
		{
			gameOver = true;
		}

		black -= SQUARES[oldRow][oldColumn]; //remove piece at old position
		black += targetSquareCheckFilter; //add piece at new position
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
