#pragma once

#include <torch/torch.h>

#include "utils/trainerSettings.hpp"
#include "common.hpp"
#include "utils/types.hpp"
#include <filesystem>

using Data = std::pair<torch::Tensor, torch::Tensor>;

class Dataset : public torch::data::datasets::Dataset<Data> {
  public:
	explicit Dataset(TrainerSettings* settings);

	bool loadData(std::string path);
	
	torch::data::Example<> get(size_t index) override;

	torch::optional<size_t> size() const override;

  private:
	std::vector<Data> m_Data;
	TrainerSettings* m_Settings;

};