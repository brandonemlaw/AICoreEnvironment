#pragma once
#include "stdafx.h"
#include <random>
#include <vector>
#include <algorithm>
#include <float.h>
#include <ctime>
#include <thread>

#include "Node.h"
#include "CriticalSectionLock.h"
#include "ThreadPruner.h"
#include "Evaluator.h"


//TODO
// -abort if memory pressure
// -counteract their L formation when advancing





//Constants
const unsigned int COLUMNS[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };	//A-H
const double SECONDS_TO_WORK = 4.5;




Node* root = NULL;


extern "C" __declspec(dllexport) Move __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned int blackRows[], unsigned int whiteRows[], bool isWhitesTurn);

bool executeRandomGame(Board& rawBoard, bool isWhitesTurn);


void setRoot(Node*& r, Board board, bool isWhitesTurn);
void runMonteCarloAlgorithm(Node* root, Board mboard, bool isWhitesTurn);
void processNode(Node* node, bool isWhitesTurn);
Node* getChildNode(Node* node, int i);
int getNodeChildren(Node* node);

void incrementTarget(int& target, bool validMoveExists);
bool compareMove(const Move& a, const Move& b);
void backTrackValues(Node* node, bool whiteDidWin);

void expandNode(Node* node);
Node* selectPromisingNode(Node* root);
double getUCTRating(Node* node, bool isWhitesTurnMaster);
