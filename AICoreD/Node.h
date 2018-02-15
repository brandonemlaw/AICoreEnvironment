#pragma once

#include "State.h"
#include <vector>
#include <math.h>
#include <Windows.h>

class Node
{
public:
	State state;
	Node* parent;
	std::vector<Node*> children;

	Node::Node(Node* myParent, Board myBoard, Move myMove, bool myIsWhitesTurn);
	~Node();


	static Node* rootNode(Board newBoard, bool isWhitesTurn);

	CRITICAL_SECTION cs;

private:
	Node();
};