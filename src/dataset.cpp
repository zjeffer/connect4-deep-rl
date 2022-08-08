#include "dataset.hpp"


Dataset::Dataset(TrainerSettings* settings) {
	m_Settings = settings;
	// load data
	if(!loadData(m_Settings->getMemoryFolder())){
		LOG(FATAL) << "Failed to load data from " << m_Settings->getMemoryFolder();
		exit(EXIT_FAILURE);
	}
}

bool Dataset::loadData(std::string path) {
	// TODO: load data from folder
	try {
		for (auto& file : std::filesystem::directory_iterator(path)) {
			if (file.path().extension() == ".bin") {
				std::ifstream input(file.path(), std::ios::binary);
				if(!input.is_open()){
					LOG(FATAL) << "Failed to open " << file.path();
					exit(EXIT_FAILURE);
				}
				while(!input.eof()){
					MemoryElement element;
					input.read((char*)&element, sizeof(MemoryElement));
					
					Data d;
					// TODO: load data
					

				}
				input.close();
			}
		}
	} catch (std::exception& e) {
		return false;
	}
	return true;
}

torch::data::Example<> Dataset::get(size_t index) {
	torch::Tensor input = m_Data[index].first;
    torch::Tensor output = m_Data[index].second;
    return {input, output};
}



torch::optional<size_t> Dataset::size() const {
    return m_Data.size();
}
