#pragma once
#include "SubmitMove.h"
struct Move
{
	int value;
	unsigned int row;
	unsigned int col;
	unsigned int target;

	Move()
	{
		value = 0;
		row = 0;
		col = 0;
		target = 0;
	}

	Move(int myRow, int myCol, int myTarget)
	{
		value = 0;
		row = myRow;
		col = myCol;
		target = myTarget;
	}

	SubmitMove toSubmitMove()
	{
		SubmitMove result;
		result.target = target;
		result.col = col;
		result.row = row;
		result.value = value;

		return result;
	}
};
