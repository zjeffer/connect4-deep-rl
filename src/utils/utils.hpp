#pragma once

#include <time.h>
#include <torch/torch.h>

#include <filesystem>
#include <string>

#include "../common.hpp"
#include "types.hpp"

namespace utils
{

/**
 * @brief Get the current time in a string object
 *
 * @return std::string
 */
std::string getTimeString();

/**
 * @brief Convert the given board to a single vector of ints
 *
 * @param tensor: the board
 * @return std::vector<uint8_t>: a single list of every row in the board
 */
std::vector<uint8_t> boardToVector(torch::Tensor tensor);

/**
 * @brief Convert the output of the network to a single tensor
 *
 * @param moveList: the list of moves (policy output)
 * @param value: the value of the position (value output
 * @return torch::Tensor
 */
torch::Tensor moveListToOutputs(std::vector<float> const& moveList, float const& value);

template<typename T>
void writeVectorToFile(std::vector<T>& vector, std::ofstream& file);

template<typename T>
void readVectorFromFile(std::vector<T>& vector, std::ifstream& file);

bool writeMemoryElementsToFile(std::vector<MemoryElement>& elements, std::filesystem::path const& filepath);
bool readMemoryElementsFromFile(std::vector<MemoryElement>& elements, std::filesystem::path const& filepath);

/**
 * @brief Extract the directory from the given filename
 *
 * @param filename
 * @return std::string: the directory
 */
std::string getDirectoryFromFilename(std::string filename);

/**
 * @brief Write the loss history to a csv file
 *
 * @param filename
 * @param lossHistory
 */
void writeLossToCSV(std::string filename, LossHistory& lossHistory);

} // namespace utils