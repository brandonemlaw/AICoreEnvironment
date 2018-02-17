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
	static void pruneAllAbove(Node* root, Node* nodeToSave);

private:
	Node* nodeToSave;
	Node* root;
	
	Node* getRoot(Node* node);

};