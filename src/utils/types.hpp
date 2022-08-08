#pragma once

#include <vector>


struct Connect4Board {
	int rows;
	int cols;
	std::vector<int> board;
};

struct MoveProb {
	int move;
	float prob;
};

struct MemoryElement {
	std::vector<int> board;
	int player;
	std::vector<MoveProb> moves;
	int winner;
};