#include "utils.hpp"

std::string utils::getTimeString() {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", timeinfo);
	return std::string(buffer);
}

std::vector<int> utils::boardToVector(torch::Tensor tensor) {
	std::vector<int> board;
	for (int i = 0; i < tensor.size(0); i++) {
		for (int j = 0; j < tensor.size(1); j++) {
			board.push_back(tensor[i][j].item().to<int>());
		}
	}
	return board;
}

torch::Tensor utils::boardToTensor(Connect4Board board){
	return torch::from_blob(board.board.data(), {board.rows, board.cols});
}

Connect4Board utils::tensorToBoard(torch::Tensor tensor){
	Connect4Board board;
	board.rows = tensor.size(0);
	board.cols = tensor.size(1);
	// create int array of size rows * cols
	board.board = std::vector<int>(tensor.data_ptr<int>(), tensor.data_ptr<int>() + tensor.numel());
	return board;
}