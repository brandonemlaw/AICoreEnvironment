#include "Node.h"



Node::Node(Node* myParent, Board myBoard, Move myMove, bool myIsWhitesTurn)
{
	//Init the critical section for multithread locking
	InitializeCriticalSection(&cs);

	//Get the board and turn
	state.board.setParameters(myBoard.blackCount, myBoard.whiteCount, myBoard.black, myBoard.white);
	state.isWhitesTurn = !myIsWhitesTurn;

	//set the new row by the direction of travel
	int newRow = myMove.row - 1;
	if (myIsWhitesTurn)
	{
		newRow = myMove.row + 1;
	}

	//make the move
	state.board.makeMove(myIsWhitesTurn, myMove.row, myMove.col, newRow, myMove.col + myMove.target - 1);

	//set the visits and wins
	state.visits = 0;
	state.wins = 0;

	//set the parent
	parent = myParent;

	//set the childCount
	childCount = 0;

	//set the source move 
	state.sourceMove = myMove;
}

Node::~Node()
{
	DeleteCriticalSection(&cs);
}

Node::Node()
{
	//Init the critical section for multithread locking
	InitializeCriticalSection(&cs);

	//set the childCount
	childCount = 0;
}


//Used only for creating the root node
Node* Node::rootNode(Board newBoard, bool isWhitesTurn)
{
	Node* result = new Node();
	result->parent = NULL;
	result->state.board.setParameters(newBoard.blackCount, newBoard.whiteCount, newBoard.black, newBoard.white);
	result->state.isWhitesTurn = isWhitesTurn;
	result->state.visits = 0;
	result->state.wins = 0;
	result->childCount = 0;

	return result;
}