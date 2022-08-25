#pragma once

#include <vector>
#include <string>
#include "../connect4/player.hpp"


/**
 * @brief Holds the data for one position in memory.
 * 
 */
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

/**
 * @brief Holds the info to create a new Agent.
 * 
 */
struct AgentData {
	~AgentData() {}

	std::string name;
	std::string nn_path;
	ePlayer player;
};

/**
 * @brief Keeps history of the training process.
 * 
 */
struct LossHistory {
	~LossHistory() {}
	
	int historySize = 0;
	int batch_size = 0;
	int data_size = 0;
	std::vector<float> losses = std::vector<float>();
	std::vector<float> values  = std::vector<float>();
	std::vector<float> policies = std::vector<float>();
};

/**
 * @brief Tally for counting wins when self-playing.
 * 
 */
struct SelfPlayTally {
	~SelfPlayTally() {}

	int draws = 0;
	int red = 0;
	int yellow = 0;
};