#pragma once
#include "stdafx.h"
#include <random>
#include <vector>
#include <algorithm>
#include <float.h>
#include <ctime>
#include <thread>
#include <fstream>
#include <iomanip>
#include <vector>
#include <tuple>
#include <algorithm>

#include "Node.h"
#include "CriticalSectionLock.h"
#include "ThreadPruner.h"
#include "Evaluator.h"



//Persistent root node for the tree
Node* root = NULL;

//Externals
extern "C" __declspec(dllexport) void __stdcall  EmptyMemory();
extern "C" __declspec(dllexport) SubmitMove __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned long long black, unsigned long long white, bool isWhitesTurn, unsigned int mode);

//Primary Subroutines
void runMonteCarloAlgorithm(Node* root, bool isWhitesTurn, std::time_t endTime);
void seedWithAlphaBeta(Node* root, bool isWhitesTurn);

//Deep Search Components
void removeExtraNodes(std::vector<std::tuple <Node*, int>> values);
int deepSearch(Node* root, int depth);
int alphaBeta(Node* node, int depth, int alpha, int beta, bool maximizingPlayer);
bool compareABPairs(std::tuple<Node*, int>& first, std::tuple<Node*, int>& second);
int abEval(Node* node);

//Monte Carlo Components
Node* selectPromisingNode(Node* root);
void expandNode(Node* node);
void processNode(Node* node, bool isWhitesTurn);
bool executeRandomGame(Board& rawBoard, bool isWhitesTurn);
void backTrackValues(Node* node, bool didWeWin);
double getUCTRating(Node* node, bool isWhitesTurnMaster);

//Other/Utility Components
int getNodeChildren(Node* node);
Node* getChildNode(Node* node, int i);

void setRoot(Node*& r, Board board, bool isWhitesTurn);
bool compareMove(const Move& a, const Move& b);

unsigned long long int reverse(unsigned long long int num);




