#pragma once

#include <filesystem>
#include <torch/torch.h>
#include <string>
#include <time.h>

#include "types.hpp"

namespace utils {
	std::string getTimeString();

	std::vector<uint8_t> boardToVector(torch::Tensor tensor);

	torch::Tensor moveListToOutputs(const std::vector<float>& moveList, const float& value);

	bool writeMemoryElementsToFile(std::vector<MemoryElement>& elements, const std::filesystem::path& filepath);
	bool readMemoryElementsFromFile(std::vector<MemoryElement>& elements, const std::filesystem::path& filepath);

	void writeIntVector(std::vector<uint8_t>& vector, std::ofstream& file);
	void writeFloatVector(std::vector<float>& vector, std::ofstream& file);
	void readIntVector(std::vector<uint8_t>& vector, std::ifstream& file);
	void readFloatVector(std::vector<float>& vector, std::ifstream& file);

}