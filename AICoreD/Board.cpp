
#include "Board.h"


Board::Board()
{
	white = 0;
	black = 0;

	/*for (int i = 0; i < 8; i++)
	{
		whiteRows[i] = 1;
		blackRows[i] = 1;
	}*/
	for (int i = 0; i < 8; i++)
	{
		white += COLUMNS[i];
		white += 8 + COLUMNS[i];
		black += 8*6 + COLUMNS[i];
		black += 8*7 + COLUMNS[i];
	}
}


Board::Board(int newBlackCount, int newWhiteCount, unsigned long newBlack, unsigned long newWhite)
{
	//load the new info
	blackCount = newBlackCount;
	whiteCount = newWhiteCount;
	white = newWhite;
	black = newBlack;
	
	/*//load the pieces info
	for (int i = 0; i < 8; i++)
	{
		whiteRows[i] = newWhiteRows[i];
		blackRows[i] = newBlackRows[i];
	}*/

}

void Board::fromBoard(Board & oldBoard)
{
	//load the new info
	blackCount = oldBoard.blackCount;
	whiteCount = oldBoard.whiteCount;
	white = oldBoard.white;
	black = oldBoard.black;

	//load the pieces info
	/*for (int i = 0; i < 8; i++)
	{
		whiteRows[i] = oldBoard.whiteRows[i];
		blackRows[i] = oldBoard.blackRows[i];
	}*/

}

void Board::setParameters(int newBlackCount, int newWhiteCount, unsigned long newBlack, unsigned long newWhite)
{
	//load the new info
	blackCount = newBlackCount;
	whiteCount = newWhiteCount;
	white = newWhite;
	black = newBlack;

	/*//load the pieces info
	for (int i = 0; i < 8; i++)
	{
		whiteRows[i] = newWhiteRows[i];
		blackRows[i] = newBlackRows[i];
	}*/

}


bool Board::makeMove(bool playerWhite, int oldRow, int oldColumn, int row, int column)
{
	//to do:the array values should be put into temp variable so they dont have to be accessed again

	//bound checking
	/*if (oldRow < 0 || oldRow > 7 || oldColumn < 0 || oldColumn > 7)
	{
		return false;
	}*/

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

		if ((white >> row * 8) & COLUMNS[column])
		{
			return false;
		}

		if (column == oldColumn && ((black >> row) & COLUMNS[column]))
		{
			return false;
		}

		if ((black >> row * 8) & COLUMNS[column])
		{
			black -= 8 * row + COLUMNS[column];
			//blackRows[row] -= COLUMNS[column];
			blackCount--;
		}

		if (row == 7 || blackCount == 0)
		{
				gameOver = true;
		}

		white -= 8 * row + COLUMNS[oldColumn];
		white += 8 * row + COLUMNS[column];
		//whiteRows[oldRow] -= COLUMNS[oldColumn];
		//whiteRows[row] += COLUMNS[column];
	}

	if (!playerWhite)
	{
		/*//allows only move down one row at time
		if (row != oldRow - 1)
		{
			return false;
		}*/
		//is there a black piece already at new position
		if ((black >> row * 8) & COLUMNS[column])
		{
			return false;
		}
		/*//is there a black piece to be moved from old position
		if (blackRows[oldRow] | COLUMNS[oldColumn] != 0)
		{
			return false;
		}*/
		//prevent piece from moving forward if white piece is in way
		if (column == oldColumn && ((white >> row * 8) & COLUMNS[column]))
		{
			return false;
		}
		//if piece of white was taken
		if ((white >> row * 8) & COLUMNS[column])
		{
			white -= 8 * row + COLUMNS[column];
			//whiteRows[row] -= COLUMNS[column];
			whiteCount--;
		}

		if (row == 0 || whiteCount == 0)
		{
			gameOver = true;
		}

		black -= 8 * row + COLUMNS[oldColumn];
		black += 8 * row + COLUMNS[column];
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

char Board::getPieceAt(int row, int column)
{
	if ((white >> row * 8) & COLUMNS[column])
	//if ((whiteRows[row] & COLUMNS[column]) != 0)
	{
		return 'W';
	}
	else if ((black >> row * 8) & COLUMNS[column])
	//else if ((blackRows[row] & COLUMNS[column]) != 0)
	{
		return 'B';
	}
	else
	{
		return 'S';
	}

}



bool operator== (const Board &b1, const Board &b2)
{
	return (
		b1.black == b2.black && b1.white == b2.white &&
		b1.blackCount == b2.blackCount &&
		b1.whiteCount == b2.whiteCount &&
		b1.gameOver == b2.gameOver
		);
}
