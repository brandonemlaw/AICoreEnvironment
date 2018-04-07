#pragma once

#include "State.h"
#include <vector>
#include <math.h>
#include <Windows.h>

//DEBUG
#include <string>

class Node
{
public:
	State state;
	Node* parent;
	Node* next;
	Node* child;
	int childCount;

	bool flag;

	//DEBUG
	std::string origin;


	Node::Node(Node& source);
	Node::Node(Node* myParent, Board myBoard, Move myMove, bool myIsWhitesTurn);
	~Node();


	static Node* rootNode(Board newBoard, bool isWhitesTurn);

	CRITICAL_SECTION cs;

private:
	Node();
	bool isInConflict();
};