#pragma once

#include <torch/torch.h>
#include <filesystem>

#include "utils/trainerSettings.hpp"
#include "utils/types.hpp"
#include "neuralNetwork.hpp"
#include "common.hpp"

using Data = std::pair<torch::Tensor, torch::Tensor>;

class C4Dataset : public torch::data::datasets::Dataset<C4Dataset> {
  public:
	explicit C4Dataset(TrainerSettings* settings);

	bool loadData(std::string folder);
	
	torch::data::Example<> get(size_t index) override;

	torch::optional<size_t> size() const override;

  private:
	std::vector<Data> m_Data;
	TrainerSettings* m_Settings;

};