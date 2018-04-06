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


//TODO
// -abort if memory pressure


Node* root = NULL;


extern "C" __declspec(dllexport) void __stdcall  EmptyMemory();
extern "C" __declspec(dllexport) SubmitMove __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned long long black, unsigned long long white, bool isWhitesTurn, unsigned int mode);

bool executeRandomGame(Board& rawBoard, bool isWhitesTurn);

unsigned long long int reverse(unsigned long long int num);

//Deep alpha beta
Node* chooseWithDeepSearch(Node* root, int depth);
int deepSearch(Node* root, int depth);

Node* seedWithAlphaBeta(Node* root, bool isWhitesTurn);
int alphaBeta(Node* node, int depth, int alpha, int beta, bool maximizingPlayer);
bool compareABPairs(std::tuple<Node*, int>& first, std::tuple<Node*, int>& second);
int abEval(Node* node);


void setRoot(Node*& r, Board board, bool isWhitesTurn);
void runMonteCarloAlgorithm(Node* root, Board mboard, bool isWhitesTurn, std::time_t endTime);
void processNode(Node* node, bool isWhitesTurn);
Node* getChildNode(Node* node, int i);
int getNodeChildren(Node* node);

void incrementTarget(int& target, bool validMoveExists);
bool compareMove(const Move& a, const Move& b);
void backTrackValues(Node* node, bool didWeWin);

void expandNode(Node* node);
Node* selectPromisingNode(Node* root);
double getUCTRating(Node* node, bool isWhitesTurnMaster);
