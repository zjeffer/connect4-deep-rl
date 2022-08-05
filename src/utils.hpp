#pragma once

#include <torch/torch.h>
#include <string>
#include <time.h>

#include "types.hpp"

namespace utils {
	std::string getTimeString() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", timeinfo);
		return std::string(buffer);
	}

	torch::Tensor boardToTensor(Connect4Board board){
		return torch::from_blob(board.board, {board.rows, board.cols});
	}

	Connect4Board tensorToBoard(torch::Tensor tensor){
		Connect4Board board;
		board.rows = tensor.size(0);
		board.cols = tensor.size(1);
		// create int array of size rows * cols
		board.board = std::vector<int>(tensor.data_ptr<int>(), tensor.data_ptr<int>() + tensor.numel());
		return board;
	}
}