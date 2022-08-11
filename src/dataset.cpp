#include "dataset.hpp"
#include "utils/types.hpp"
#include "utils/utils.hpp"
#include <ATen/ops/tensor.h>

C4Dataset::C4Dataset(TrainerSettings* settings) {
	m_Settings = settings;
	// load data
	if(!loadData(m_Settings->getMemoryFolder())){
		LOG(FATAL) << "Failed to load data from " << m_Settings->getMemoryFolder();
		exit(EXIT_FAILURE);
	}
}

bool C4Dataset::loadData(std::string folder) {
	// read data from the given folder into the m_Data vector
	for (auto& file : std::filesystem::directory_iterator(folder)) {
		if (file.path().extension() == ".bin") {
			std::vector<MemoryElement> elements;
			if (!utils::readMemoryElementsFromFile(elements, file.path())){
				LOG(FATAL) << "Failed to read memory elements from " << file.path();
				exit(EXIT_FAILURE);
			}
			if (elements.size() == 0) {
				LOG(FATAL) << "No memory elements in " << file.path();
				exit(EXIT_FAILURE);
			}
			// convert MemoryElements to Data
			auto opts = torch::TensorOptions().dtype(torch::kUInt8);
			for (auto& element : elements) {
				if (element.board.size() == 0){
					LOG(WARNING) << "Empty board in " << file.path();
					// TODO: every file contains 1 empty board?
					continue;
				}
				torch::Tensor board = torch::from_blob(element.board.data(), {m_Settings->getRows(), m_Settings->getCols()}, opts);
				
				torch::Tensor input = NeuralNetwork::boardToInput(board, element.currentPlayer, m_Settings->getInputPlanes()).squeeze();
				torch::Tensor output = utils::moveListToOutputs(element.moveList, element.winner);


				m_Data.push_back(std::make_pair(input, output));
			}
		} else {
			LOG(WARNING) << "Ignoring file: " << file.path();
		}
	}
	return true;
}

torch::data::Example<> C4Dataset::get(size_t index) {
	torch::Tensor input = m_Data[index].first;
    torch::Tensor output = m_Data[index].second;
    return {input, output};
}



torch::optional<size_t> C4Dataset::size() const {
    return m_Data.size();
}
