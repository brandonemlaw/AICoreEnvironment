
#include "ThreadPruner.h"





Node* ThreadPruner::getRoot(Node* node)
{
	if (node->parent != NULL)
	{
		return getRoot(node->parent);
	}
	else
	{
		return node;
	}
}

ThreadPruner::ThreadPruner(Node* save, Node* newRoot)
{
	//set the node to save
	nodeToSave = save;

	//find the root node
	root = newRoot;
}

ThreadPruner::~ThreadPruner()
{
	std::thread pruner(&ThreadPruner::pruneAllAbove, root, nodeToSave);
	pruner.detach();
}

void ThreadPruner::pruneAllAbove(Node* root, Node* nodeToSave)
{
	if (root != NULL)
	{
		//create a queue of nodes to delete, starting with the root
		std::queue<Node*> nodes;
		nodes.push(root);

		//until we are out of nodes to delete
		while (nodes.size() > 0)
		{
			//get the first node from the queue
			Node* node = nodes.front();
			nodes.pop();

			if (nodes.size() == 0)
			{
				int b = 1;
			}

			//delete only if node if not the avoid node
			if (node != nodeToSave /*&& node != NULL*/)
			{

				//add all the children to the queue
				for (int i = 0; i < node->children.size(); i++)
				{
					nodes.push(node->children[i]);
				}

				//actually delete the node - but let the lock go out of scope first
				delete node;
			}
		}
	}

}