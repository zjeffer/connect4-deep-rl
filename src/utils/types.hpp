#pragma once

#include <vector>


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