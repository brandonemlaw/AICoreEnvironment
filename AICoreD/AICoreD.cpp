//Breakthrough AI
//SoftDev Team 1
//AI Lead: Brandon Emlaw
//References: 
//	https://github.com/antirez/connect4-montecarlo/blob/master/c4.c
//	https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
//	http://www.baeldung.com/java-monte-carlo-tree-search
//	https://www.theatlantic.com/technology/archive/2017/07/marion-tinsley-checkers/534111/

//For tree deletion: https://www.geeksforgeeks.org/non-recursive-program-to-delete-an-entire-binary-tree/


#include "AICoreD.h"


extern "C" __declspec(dllexport) Move __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned int blackRows[], unsigned int whiteRows[], bool isWhitesTurn)
{
	//Init the current board from the parameters
	Board board;
	board.setParameters(blackCount, whiteCount, blackRows, whiteRows);

	//Save the original root for pruning
	Node* originalRoot = root;

	//Setup the roots
	setRoot(root, board, isWhitesTurn);

	//Start four monte carlo evaluation threads
	std::thread mc(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc2(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc3(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc4(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc5(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc6(runMonteCarloAlgorithm, root, board, isWhitesTurn);


	//Join all the threads
	mc.join();
	mc2.join();
	mc3.join();
	mc4.join();
	mc5.join();
	mc6.join();


	//Find the first level child with the max value as the result
	double maxValue = 0;
	Node* result = NULL;

	//For each of the first level children (every move we could choose)....
	for (unsigned int i = 0; i < root->children.size(); i++)
	{
		//give it a value from the evaluate function
		double value = evaluate(root->children[i], board, isWhitesTurn);

		//If the value is the best value, then choose it
		if (value > maxValue)
		{
			//To choose the move, save the new value and set the move Node as the result
			maxValue = value;
			result = root->children[i];

			//this will be overriden if a new best move is found later
		}
	}

	//set the result to prune above
	ThreadPruner prune(result, originalRoot);

	//set the result as the new root
	root = result;

	//return the sourceMove from the result
	return result->state.sourceMove;
}


void setRoot(Node*& r, Board board, bool isWhitesTurn)
{
	//if root is not set, create a new root
	if (r == NULL)
	{
		r = Node::rootNode(board, isWhitesTurn);
	}

	//if the root is not null, find the correct child, since the other player has moved
	else
	{
		bool foundNewRoot = false;
		unsigned short int size = root->children.size();
		for (unsigned short int i = 0; (!foundNewRoot && i < size); i++)
		{
			//if the current boardstate is found
			if (r->children[i]->state.board == board)
			{
				//set the new root
				r = r->children[i];

				//set the root's parent to null
				r->parent = NULL;

				//set the found flag
				foundNewRoot = true;
			}
		}

		//if we didn't find one, set the root over again to empty
		if (!foundNewRoot)
		{
			r = Node::rootNode(board, isWhitesTurn);
		}
	}
}


//*******EVALUATION AND SUB FUNCTIONS*******//


//Evaluate how good a move is
double evaluate(Node* move, Board boardBeforeMove, bool playingForWhite)
{
	//Start with the win score from the tree search process
	double score = move->state.wins;

	//***SETUP PARAMETER***///




	//Check for if the move leads to an immediate victory 
	if (move->state.board.isGameOver())
	{
		//If we win with this move, Return the maximum possible score
		return DBL_MAX;
	}

	//Check for if the move allows the enemy any moves that make us lose
	//For each of the enemy's moves
	unsigned short size = move->children.size();
	for (unsigned short i = 0; i < size; i++)
	{
		//If their move leads to the game ending (meaning they won)
		if (move->children[i]->state.board.isGameOver())
		{
			//We would lose. Since the move we're looking at allows them a move that will let them win, it is an awful move.
			//Return the minimum score possible.
			return DBL_MIN;
		}
	}





	//If we can take a piece
	bool areTakingAPiece = false;
	if (move->parent->state.enemyPieceCount(playingForWhite) < move->state.enemyPieceCount(playingForWhite))
	{
		areTakingAPiece = true;
	}
	
	if (areTakingAPiece)
	{
		score += TAKE_PIECE_WEIGHT;
	}


	//Calculate how many ways they have to take us with this move
	int waysToTakeUs = 0;

	//For each move they can make
	for (auto &theirMove : move->children)
	{
		//If they can take us (our piece count after their move is < our piece count after our move)
		if (theirMove->state.aiPieceCount(playingForWhite) < move->state.aiPieceCount(playingForWhite))
		{
			//Add one way to take us
			waysToTakeUs += 1;
		}
	}


	//If they can take us at least one way...
	if (waysToTakeUs > 0)
	{
		score += LOSE_PIECE_WEIGHT;
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

	//Is position defended and/or blocked
	bool right = defendedRight(move->state.board, endRow, endCol, playingForWhite);
	bool left = defendedLeft(move->state.board, endRow, endCol, playingForWhite);
	bool block = blocked(move->state.board, endRow, endCol, playingForWhite);

	
	if (right && left)
	{
		score += DOUBLE_DEFENDED_WEIGHT;
	}
	else if (right || left)
	{
		score += DEFENDED_WEIGHT;
	}

	if (blocked)
	{
		score += BLOCKED_WEIGHT;
	}

	//move->state.board.getPieceAt(5, 7);



	//Return the current score
	return score;
}


bool defendedLeft(Board board, int row, int col, bool playingForWhite)
{
	if (playingForWhite)
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row + 1;

		//Return whether or not their is a piece there
		return board.blackRows[rowEnemyDefendingFrom] % COLUMNS[col - 1] == 0;
	}
	else
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row - 1;

		//Return whether or not their is a piece there
		return board.whiteRows[rowEnemyDefendingFrom] % COLUMNS[col - 1] == 0;
	}
}

bool defendedRight(Board board, int row, int col, bool playingForWhite)
{
	if (playingForWhite)
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row + 1;

		//Return whether or not their is a piece there
		return board.blackRows[rowEnemyDefendingFrom] % COLUMNS[col - 1] == 0;
	}
	else
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row - 1;

		//Return whether or not their is a piece there
		return board.whiteRows[rowEnemyDefendingFrom] % COLUMNS[col - 1] == 0;
	}
}

//Return true if the piece is going to be blocked
bool blocked(Board board, int row, int col, bool playingForWhite)
{
	if (playingForWhite)
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row + 1;

		//Return whether or not their is a piece there
		return board.blackRows[rowEnemyDefendingFrom] % COLUMNS[col] == 0;
	}
	else
	{
		//Find the row the enemy is defending from
		int rowEnemyDefendingFrom = row - 1;

		//Return whether or not their is a piece there
		return board.whiteRows[rowEnemyDefendingFrom] % COLUMNS[col] == 0;
	}
}




//*******END EVALUATION SUB FUNCTIONS *******//


void runMonteCarloAlgorithm(Node* root, Board mboard, bool isWhitesTurn)
{
	//Set the current and end times
	std::time_t current = std::time(0);
	std::time_t end = current + SECONDS_TO_WORK;
	bool stop = false;


	//While we are not out of time
	for (int b = 0; !stop; b++)
	{
		//Select a promising node
		Node* node = selectPromisingNode(root);

		expandNode(node);

		int i = rand() % getNodeChildren(node);
		node = getChildNode(node, i);

		processNode(node, isWhitesTurn);

		//Update the current time+
		if (b % 1000 == 0)
		{
			current = std::time(0);
			if (current > end)
			{
				stop = true;
			}
		}

	}

}


void processNode(Node* node, bool isWhitesTurn)
{
	CriticalSectionLock lock(node->cs);
	bool whiteDidWin = executeRandomGame(node->state.board, node->state.isWhitesTurn);
	backTrackValues(node, whiteDidWin == isWhitesTurn);
}

Node* getChildNode(Node* node, int i)
{
	CriticalSectionLock lock(node->cs);
	return node->children[i];
}

int getNodeChildren(Node* node)
{
	CriticalSectionLock lock(node->cs);
	return node->children.size();
}


bool executeRandomGame(Board& rawBoard, bool isWhitesTurn)
{
	//Make a copy of the board
	Board board = Board(rawBoard);

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
		short int target = (rand() % 3) - 1;
		bool validMoveExists = false;
		bool successfulMove = false;

		//Run the move and see if it is valid
		//bool successfulMove = board.makeMove(isWhitesTurn, row, col, row + moveChange, col + target);

		//Loops until valid move is found or target == 2. If target == 2, means validMove became false and we've gone around again at least once.
		//At this point, we will just choose a new random move all over again.
		while (!successfulMove && target != 2)
		{
			successfulMove = board.makeMove(isWhitesTurn, row, col, row + moveChange, col + target);
			incrementTarget(target, validMoveExists);
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

void incrementTarget(short int& target, bool validMoveExists)
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
}


bool compareMove(const Move& a, const Move& b)
{
	return a.value > b.value;
}



/*
selectPromisingNode

Receives a Node for the root of the tree and starts with that as the node.
The children of the node are all examined for their UTCRating, and the best
is selected. This becomes the new node.

When the node has no children, the end of the tree has been reached, and the
node is returned.

*/
Node* selectPromisingNode(Node* root)
{
	Node* node = root;

	//Continue down the tree until the end is reached
	int numberOfChildren = getNodeChildren(node);
	if (numberOfChildren > 0)
	{
		do
		{
			CriticalSectionLock lockNodeWhile(node->cs);
			//Find the node with the best UTC rating
			double bestRating = -DBL_MAX;
			int bestRatingLocation = 0;

			for (unsigned int i = 0; i < node->children.size(); i++)
			{
				Node* currentNode = node->children[i];
				CriticalSectionLock lock2(currentNode->cs);

				double rating = getUCTRating(currentNode, root->state.isWhitesTurn);
				if (rating > bestRating)
				{
					bestRating = rating;
					bestRatingLocation = i;
				}
			}

			node = node->children[bestRatingLocation];
			CriticalSectionLock lockNodeWhile2(node->cs);
			numberOfChildren = node->children.size();

		} while (numberOfChildren > 0);
	}

	return node;
}



void expandNode(Node* node)
{	
	CriticalSectionLock lock(node->cs);

	//generate all the possible resulting states
	node->state.generateAllMoves();

	//for each resulting state, create a node and add it to the children
	for (unsigned int i = 0; i < node->state.allMoves.size(); i++)
	{
		Node* noob = new Node(node, node->state.board, node->state.allMoves[i], node->state.isWhitesTurn);
		node->children.emplace_back(noob);
	}
}

void backTrackValues(Node* node, bool whiteDidWin)
{

	//Did we, the AI, win?
	//If whitewillwin and whitejustmoved (!isWhitesTurn)
	//or !whitewillwin and blackjustmove (isWhitesturn)
	if (whiteDidWin != node->state.isWhitesTurn)
	{
		//Increase visits and wins by 1 all the way to the top of the tree
		while (node != NULL)
		{
			//Increase visits and wins
			node->state.visits++;
			node->state.wins++;

			//Move up the tree a step
			node = node->parent;
		}
	}
	else
	{
		//Increase visits ONLY (not wins) by 1 all the way to the top of the tree
		while (node != NULL)
		{
			//Increase visits
			node->state.visits++;

			//Move up the tree a step
			node = node->parent;
		}
	}
	
}

double getUCTRating(Node* node, bool isWhitesTurnMaster)
{
	//Return -infinity if the node is null
	if (node == NULL)
	{
		return DBL_MIN;
	}

	CriticalSectionLock lock(node->cs);

	//Return max value if unvisited
	if (node->state.visits == 0) {
		return DBL_MAX;
	}

	//NEED TO INVERT STATE WINS? NO. A win is a win for us.
	double wins = node->state.wins;

	//See formula on Wikipedia and other sources
	return  (wins / (double)node->state.visits)
		+ sqrt(2 * log((double)node->parent->state.visits / (double)node->state.visits));
}


/*
void main()
{
Board myBoard = Board();
myBoard.makeMove(true, 1, 6, 2, 4, false);
AIGetMove(myBoard.blackCount, myBoard.whiteCount, myBoard.blackRows, myBoard.whiteRows, false);
}*/

