#pragma once

#include <vector>
#include <string>
#include "../connect4/player.hpp"



struct MemoryElement {
	MemoryElement() {}
	MemoryElement(const MemoryElement& other) {
		board = other.board;
		currentPlayer = other.currentPlayer;
		winner = other.winner;
		moveList = other.moveList;
	};
	~MemoryElement() {}

	std::vector<uint8_t> board;
	uint8_t currentPlayer;
	std::vector<float> moveList;
	uint8_t winner;
};

struct AgentData {
	~AgentData() {}

	std::string name;
	std::string nn_path;
	ePlayer player;
};

struct LossHistory {
	~LossHistory() {}
	
	int historySize = 0;
	int batch_size = 0;
	int data_size = 0;
	std::vector<float> losses = std::vector<float>();
	std::vector<float> values  = std::vector<float>();
	std::vector<float> policies = std::vector<float>();
};

struct SelfPlayTally {
	~SelfPlayTally() {}

	int draws = 0;
	int red = 0;
	int yellow = 0;
};