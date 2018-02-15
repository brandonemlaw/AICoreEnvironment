//Breakthrough AI
//SoftDev Team 1
//AI Lead: Brandon Emlaw

#include "Board.h"
#include <random>


//Reference made to: https://github.com/antirez/connect4-montecarlo/blob/master/c4.c


struct Move
{
	int value;
	short unsigned int row;
	short unsigned int col;
	short unsigned int target;
};

const unsigned int COLUMNS[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };	//A-H


Move AIGetMove(int blackCount, int whiteCount, unsigned int blackRows[], unsigned int whiteRows[], bool isWhitesTurn);
bool executeRandomGame(Board& board, bool isWhitesTurn);
void populateMoveList(Move moves[], unsigned int& moveCount, unsigned int pieceCount, unsigned int* currentPieces, unsigned int* otherPieces, unsigned short int row);


void populateMoveList(Move moves[], unsigned int& moveCount, unsigned int pieceCount, unsigned int* currentPieces, unsigned int* otherPieces, unsigned short int row)
{
	bool rowUp = false;
	if (row == 0)
	{
		rowUp = true;
	}


	while (pieceCount > 0)
	{
		//If the search is over, abort
		if (row > 7 || row < 0)
		{
			pieceCount = 0;
		}
		//If there are no pieces on the row, proceed
		else if (currentPieces[row] == 1)
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
			for (unsigned short int col = 0; col < 8; col++)
			{
				//For each piece in the row
				if (currentPieces[row] % COLUMNS[col] == 0)
				{
					//get new row number for checking
					unsigned short int newRow = row - 1;
					if (rowUp)
					{
						newRow = row + 1;
					}

					for (unsigned short int target = 0; target < 3; target++)
					{
						//if the move is out of bounds
						if ((col + target - 1) < 0 || (col + target - 1) > 7)
						{ /*do nothing; move is out of bounds*/	}

						//if the move is directly ahead but not into an opponent or my piece,
						else if (target == 1 && otherPieces[newRow] % COLUMNS[col] != 0 && currentPieces[newRow] % COLUMNS[col] != 0)
						{
							//add it to the moves list
							moves[moveCount].row = row;
							moves[moveCount].col = col;
							moves[moveCount].target = target;
							moves[moveCount].value = 0;
							moveCount++;
						}
						//if the move to the side but not into my own piece,
						//TODO - check for divide by less than zero
						else if (target != 1 && currentPieces[newRow] % COLUMNS[col + target - 1] != 0)
						{
							//add it to the moves list
							moves[moveCount].row = row;
							moves[moveCount].col = col;
							moves[moveCount].target = target;
							moves[moveCount].value = 0;
							moveCount++;
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

Move AIGetMove(int blackCount, int whiteCount, unsigned int blackRows[], unsigned int whiteRows[], bool isWhitesTurn)
{
	const int RANDOM_GAMES_PER_MOVE = 1000;

	//Create list for possible moves 
	//TODO - reevaluate maximum move quantity
	Move moves[4096];
	unsigned int moveCount = 0;
	unsigned int bestMove = -1;
	int bestMoveScore = -RANDOM_GAMES_PER_MOVE - 9;

		//Populate moves by whose turn it is
	if (isWhitesTurn)
	{
		populateMoveList(moves, moveCount, whiteCount, whiteRows, blackRows, 0);
	}
	else
	{
		populateMoveList(moves, moveCount, blackCount, blackRows, whiteRows, 7);
	}

	for (unsigned int i = 0; i < moveCount; i++)
	{
		for (int j = 0; j < RANDOM_GAMES_PER_MOVE; j++)
		{
			//Build the board and run the simulation
			Board board = Board(blackCount, whiteCount, blackRows, whiteRows);
			bool win = executeRandomGame(board, isWhitesTurn);

			//If we won, increment the value; if we lost, decrement the value
			if (win)
			{
				moves[i].value++;
			}
			else
			{
				moves[i].value--;
			}

			//Compare move to previous best move score
			if (moves[i].value > bestMoveScore)
			{
				bestMoveScore = moves[i].value;
				bestMove = i;
			}
		}
	}

	return moves[bestMove];
}

bool executeRandomGame(Board& board, bool isWhitesTurn)
{
	while (!board.gameOver)
	{
		//set the reference for the current pieces set
		unsigned int* currentPieces = NULL;
		short int moveChange;
		if (isWhitesTurn)
		{
			currentPieces = board.whiteRows;
			moveChange = 1;
		}
		else
		{
			currentPieces = board.blackRows;
			moveChange = -1;
		}

		//choose a row randomly and then loop until a valid row (with a piece to move) is found
		short unsigned int row = rand() % 8;
		while (currentPieces[row] == 1)
		{
			if (row >= 7)
			{
				row = 0;
			}
			else
			{
				row++;
			}
		}

		//choose a col randomly and then loop until a valid row (with a piece to move) is found
		short unsigned int col = rand() % 8;
		while (currentPieces[row] % board.COLUMNS[col] != 0)
		{
			if (col >= 7)
			{
				col = 0;
			}
			else
			{
				col++;
			}
		}

		//choose a target position (-1 to 1) and loop until valid
		short int target = rand() % 3 - 1;
		bool validMoveExists = false;

		//Run the move and see if it is valid
		bool successfulMove = board.makeMove(isWhitesTurn, row, col, row + moveChange, col + target, false);

		//Loops until valid move is found or target == 2. If target == 2, means validMove became false and we've gone around again at least once.
		//At this point, we will just choose a new random move all over again.
		while (!successfulMove && target != 2)
		{
			if (target == 1 && validMoveExists)
			{
				target = -1;
				validMoveExists = false;
			}
			else
			{
				target++;
			}
			board.makeMove(isWhitesTurn, row, col, row + moveChange, col + target, false);
		}

		//Switch whose turn it is if the move was successful in the end
		if (successfulMove)
		{
			isWhitesTurn = !isWhitesTurn;
		}
	}

	//Return whoever just played and therefore won the game
	return !isWhitesTurn;
}


/*
void main()
{
	Board myBoard = Board();
	myBoard.makeMove(true, 1, 6, 2, 4, false);
	AIGetMove(myBoard.blackCount, myBoard.whiteCount, myBoard.blackRows, myBoard.whiteRows, false);
}*/
