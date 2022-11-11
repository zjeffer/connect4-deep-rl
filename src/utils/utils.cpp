#include "utils.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>

#include "types.hpp"

namespace utils
{

std::string getTimeString()
{
    time_t     rawtime;
    struct tm* timeinfo;
    char       buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", timeinfo);
    return std::string(buffer);
}

std::vector<uint8_t> boardToVector(torch::Tensor tensor)
{
    std::vector<uint8_t> board;
    for (int i = 0; i < tensor.size(0); i++)
    {
        for (int j = 0; j < tensor.size(1); j++)
        {
            board.push_back(tensor[i][j].item().to<uint8_t>());
        }
    }
    return board;
}

torch::Tensor moveListToOutputs(std::vector<float> const& moveList, float const& value)
{
    // concat all move probabilities into one tensor, and add the value to the end
    torch::Tensor outputs = torch::zeros({(int)moveList.size() + 1});
    for (int i = 0; i < (int)moveList.size(); i++)
    {
        outputs[i] = torch::tensor(moveList[i]);
    }
    outputs[moveList.size()] = torch::tensor(value);
    return outputs;
}

template<typename T>
void writeVectorToFile(std::vector<T>& vector, std::ofstream& file)
{
    typename std::vector<T>::size_type size = vector.size();
    file.write((char*)&size, sizeof(size));
    file.write((char*)vector.data(), sizeof(T) * size);
}

template<typename T>
void readVectorFromFile(std::vector<T>& vector, std::ifstream& file)
{
    typename std::vector<T>::size_type size = vector.size();
    file.read((char*)&size, sizeof(size));
    vector.resize(size);
    file.read((char*)vector.data(), sizeof(T) * size);
}

bool writeMemoryElementsToFile(std::vector<MemoryElement>& elements, std::filesystem::path const& filepath)
{
    std::ofstream file(filepath, std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        for (MemoryElement& element: elements)
        {
            writeVectorToFile<uint8_t>(element.board, file);
            file.write((char*)&element.currentPlayer, sizeof(element.currentPlayer));
            writeVectorToFile<float>(element.moveList, file);
            file.write((char*)&element.winner, sizeof(element.winner));
        }
        file.close();
        return true;
    }
    return false;
}

bool readMemoryElementsFromFile(std::vector<MemoryElement>& elements, std::filesystem::path const& filepath)
{
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    int           i = 0;
    if (file.is_open())
    {
        while (!file.eof())
        {
            MemoryElement element;
            readVectorFromFile<uint8_t>(element.board, file);
            file.read((char*)&element.currentPlayer, sizeof(element.currentPlayer));
            readVectorFromFile<float>(element.moveList, file);
            file.read((char*)&element.winner, sizeof(element.winner));

            elements.push_back(element);
            i++;
        }
        return true;
    }
    return false;
}

std::string getDirectoryFromFilename(std::string filename)
{
    if (filename.find_last_of("/") == std::string::npos)
    {
        return "";
    }
    return filename.substr(0, filename.find_last_of("/"));
}

void writeLossToCSV(std::string filename, LossHistory& lossHistory)
{
    if (!filename.ends_with((".csv")))
    {
        filename += ".csv";
    }
    // check if filename is in subdirectory
    std::string dir;
    if (!(dir = utils::getDirectoryFromFilename(filename)).empty())
    {
        // if it is, create the directory to be sure
        std::filesystem::create_directories(dir);
    }
    LOG(INFO) << "Saving loss history to " << filename;
    std::ofstream file;
    file.open(filename);
    // header
    file << "Epoch;Policy Loss;Value Loss;Total Loss" << std::endl;
    // data
    for (int i = 0; i < lossHistory.historySize; i++)
    {
        file << i << ";" << lossHistory.policies[i] << ";" << lossHistory.values[i] << ";" << lossHistory.losses[i] << "\n";
    }
    file << "\n";
    file.close();
}

} // namespace utils
