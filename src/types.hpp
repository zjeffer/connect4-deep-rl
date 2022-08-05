#pragma once

#include <vector>


struct Connect4Board {
	int rows;
	int cols;
	std::vector<int> board;
};