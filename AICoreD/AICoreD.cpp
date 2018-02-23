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


extern "C" __declspec(dllexport) void __stdcall  EmptyMemory()
{
	//Prune the whole tree
	Node** test = new Node*;
	std::thread pruner(ThreadPruner::pruneAllAbove, root, *test);
	pruner.join();
	delete test;
}

extern "C" __declspec(dllexport) SubmitMove __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned long black, unsigned long white, bool isWhitesTurn, unsigned int mode)
{
	//Init the random number generator
	srand(time(NULL));

	//Init the current board from the parameters
	Board board;
	board.setParameters(blackCount, whiteCount, black, white);

	//Save the original root for pruning
	Node* originalRoot = root;

	//Setup the roots
	setRoot(root, board, isWhitesTurn);


	//Start four monte carlo evaluation threads
	std::thread mc(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	/*std::thread mc2(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc3(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc4(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc5(runMonteCarloAlgorithm, root, board, isWhitesTurn);
	std::thread mc6(runMonteCarloAlgorithm, root, board, isWhitesTurn);*/


	//Join all the threads
	mc.join();
	/*mc2.join();
	mc3.join();
	mc4.join();
	mc5.join();
	mc6.join();*/


	//Find the first level child with the max value as the result
	double maxValue = -1;
	Node* result = NULL;

	//For each of the first level children (every move we could choose)....
	for (unsigned int i = 0; i < root->childCount; i++)
	{
		//give it a value from the evaluate function
		double value = Evaluator::evaluate(root->child[i], board, isWhitesTurn);

		//If the value is the best value, then choose it
		if (value > maxValue)
		{
			//To choose the move, save the new value and set the move Node as the result
			maxValue = value;
			result = root->child[i];

			//this will be overriden if a new best move is found later
		}
	}

	//write to the log file
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ofstream log_file("AILog.txt", std::ios_base::out | std::ios_base::app);
	log_file << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "\t" << root->state.visits << "\t" << maxValue << "\t" << originalRoot << std::endl;

	//Prune above the root
	std::thread pruner(ThreadPruner::pruneAllAbove, originalRoot, root);
	pruner.detach();

	//Prune above the result
	result->parent = NULL;
	std::thread pruner2(ThreadPruner::pruneAllAbove, root, result);
	pruner2.detach();


	//set the result as the new root
	root = result;
	

	//return the sourceMove from the result
	return result->state.sourceMove.toSubmitMove();
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
		unsigned int size = root->childCount;
		for (unsigned int i = 0; (!foundNewRoot && i < size); i++)
		{
			//if the current boardstate is found
			if (r->child[i]->state.board == board)
			{
				//set the new root
				r = r->child[i];

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
	return node->child[i];
}

int getNodeChildren(Node* node)
{
	CriticalSectionLock lock(node->cs);
	return node->childCount;
}


bool executeRandomGame(Board& rawBoard, bool isWhitesTurn)
{
	//Make a copy of the board
	Board board = Board(rawBoard);

	while (!board.gameOver)
	{
		if (isWhitesTurn)
		{
			//constants
			const int ROW_CHANGE = 1;

			//variables
			unsigned int row;
			unsigned int col;
			unsigned int rowValue = 0;
			unsigned int myNextRowValue = 0;
			unsigned int theirNextRowValue = 0;
			unsigned int colRep = 0;

			int validFound = 0;

			//choose a row randomly  
			row = rand() % 8;

			//loop until a valid row (with a valid piece to move) is found
			while (!validFound)
			{

				//if we we found a row with a piece, check for the valid column
				if (!board.white >> row * 8 == 0)
				{
					//set the parameters for quick checking of each column
					rowValue = board.white >> row * 8;
					myNextRowValue = board.white >> (row + ROW_CHANGE) * 8;
					theirNextRowValue = board.black >> (row + ROW_CHANGE) * 8;

					//choose a col randomly (guarenteed to find one) and then loop until a valid row (with a piece to move) is found
					col = rand() % 8;

					//copy the initial column so we know when we've finished the row
					unsigned int initialCol = col;


					//check every column until we've looped around or found one
					do
					{
						//if there's at least something in the col
						if (!( (rowValue & board.COLUMNS[col]) == 0) )
						{
							//check if we found a valid solution
							if (/* leftClear*/ (myNextRowValue & (colRep << 1)) == 0)
								validFound += 1;
							if (/*centerClear*/ (myNextRowValue & colRep) == 0 && (theirNextRowValue & colRep) == 0)
								validFound += 2;
							if (/*rightClear*/ (myNextRowValue & (colRep >> 1)) == 0)
								validFound += 4;
						}

						//advance to the next col if not found yet
						if (!validFound)
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

					} while (!validFound && col != initialCol);

				}


				//advance to the next row if not found yet
				if (!validFound)
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
			}

			//now that we have a valid solution, execute it
			unsigned int target;
			switch (validFound)
			{
			case 1:	// l--
				target = 0;
				break;
			case 2:	// -c-
				target = 1;
				break;
			case 3: // lc-
				target = rand() % 2;
				break;
			case 4: // --r
				target = 2;
				break;
			case 5: // l-r
				target = rand() % 2;
				if (target == 1)
					target++;
				break;
			case 6: // -cr
				target = rand() % 2 + 1;
				break;
			case 7: // lcr
				target = rand() % 3;
				break;
			}

			if (board.makeMove(isWhitesTurn, row, col, row + ROW_CHANGE, col + target - 1))
				isWhitesTurn = !isWhitesTurn;

		}
		else
		{
			//constants
			const int ROW_CHANGE = -1;

			//variables
			unsigned int row;
			unsigned int col;
			unsigned int rowValue = 0;
			unsigned int myNextRowValue = 0;
			unsigned int theirNextRowValue = 0;
			unsigned int colRep = 0;

			int validFound = 0;

			//choose a row randomly  
			row = rand() % 8;

			//loop until a valid row (with a valid piece to move) is found
			while (!validFound)
			{

				//if we we found a row with a piece, check for the valid column
				if (!board.black >> row * 8 == 0)
				{
					//set the parameters for quick checking of each column
					rowValue = board.black >> row * 8;
					myNextRowValue = board.black >> (row + ROW_CHANGE) * 8;
					theirNextRowValue = board.white >> (row + ROW_CHANGE) * 8;

					//choose a col randomly (guarenteed to find one) and then loop until a valid row (with a piece to move) is found
					col = rand() % 8;

					//copy the initial column so we know when we've finished the row
					unsigned int initialCol = col;


					//check every column until we've looped around or found one
					do
					{
						//if there's at least something in the col
						if (!(rowValue & board.COLUMNS[col]) == 0)
						{
							//check if we found a valid solution
							if (/* leftClear*/ (myNextRowValue & (colRep << 1)) == 0)
								validFound += 1;
							if (/*centerClear*/ (myNextRowValue & colRep) == 0 && (theirNextRowValue & colRep) == 0)
								validFound += 2;
							if (/*rightClear*/ (myNextRowValue & (colRep >> 1)) == 0)
								validFound += 4;
						}

						//advance to the next col if not found yet
						if (!validFound)
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

					} while (!validFound && col != initialCol);

				}


				//advance to the next row if not found yet
				if (!validFound)
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
			}

			//now that we have a valid solution, execute it
			unsigned int target;
			switch (validFound)
			{
			case 1:	// l--
				target = 0;
				break;
			case 2:	// -c-
				target = 1;
				break;
			case 3: // lc-
				target = rand() % 2;
				break;
			case 4: // --r
				target = 2;
				break;
			case 5: // l-r
				target = rand() % 2;
				if (target == 1)
					target++;
				break;
			case 6: // -cr
				target = rand() % 2 + 1;
				break;
			case 7: // lcr
				target = rand() % 3;
				break;
			}

			if (board.makeMove(isWhitesTurn, row, col, row + ROW_CHANGE, col + target - 1))
				isWhitesTurn = !isWhitesTurn;

		}

	}

	//Return whoever just played and therefore won the game
	return !isWhitesTurn;
}

void incrementTarget(int& target, bool validMoveExists)
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

			for (unsigned int i = 0; i < node->childCount; i++)
			{
				Node* currentNode = node->child[i];
				CriticalSectionLock lock2(currentNode->cs);

				double rating = getUCTRating(currentNode, root->state.isWhitesTurn);
				if (rating > bestRating)
				{
					bestRating = rating;
					bestRatingLocation = i;
				}
			}

			node = node->child[bestRatingLocation];
			CriticalSectionLock lockNodeWhile2(node->cs);
			numberOfChildren = node->childCount;

		} while (numberOfChildren > 0);
	}

	return node;
}



void expandNode(Node* node)
{	
	CriticalSectionLock lock(node->cs);

	//generate all the possible resulting states, create a node and add it to the children

	//set the terms
	int myPieces = node->state.board.black;
	int theirPieces = node->state.board.white;
	int rowChange = -1;
	if (node->state.isWhitesTurn)
	{
		rowChange = 1;
		myPieces = node->state.board.white;
		theirPieces = node->state.board.black;
	}

	//until we've looked at all the pieces
	int row = 0;
	int col = 0;
	int pieces = 0;
	while (pieces != 0)
	{
		//if we've found a piece
		if (pieces & 1)
		{
			//check what is moves are and add a node for each

			////*check for target = 0*/////
			if (col > 0)
			{

				//if there is not one of our pieces there
				if (!(myPieces >> ((row + rowChange) * 8 + col - 1)) & 1)
				{
					//add the node
					node->child[node->childCount] = new Node(node, node->state.board, Move(row, col, 0), node->state.isWhitesTurn);
					node->childCount++;
				}
			}

			////*check for target = 1*/////
			if (col < 7)
			{
				//if there is not one of our pieces there
				if (!(myPieces >> ((row + rowChange) * 8 + col)) & 1)
				{
					//and there is not one of their pieces there
					if (!(theirPieces >> ((row + rowChange) * 8 + col)) & 1)
					{
						//add the node
						node->child[node->childCount] = new Node(node, node->state.board, Move(row, col, 1), node->state.isWhitesTurn);
						node->childCount++;
					}
				}
			}

			////*check for target = 2*/////
			if (col < 7)
			{

				//if there is not one of our pieces there
				if (!(myPieces >> ((row + rowChange) * 8 + col + 1)) & 1)
				{
					//add the node
					node->child[node->childCount] = new Node(node, node->state.board, Move(row, col, 2), node->state.isWhitesTurn);
					node->childCount++;
				}
			}

		}

		//advance the bits by 1
		pieces >>= 1;
		col++;
		if (col > 7)
		{
			row++;
			col = 0;
		}
	}


	/*//for each resulting state, create a node and add it to the children
	for (unsigned int i = 0; i < node->state.allMoves.size(); i++)
	{
		node->children.emplace_back(new Node(node, node->state.board, node->state.allMoves[i], node->state.isWhitesTurn));
	}*/
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

