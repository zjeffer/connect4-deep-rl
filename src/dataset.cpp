#include "dataset.hpp"

C4Dataset::C4Dataset(TrainerSettings* settings) {
	m_Settings = settings;
	// load data
	if(!loadData(m_Settings->getMemoryFolder())){
		// LOG(FATAL) << "Failed to load data from " << m_Settings->getMemoryFolder();
		// exit(EXIT_FAILURE);
		LOG(WARNING) << "Failed to load data";
	}
}

bool C4Dataset::loadData(std::string path) {
	try {
		for (auto& file : std::filesystem::directory_iterator(path)) {
			if (file.path().extension() == ".bin") {
				std::ifstream input(file.path(), std::ios::in | std::ios::binary);
				if(!input.is_open()){
					LOG(FATAL) << "Failed to open " << file.path();
					exit(EXIT_FAILURE);
				}
				while(!input.eof()){
					MemoryElement element;
					input.read((char*)&element, sizeof(MemoryElement));

					auto opts = torch::TensorOptions().dtype(torch::kInt8);
					torch::Tensor board = torch::from_blob(element.board.data(), {m_Settings->getRows(), m_Settings->getCols()}, opts);

					// create input tensor from board
					torch::Tensor inputTensor = NeuralNetwork::boardToInput(board, element.player, m_Settings->getInputPlanes());

					// create output tensor from winner
					LOG(DEBUG) << "Creating output tensor from winner";
					torch::Tensor outputTensor = torch::full({m_Settings->getRows(), m_Settings->getCols()}, element.winner);

					LOG(DEBUG) << "Adding input and output tensor to dataset";
					Data d;
					d.first = inputTensor;
					d.second = outputTensor;
					
					m_Data.push_back(d);
				}
				input.close();
			}
		}
	} catch (std::exception& e) {
		return false;
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
