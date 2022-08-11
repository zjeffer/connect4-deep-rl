#include "utils.hpp"
#include "types.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>

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

std::vector<uint8_t> boardToVector(torch::Tensor tensor) {
	std::vector<uint8_t> board;
	for (int i = 0; i < tensor.size(0); i++) {
		for (int j = 0; j < tensor.size(1); j++) {
			board.push_back(tensor[i][j].item().to<uint8_t>());
		}
	}
	return board;
}

torch::Tensor moveListToOutputs(const std::vector<float>& moveList, const float& value) {
	// concat all move probabilities into one tensor, and add the value to the end
	torch::Tensor outputs = torch::zeros({ (int)moveList.size() + 1 });
	for (int i = 0; i < (int)moveList.size(); i++) {
		outputs[i] = torch::tensor(moveList[i]);
	}
	outputs[moveList.size()] = torch::tensor(value);
	return outputs;
}


void writeIntVector(std::vector<uint8_t>& vector, std::ofstream& file) {
	typename std::vector<uint8_t>::size_type size = vector.size();
	file.write((char*)&size, sizeof(size));
	file.write((char*)vector.data(), sizeof(uint8_t) * size);
}

void writeFloatVector(std::vector<float>& vector, std::ofstream& file) {
	typename std::vector<float>::size_type size = vector.size();
	file.write((char*)&size, sizeof(size));
	file.write((char*)vector.data(), sizeof(float) * size);
}

void readIntVector(std::vector<uint8_t>& vector, std::ifstream& file) {
	typename std::vector<uint8_t>::size_type size = vector.size();
	file.read((char*)&size, sizeof(size));
	vector.resize(size);
	file.read((char*)vector.data(), sizeof(uint8_t) * size);
}

void readFloatVector(std::vector<float>& vector, std::ifstream& file) {
	typename std::vector<float>::size_type size = vector.size();
	file.read((char*)&size, sizeof(size));
	vector.resize(size);
	file.read((char*)vector.data(), sizeof(float) * size);
}

bool writeMemoryElementsToFile(std::vector<MemoryElement>& elements, const std::filesystem::path& filepath) {
	std::ofstream file(filepath, std::ios::out | std::ios::binary);
	if (file.is_open()) {
		for (MemoryElement &element : elements) {
			writeIntVector(element.board, file);
			file.write((char*)&element.currentPlayer, sizeof(element.currentPlayer));			
			writeFloatVector(element.moveList, file);
			file.write((char*)&element.winner, sizeof(element.winner));
		}
		file.close();
		return true;
	}
	return false;
}

bool readMemoryElementsFromFile(std::vector<MemoryElement> &elements, const std::filesystem::path &filepath) {
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	int i = 0;
	if (file.is_open()) {
		while(!file.eof()){
			MemoryElement element;
			readIntVector(element.board, file);
			file.read((char*)&element.currentPlayer, sizeof(element.currentPlayer));
			readFloatVector(element.moveList, file);
			file.read((char*)&element.winner, sizeof(element.winner));

			if (element.board.size() == 0) {
				LOG(WARNING) << "Empty memory element found at index " << i;
			}

			elements.push_back(element);
			i++;
		}
		return true;
	}
	return false;
}

} // namespace utils



