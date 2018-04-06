#include "Node.h"


Node::Node(Node& source)
{
	//Init the critical section for multithread locking
	InitializeCriticalSection(&cs);

	//copy all of the parameters
	state = source.state;
	parent = source.parent;
	childCount = source.childCount;
	next = source.next;
	child = source.child;
	flag = source.flag;
}

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

	next = NULL;
	child = NULL;

	//set the source move 
	state.sourceMove = myMove;

	//set the flag
	flag = isInConflict();

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
	child = NULL;
	next = NULL;
	
	//set the flag
	flag = true;
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
	result->flag = true;

	return result;
}

bool Node::isInConflict()
{
	//If we have no parent, default to being in conflict
	if (this->parent == NULL)
	{
		return true;
	}

	//TODO row 6?
	//If it is near winning (at or beyond row 6), it is "in conflict"
	if (this->state.sourceMove.row > 6)
	{
		return true;
	}

	//If there is an enemy piece one or two rows ahead, it is in conflcit

	//move to the target piece in question by dividing the board by the value of the square of origin
	unsigned long long pieces = this->parent->state.board.black;
	pieces /= SQUARES[this->state.sourceMove.row][this->state.sourceMove.col];

	//If there is a merge with the squares ahead
	if (pieces & SPACES_AHEAD)
	{
		return true;
	}

	//If we haven't detected a conflict, then return false.
	return false;
}