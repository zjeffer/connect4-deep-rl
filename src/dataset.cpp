#include "dataset.hpp"

#include "utils/types.hpp"
#include "utils/utils.hpp"
#include <ATen/ops/tensor.h>

C4Dataset::C4Dataset(TrainerSettings* settings)
  : m_Settings(settings)
{
    // load data from the memory folder
    if (!loadData(m_Settings->getMemoryFolder()))
    {
        LFATAL << "Failed to load data from " << m_Settings->getMemoryFolder();
    }
}

bool C4Dataset::loadData(std::string folder)
{
    int rows        = m_Settings->getRows();
    int cols        = m_Settings->getCols();
    int inputPlanes = m_Settings->getInputPlanes();
    // read data from the given folder into the m_Data vector
    for (auto& file: std::filesystem::directory_iterator(folder))
    {
        if (file.path().extension() == ".bin")
        {
            std::vector<MemoryElement> elements;
            if (!utils::readMemoryElementsFromFile(elements, file.path()))
            {
                LFATAL << "Failed to read memory elements from " << file.path();
            }
            if (elements.size() == 0)
            {
                LFATAL << "No memory elements in " << file.path();
            }
            // convert MemoryElements to Data
            auto opts = torch::TensorOptions().dtype(torch::kUInt8);
            for (auto& element: elements)
            {
                if (element.board.size() == 0)
                {
                    LWARN << "Empty board in " << file.path();
                    LDEBUG << "board: " << element.board;
                    LDEBUG << "currentPlayer: " << element.currentPlayer;
                    LDEBUG << "moveList: " << element.moveList;
                    LDEBUG << "winner: " << element.winner;
                    exit(1);
                }
                // load the board from the int vector
                torch::Tensor board = torch::from_blob(element.board.data(), {rows, cols}, opts);

                // convert the board to an input for the neural network
                ePlayer       player = element.currentPlayer == 1 ? ePlayer::YELLOW : element.currentPlayer == 2 ? ePlayer::RED : ePlayer::NONE;
                torch::Tensor input  = NeuralNetwork::boardToInput(board, player, inputPlanes).squeeze();
                // convert the list of all moves and the final winner to an output for the neural network
                torch::Tensor output = utils::moveListToOutputs(element.moveList, element.winner);

                // add the input & output to the dataset
                m_Data.push_back(std::make_pair(input, output));
            }
        }
    }
    return true;
}

torch::data::Example<> C4Dataset::get(size_t index)
{
    return {m_Data[index].first, m_Data[index].second};
}

torch::optional<size_t> C4Dataset::size() const
{
    return m_Data.size();
}
