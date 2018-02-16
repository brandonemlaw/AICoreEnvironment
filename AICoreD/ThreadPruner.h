#pragma once

#include "Node.h"
#include "CriticalSectionLock.h"
#include <queue>
#include <thread>

class ThreadPruner
{
public:
	ThreadPruner(Node* save, Node* newRoot);
	~ThreadPruner();

private:
	Node* nodeToSave;
	Node* root;
	
	static void pruneAllAbove(Node* root, Node* nodeToSave);
	Node* getRoot(Node* node);

};