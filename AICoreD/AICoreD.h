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



//Weights
double TAKE_PIECE_WEIGHT = 0;
double LOSE_PIECE_WEIGHT = 0;
double DOUBLE_DEFENDED_WEIGHT = 0;
double DEFENDED_WEIGHT = 0;
double BLOCKED_WEIGHT = 0;



//Constants
const unsigned int COLUMNS[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };	//A-H
const int SECONDS_TO_WORK = 4;




Node* root = NULL;


extern "C" __declspec(dllexport) Move __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned int blackRows[], unsigned int whiteRows[], bool isWhitesTurn);

bool executeRandomGame(Board& rawBoard, bool isWhitesTurn);

double evaluate(Node* move, Board boardBeforeMove, bool playingForWhite);

bool defendedLeft(Board board, int row, int col, bool playingForWhite);
bool defendedRight(Board board, int row, int col, bool playingForWhite);
bool blocked(Board board, int row, int col, bool playingForWhite);

void setRoot(Node*& r, Board board, bool isWhitesTurn);
void runMonteCarloAlgorithm(Node* root, Board mboard, bool isWhitesTurn);
void processNode(Node* node, bool isWhitesTurn);
Node* getChildNode(Node* node, int i);
int getNodeChildren(Node* node);

void incrementTarget(short int& target, bool validMoveExists);
bool compareMove(const Move& a, const Move& b);
void backTrackValues(Node* node, bool whiteDidWin);

void expandNode(Node* node);
Node* selectPromisingNode(Node* root);
double getUCTRating(Node* node, bool isWhitesTurnMaster);
