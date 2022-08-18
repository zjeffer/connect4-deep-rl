#pragma once

#include "dataset.hpp"
#include "neuralNetwork.hpp"
#include "utils/trainerSettings.hpp"

using DataLoader = torch::data::StatelessDataLoader<
    torch::data::datasets::MapDataset<
        C4Dataset, torch::data::transforms::Stack<
                     torch::data::Example<at::Tensor, at::Tensor>>>,
    torch::data::samplers::RandomSampler>;

class Trainer {
public:
  Trainer(TrainerSettings *settings);
  ~Trainer();

  std::filesystem::path train();

private:
  std::shared_ptr<NeuralNetwork> m_NN;
  TrainerSettings *m_Settings;
  torch::Device m_Device = torch::Device(torch::kCPU);
};