#pragma once

#include <cstdint>
#include <vector>


struct MemoryElement {
	std::vector<uint8_t> board;
	uint8_t currentPlayer;
	std::vector<float> moveList;
	uint8_t winner;
};