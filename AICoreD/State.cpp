#include "State.h"

/*void State::generateAllMoves()
{
	allMoves.reserve(35);


	//Define the parameters, assuming it is black's turn
	bool rowUp = false;
	unsigned int row = 7;
	unsigned int* currentPieces = board.blackRows;
	unsigned int* otherPieces = board.whiteRows;
	int pieceCount = board.blackCount;

	//Redefine the parameters if it's whites turn
	if (isWhitesTurn)
	{
		rowUp = true;
		row = 0;
		currentPieces = board.whiteRows;
		otherPieces = board.blackRows;
		pieceCount = board.whiteCount;
	}

	while (pieceCount > 0)
	{
		//If the search is over, abort
		if ((rowUp && row >= 7) || (!rowUp && row <= 0))
		{
			pieceCount = 0;
		}
		//If there are no pieces on the row, proceed
		else if (currentPieces[row] == 0)
		{
			//increment or decrement the row count
			if (rowUp)
				row++;
			else
				row--;
		}
		//If pieces are found, extract them into the moves list
		else
		{
			for (unsigned int col = 0; col < 8; col++)
			{
				//For each piece in the row
				if ((currentPieces[row] & board.COLUMNS[col]) > 0)
				{
					//get new row number for checking
					unsigned int newRow = row - 1;
					if (rowUp)
					{
						newRow = row + 1;
					}

					for (unsigned int target = 0; target < 3; target++)
					{
						//if the move is out of bounds
						if ((col + target - 1) < 0 || (col + target - 1) > 7)
						{ 
						}

						//if the move is directly ahead but not into an opponent or my piece,
						else if (target == 1 && (otherPieces[newRow] & board.COLUMNS[col]) == 0 && (currentPieces[newRow] & board.COLUMNS[col]) == 0)
						{
							//add it to the moves list
							allMoves.emplace_back(row, col, target);
						}
						//if the move to the side but not into my own piece,
						else if (target != 1 && (currentPieces[newRow] & board.COLUMNS[col + target - 1]) == 0)
						{
							//add it to the moves list
							allMoves.emplace_back(row, col, target);
						}
					}

					//reduce the number of pieces being searched for
					pieceCount--;
				}
			}

			//increment or decrement the row count
			if (rowUp)
				row++;
			else
				row--;
		}
	}
}
*/
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