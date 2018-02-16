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


//TODO
// -abort if memory pressure
// -counteract their L formation when advancing


//Weights
double MASTER_WEIGHT = 1;
double TAKE_PIECE_WEIGHT = 1.1; //1.05
double LOSE_PIECE_WEIGHT = 1;

double DOUBLE_BACKUP_WEIGHT = 1.1;
double BACKUP_WEIGHT = 1.05;

double SIDE_WEIGHT = 1.05;

double DOUBLE_DEFENDED_WEIGHT = 0.8;
double DEFENDED_WEIGHT = 0.9;
double BLOCKED_WEIGHT = 1;



//Constants
const unsigned int COLUMNS[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };	//A-H
const int SECONDS_TO_WORK = 2;




Node* root = NULL;


extern "C" __declspec(dllexport) Move __stdcall  AIGetMove(int blackCount, int whiteCount, unsigned int blackRows[], unsigned int whiteRows[], bool isWhitesTurn);

bool executeRandomGame(Board& rawBoard, bool isWhitesTurn);

double evaluate(Node* move, Board boardBeforeMove, bool playingForWhite);


bool backupLeft(Board board, int row, int col, bool playingForWhite);
bool backupRight(Board board, int row, int col, bool playingForWhite);
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
