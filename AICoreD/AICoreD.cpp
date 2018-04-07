//Breakthrough AI
//SoftDev Team 1
//AI Lead: Brandon Emlaw
//References: 
//	https://github.com/antirez/connect4-montecarlo/blob/master/c4.c
//	https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
//	http://www.baeldung.com/java-monte-carlo-tree-search
//	https://www.theatlantic.com/technology/archive/2017/07/marion-tinsley-checkers/534111/
//  https://webdocs.cs.ualberta.ca/~mmueller/ps/enzenberger-mueller-acg12.pdf
//	https://end.wikipedia.org/wiki/Alpha-beta_pruning
//	www.hongliangjie.com/2011/10/10/sortin-tuples-in-c/

//For tree deletion: https://www.geeksforgeeks.org/non-recursive-program-to-delete-an-entire-binary-tree/


//TODO
//	-Directly prune or bias against awful children
//	-track number of pieces on the 2nd to last row, for example, avoid the scenario leading to it
//  -summary: prune by evalution

#include "AICoreD.h"


extern "C" __declspec(dllexport) void __stdcall  EmptyMemory()
{
	//Prune the whole tree
	Node** test = new Node*;
	std::thread pruner(ThreadPruner::pruneAllAbove, root, *test);
	pruner.join();
	delete test;
	root = NULL;
}

extern "C" __declspec(dllexport) SubmitMove __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned long long black, unsigned long long white, bool isWhitesTurn, unsigned int mode)
{
	//IsWhitesTurn is true if the AI is moving for white


	//Reverse the board and turn indicator of moving for black
	bool reversed = false;
	if (!isWhitesTurn)
	{
		//flip the board
		unsigned long long int temp = reverse(black);
		black = reverse(white);
		white = temp;

		//flip the count
		int temp2 = blackCount;
		blackCount = whiteCount;
		whiteCount = temp2;

		//flip the turn indicator
		isWhitesTurn = true;

		//set the reversed flag
		reversed = true;
	}


	//Init the clock and end time with the current parameters
	std::time_t current = std::time(0);

	//Set the endtime based on the mode
	std::time_t endTime = current + SECONDS_TO_WORK;
	if (mode != 0)
	{
		endTime = current + SECONDS_TO_WORK_EASY;
	}


	//Init the random number generator
	srand(time(NULL));


	//Init the current board from the parameters
	Board board;
	board.setParameters(blackCount, whiteCount, black, white);

	//Save the original and secondary root for pruning
	Node* originalRoot = root;

	//Setup the roots
	setRoot(root, board, isWhitesTurn);

	//If we're in easy mode, give it a chance of making a rash move
	if (mode == 2 && (rand() % RASH_MOVE_CHANCE == 0))
	{
	}
	else
	{
		//In nearly every case:
		//Run AB seeding on the tree
		seedWithAlphaBeta(root, isWhitesTurn);
	}

	//Start four monte carlo evaluation threads
	runMonteCarloAlgorithm(root, board, isWhitesTurn, endTime);
	std::thread mc(runMonteCarloAlgorithm, root, board, isWhitesTurn, endTime);
	std::thread mc2(runMonteCarloAlgorithm, root, board, isWhitesTurn, endTime);
	std::thread mc3(runMonteCarloAlgorithm, root, board, isWhitesTurn, endTime);
	std::thread mc4(runMonteCarloAlgorithm, root, board, isWhitesTurn, endTime);


	//Join all the threads
	mc.join();
	mc2.join();
	mc3.join();
	mc4.join();



	//Find the first level child with the max value as the result
	double maxValue = -1;
	Node* result = NULL;


	//Try choosing with deep search
	//Node* deepResult = chooseWithDeepSearch(root, DEEP_SEARCH_DEPTH);
	//if (deepResult == NULL)
	//{

		//Choose with monte carlo
		//For each of the first level children (every move we could choose)....
		Node* temp = root->child;
		while (temp != NULL)
		{
			//give it a value from the evaluate function
			double value = Evaluator::evaluate(temp, board, isWhitesTurn);

			//if mode is easy, randomize the value
			if (mode == 2)
			{
				//Reference: http://www.cplusplus.com/forum/beginner/26963/
				double change = (double)rand() / (RAND_MAX + 1) * RANDOM_ADDITION;
				value = value + change;
			}

			//double value = (double)temp->state.wins / (double)temp->state.visits;

			//If the value is the best value, then choose it
			if (value > maxValue)
			{
				//To choose the move, save the new value and set the move Node as the result
				maxValue = value;
				result = temp;

				//this will be overriden if a new best move is found later
			}

			//advance to the next child
			temp = temp->next;
		}
	/*}
	else
	{
		//Choose the result of the deep search
		result = deepResult;
	}*/


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
	SubmitMove resultMove = result->state.sourceMove.toSubmitMove();
	//invert the resultMove if neccessary
	if (reversed)
	{
		resultMove.col = 7 - resultMove.col;
		resultMove.row = 7 - resultMove.row;
		resultMove.target = 2 - resultMove.target;
	}
	return resultMove;
}


//Run choose with deep search - recieves a 
Node* chooseWithDeepSearch(Node* root, int depth)
{
	//Loop through each child - each o
	int maximum = 0; //INT_MIN; //set the maximum value to be beat to override MC
	Node* temp = root->child;

	//This line differs from deepSearch
	Node* choice = NULL;
	while (temp != NULL)
	{
		//only consider if the node is in conflict
		if (temp->flag)
		{
			//loop each possible opponent move
			Node* temp2 = temp->child;
			int minimum = INT_MAX;
			while (temp2 != NULL)
			{
				//run deep search on each node
				minimum = min(minimum, deepSearch(temp2, depth - 1));

				//advance to the next child
				temp2 = temp2->next;
			}

			//This line differs from deepSearch
			//If we found the new max, set the Node* choice
			if (minimum > maximum)
			{
				maximum = minimum;
				choice = temp;
			}

		}

		//advance to the next child
		temp = temp->next;
	}

	return choice;
}

int deepSearch(Node* root, int depth)
{
	//if we haven't reached our target depth or the end of the tree
	if ((depth > 0) && (root->childCount > 0))
	{
		//Loop through each child - each o
		int maximum = INT_MIN;
		Node* temp = root->child;
		while (temp != NULL)
		{
			//only consider if the node is in conflict
			if (temp->flag)
			{
				//loop each possible opponent move
				Node* temp2 = temp->child;
				int minimum = INT_MAX;
				while (temp2 != NULL)
				{
					//run deep search on each node
					minimum = min(minimum, deepSearch(temp2, depth - 2));

					//advance to the next child
					temp2 = temp2->next;
				}
				maximum = max(maximum, minimum);
			}

			//advance to the next child
			temp = temp->next;
		}

		return maximum;
	}
	else
	{
		//Evaluate
		return abEval(root);
	}
}


//Bitwise everses integers
//Code sourced from https://aticleworld.com/5-way-to-reverse-bits-of-an-integer/ 
unsigned long long int reverse(unsigned long long int num)
{
	unsigned int count = (64 - 1);
	unsigned long long int tmp = num;         //  Assign num to the tmp 

	num >>= 1; // shift num because LSB already assigned to tmp

	while (num)
	{
		tmp <<= 1;  //shift the tmp because alread have the LSB of num  

		tmp |= num & 1; // putting the set bits of num

		num >>= 1;

		count--;
	}

	tmp <<= count; //when num become zero shift tmp from the remaining counts

	return tmp;
}


//Operates the alpha beta seeding
Node* seedWithAlphaBeta(Node* root, bool isWhitesTurn)
{ 
	int inverter = 1;
	if (!isWhitesTurn)
	{
		inverter = -1;
	}

	//int vals[50] = { 0 };
	
	std::vector<std::tuple <Node*,int>> values;
	std::vector<Node*> results;
	int maxVal = INT_MIN;

	//if there are children
	if (root->childCount > 0)
	{
		//For each child
		int i = 0;
		Node* node = root->child;
		while (node != NULL)
		{
			//If the node is with a piece in conflict, run DEEP SEARCH on it
			if (node->flag)
			{
				//loop each possible opponent move
				Node* temp2 = node->child;
				int minimum = INT_MAX;
				while (temp2 != NULL)
				{
					//run deep search on each node
					minimum = min(minimum, deepSearch(temp2, DEEP_SEARCH_DEPTH));

					//advance to the next child
					temp2 = temp2->next;
				}

				//This line differs from deepSearch
				//If we found the new max, set the Node* choice
				if (minimum > maxVal)
				{
					maxVal = minimum;

				}

				//add the item to the list of values
				std::tuple<Node*, int> pair =
					std::tuple<Node*, int>(node, minimum);
				values.push_back(pair);

			}
		
			//If the node does not involve a piece in conflict, do minimal Alpha Beta searching
			
			else
			{

				//run an alpha beta evaluation
				//pass in isWhitesTurn for maximizing player, since white is maxed and black is min-ed in this algorithm
				//save the index and value into the values

				//TODO - alpha beta DISABLED
				/*std::tuple<Node*, int> pair =
					std::tuple<Node*, int>(node, alphaBeta(node, ALPHA_BETA_DEPTH, INT_MIN, INT_MAX, !isWhitesTurn) * inverter);
				*/
				std::tuple<Node*, int> pair =
				std::tuple<Node*, int>(node, abEval(node));
				
				values.push_back(pair);
			}

			//Advance to the next child
			i++;
			node = node->next;
		}

		//sort the values 
		std::sort(values.begin(), values.end(), compareABPairs);

		//pick the best 5 into the results
		const int BEST_TO_KEEP = 1;
		int found = 0;
		int valueCheck = std::get<1>(values.back()); //the comparison variable begins with the first value, 
		//to ensure one accepted value
		while (!values.empty() && (found < BEST_TO_KEEP || std::get<1>(values.back()) == valueCheck))
		{
			//add the result pointer to the list
			std::tuple<Node*, int> val = values.back();
			results.emplace_back(std::get<0>(val));
			values.pop_back();

			//increment the found
			found++;

			//update the value checkeds
			valueCheck = std::get<1>(val);

		}

		/*//NOTE
		//preserve any values that are in conflict
		for (int i = 0; i < values.size(); i++)
		{
			//get the result value
			std::tuple<Node*, int> val = values[i];

			//if the value is flagged, preserve it to the results
			if (std::get<0>(val)->flag)
			{
				//add it to the results
				results.emplace_back(std::get<0>(val));

				//swap it to the back of the values and remove it
				std::swap(values[i], values.back());
				values.pop_back();

				//decrement the counter so the search continues correctly
				i--;

				//increment the number found
				found++;

			}
		}*/

		//rebuild the tree with the new results
		if (results.size() > 0)
		{
			//init the result tree
			//Node* result = new Node(*root);
			Node* result = root;

			//add the first child
			result->childCount = 1;
			result->child = results.back();
			//result->child->parent = result;

			results.pop_back();

			//add every child
			Node* node = result->child;
			while (!results.empty())
			{
				//grab the node from the last in the results list
				Node* next = results.back();
				next->parent = result;
				results.pop_back();

				//save the node as the next one and increment the count
				node->next = next;
				result->childCount++;

				//advance to the next one
				node = next;
			}

			//set the last next pointer to null
			node->next = NULL;


			//****  delete original root   ****/
			//until we have emptied the unusued values array
			while (!values.empty())
			{
				//get the first node from the queue
				Node* node = std::get<0>(values.back());
				values.pop_back();

				//For each of the  children 
				Node* temp = node->child;
				while (temp != NULL)
				{
					//add to the queue
					values.push_back(std::tuple<Node*, int>(temp, 0));

					//advance to the next child
					temp = temp->next;
				}


				//actually delete the node
				delete node;
					
			}


			//set the result's successor to be null, so it isn't deleted twice
			result->next = NULL;

			//return the resulting tree
			return result;
		}
		else
		{
			//if we have no selected notes, return the existing root
			return root;
		}
	}
	else
	{
		//if there is nothing to evaluate, return the existing root
		return root;
	}

}

//Compare ab result pairs
bool compareABPairs(std::tuple<Node*, int>& first, std::tuple<Node*, int>& second)
{
	return std::get<1>(first) < std::get<1>(second);
}

//Execute the alpha beta evaluation
int alphaBeta(Node* node, int depth, int alpha, int beta, bool maximizingPlayer)
{
	if ((depth == 0) || (node->childCount == 0))
	{
		return abEval(node);
	}
	else
	{
		//If we're maximizing
		if (maximizingPlayer)
		{
			//set the value to the minimum possible
			int v = INT_MIN;

			//For each child node
			node = node->child;
			while (node != NULL)
			{
				v = max(v, alphaBeta(node, depth - 1, alpha, beta, FALSE));
				if (depth == 3)
				{
					bool b = true;
				}

				alpha = max(alpha, v);
				if (beta <= alpha)
				{
					//escape the loop and kill the kids
					node = NULL;
				}

				//If we're not already exiting, advance to the next child
				if (node != NULL)
				{
					node = node->next;
				}
			}


			return v;


		}
		//If we're minimizing
		else
		{
			//set the value to the minimum possible
			int v = INT_MAX;

			//For each child node
			node = node->child;
			while (node != NULL)
			{
				v = min(v, alphaBeta(node, depth - 1, alpha, beta, FALSE));
				beta = min(beta, v);
				if (beta <= alpha)
				{
					//escape the loop and kill the kids
					node = NULL;
				}

				//If we're not already exiting, advance to the next child
				if (node != NULL)
				{
					node = node->next;
				}
			}
			return v;
		}
	}
}

//Returns the AB value of a node
//ASSUMES MAX IS FOR WHITE
int abEval(Node* node)
{
	//If the game is totally over, return +/- 2000
	if (node->state.board.gameOver)
	{
		if (node->state.isWhitesTurn)
		{
			return -2000;
		}
		else
		{
			return 2000;
		}
	}
	//If the game has reached a victory location, +/- return 1000
	/*else if (node->state.board.victoryLocation > 0)
	{
		if (node->state.board.victoryLocation = 2) //black
		{
			return -1000;
		}
		else
		{
			return 1001;
		}
	}*/

	int result =  (node->state.board.whiteCount - node->state.board.blackCount);
	return result;
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

		//For each move 
		Node* temp = r->child;
		while (temp != NULL && !foundNewRoot)
		{
			//if the current boardstate is found
			if (temp->state.board == board)
			{
				//set the new root
				r = temp;

				//set the root's parent to null
				r->parent = NULL;

				//set the found flag
				foundNewRoot = true;
			}

			//advance to the next child
			temp = temp->next;
		}

			

		//if we didn't find one, clear the memory set the root over again to empty
		if (!foundNewRoot)
		{
			//dump the tree
			//Note: this will cause a memory leak; therefore, it is a LAST RESTORT!
			r = Node::rootNode(board, isWhitesTurn);

		}
	}
}




void runMonteCarloAlgorithm(Node* root, Board mboard, bool isWhitesTurn, std::time_t endTime)
{
	bool stop = false;
	bool flag = false;

	int previousVisits = 0;

	//While we are not out of time
	for (int b = 1; !stop; b++)
	{
		try
		{

			//Select a promising node
			Node* node = selectPromisingNode(root);

			expandNode(node);

			//If the node has children, select a random child
			if (node->childCount > 0)
			{
				int i = rand() % getNodeChildren(node);
				node = getChildNode(node, i);
			}
			else
			{
				flag = true;
			}

			//If the node is not NULL, process the node
			if (node != NULL)
			{
				processNode(node, isWhitesTurn);
			}
		}
		catch (...)
		{
			//write error to the log file
			std::ofstream log_file("AILog.txt", std::ios_base::out | std::ios_base::app);
			log_file << "ERROR OCCURED and was CAUGHT\n";
		}

		//Update the current time+
		if (b % 1000 == 0 || flag)
		{
			if (std::time(0) > endTime)
			{
				stop = true;
			}
		}

	}

}


void processNode(Node* node, bool isWhitesTurn)
{
	bool whiteDidWin = executeRandomGame(node->state.board, node->state.isWhitesTurn);
	backTrackValues(node, whiteDidWin == isWhitesTurn);
}

Node* getChildNode(Node* node, int i)
{
	//CriticalSectionLock lock(node->cs);

	Node* temp = node->child;
	int c = 1;
	while (temp != NULL && c <= i)
	{
		temp = temp->next;
		c++;
	}

	return temp;
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

	while (! (board.gameOver || board.black & ROWS[0] || board.white & ROWS[7]))
	{
		if (isWhitesTurn)
		{


			//constants
			const int ROW_CHANGE = 1;

			//variables
			unsigned int row;
			unsigned int col;
			unsigned int target;

			unsigned long long rowValue = 0;
			unsigned long long myNextRowValue = 0;
			unsigned long long theirNextRowValue = 0;
			unsigned int colRep = 0;

			int validFound = 0;

			//take a piece if we can
			unsigned long long whitePieces = board.white;
			unsigned long long blackPieces = board.black;
			int count = 0;

			bool foundFirstGo = false;

			while (whitePieces && !foundFirstGo)
			{
				if (whitePieces & 1)
				{
					//checks ahead a row, one to left and one to right
					if (blackPieces & 640)
					{
						row = count / 8;
						col = count % 8;
						if (blackPieces & 512)
						{
							if (col < 7)
							{
								target = 2;
								foundFirstGo = true;
							}
						}
						//TODO: need to randomize between these two!!
						else if (blackPieces & 128)
						{
							if (col > 0)
							{
								target = 0;
								foundFirstGo = true;
							}
						}

					}
				}

				//advance the spot we're looking at
				whitePieces >>= 1;
				blackPieces >>= 1;
				count++;
			}

			if (!foundFirstGo)
			{

				//choose a row randomly  
				row = rand() % 8;


				//loop until a valid row (with a valid piece to move) is found
				while (!validFound)
				{

					//if we we found a row with a piece, check for the valid column
					if (board.white & ROWS[row])
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
							colRep = COLUMNS[col];
							if (!((rowValue & colRep) == 0))
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
			unsigned int target;

			unsigned long long rowValue = 0;
			unsigned long long myNextRowValue = 0;
			unsigned long long theirNextRowValue = 0;
			unsigned int colRep = 0;


			int validFound = 0;



			//take a piece if we can
			unsigned long long whitePieces = board.white;
			unsigned long long blackPieces = board.black;
			int count = 0;

			bool foundFirstGo = false;

			while (whitePieces && !foundFirstGo)
			{
				if (whitePieces & 1)
				{
					//checks ahead a row, one to left and one to right
					if (blackPieces & 640)
					{
						if (blackPieces & 512)
						{
							int c =  count + 9;
							row = c / 8;
							col = c % 8;
							target = 0;

							if (col > 0)
							{
								foundFirstGo = true;
							}
						}
						else if (blackPieces & 128)
						{
							int c = count + 7;
							row = c / 8;
							col = c % 8;
							target = 2;

							if (col < 7)
							{
								foundFirstGo = true;
							}
						}

					}
				}

				//advance the spot we're looking at
				whitePieces >>= 1;
				blackPieces >>= 1;
				count++;
			}
			if (!foundFirstGo)
			{

				//choose a row randomly  
				row = rand() % 8;

				//loop until a valid row (with a valid piece to move) is found
				while (!validFound)
				{

					//if we we found a row with a piece, check for the valid column
					if (board.black & ROWS[row])
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
							colRep = COLUMNS[col];
							if (!(rowValue & colRep) == 0)
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
			}
			if (row == 1 && (col + target - 1) == 0 && board.blackCount == 1 && board.white == 4503766193143808)
			{
				int b = 1;
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
	
	if (node->childCount > 0)
	{
		do
		{
			CriticalSectionLock lockNodeWhile(node->cs);
			//Find the node with the best UTC rating
			double bestRating = -DBL_MAX;
			Node* bestRatingLocation = NULL;

			//For each of the first level children (every move we could choose)....
			Node* temp = node->child;
			while (temp != NULL)
			{
				CriticalSectionLock lock2(temp->cs);

				double rating = getUCTRating(temp, root->state.isWhitesTurn);
				if (rating > bestRating)
				{
					bestRating = rating;
					bestRatingLocation = temp;
				}


				//advance to the next child
				temp = temp->next;
				
			}

			

			/*for (unsigned int i = 0; i < node->childCount; i++)
			{
				Node* currentNode = node->child[i];
				CriticalSectionLock lock2(currentNode->cs);

				double rating = getUCTRating(currentNode, root->state.isWhitesTurn);
				if (rating > bestRating)
				{
					bestRating = rating;
					bestRatingLocation = i;
				}
			}*/

			node = bestRatingLocation;
			//CriticalSectionLock lockNodeWhile2(node->cs);


		} while (node->childCount > 0);
	}

	if (node == NULL)
	{
		int b = 0;
	}

	return node;
}



void expandNode(Node* node)
{	
	CriticalSectionLock lock(node->cs);

	//Only expand the node if the game isn't over
	if (!(node->state.board.gameOver))
	{

		//set the terms
		unsigned long long myPieces = node->state.board.black;
		unsigned long long theirPieces = node->state.board.white;
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
		unsigned long long pieces = myPieces;
		Node* temp = node;
		while (pieces != 0)
		{
			//if we've found a piece
			if (pieces & 1)
			{
				CriticalSectionLock lock(temp->cs);
				//check what is moves are and add a node for each

				////*check for target = 0*/////
				if (col > 0)
				{

					//if there is not one of our pieces there
					if (!((myPieces >> ((row + rowChange) * 8 + col - 1)) & 1))
					{
						//add the node
						if (node->childCount == 0)
						{
							//if there are no children, add it as the first child
							node->child = new Node(node, node->state.board, Move(row, col, 0), node->state.isWhitesTurn);
							temp = node->child;
						}
						else
						{
							temp->next = new Node(node, node->state.board, Move(row, col, 0), node->state.isWhitesTurn);
							temp = temp->next;
						}
						node->childCount++;
					}
				}

				////*check for target = 1*/////
				if (col < 7)
				{
					CriticalSectionLock lock(temp->cs);


					//if there is not one of our pieces there
					if (!((myPieces >> ((row + rowChange) * 8 + col)) & 1))
					{
						//and there is not one of their pieces there
						if (!((theirPieces >> ((row + rowChange) * 8 + col)) & 1))
						{
							//add the node
							if (node->childCount == 0)
							{
								//if there are no children, add it as the first child
								node->child = new Node(node, node->state.board, Move(row, col, 1), node->state.isWhitesTurn);
								temp = node->child;

							}
							else
							{
								temp->next = new Node(node, node->state.board, Move(row, col, 1), node->state.isWhitesTurn);
								temp = temp->next;
							}
							node->childCount++;
						}
					}
				}

				////*check for target = 2*/////
				if (col < 7)
				{
					CriticalSectionLock lock(temp->cs);

					//if there is not one of our pieces there
					if (!((myPieces >> ((row + rowChange) * 8 + col + 1)) & 1))
					{
						//add the node
						if (node->childCount == 0)
						{
							//if there are no children, add it as the first child
							node->child = new Node(node, node->state.board, Move(row, col, 2), node->state.isWhitesTurn);
							temp = node->child;
						}
						else
						{
							temp->next = new Node(node, node->state.board, Move(row, col, 2), node->state.isWhitesTurn);
							temp = temp->next;
						}
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

	}


}

void backTrackValues(Node* node, bool didWeWin)
{

	//Did we, the AI, win?
	if (didWeWin)
	{
		CriticalSectionLock lock(node->cs);
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
		CriticalSectionLock lock(node->cs);
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
	if (node->parent == NULL)
	{
		return 0;
	}

	CriticalSectionLock lock(node->cs);

	//Return max value if unvisited
	if (node->state.visits == 0) {
		return DBL_MAX;
	}

	//NEED TO INVERT STATE WINS? NO. A win is a win for us.
	double wins = node->state.wins;
	

	//See formula on Wikipedia and other sources
	return (wins / (double)node->state.visits)
		+ TREE_BALANCE_CONSTANT * sqrt(2 * log((double)node->parent->state.visits / (double)node->state.visits));
}


/*
void main()
{
Board myBoard = Board();
myBoard.makeMove(true, 1, 6, 2, 4, false);
AIGetMove(myBoard.blackCount, myBoard.whiteCount, myBoard.blackRows, myBoard.whiteRows, false);
}*/
