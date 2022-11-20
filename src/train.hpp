#pragma once

#include "dataset.hpp"
#include "neuralNetwork.hpp"
#include "utils/trainerSettings.hpp"

using DataLoader =
  torch::data::StatelessDataLoader<torch::data::datasets::MapDataset<C4Dataset, torch::data::transforms::Stack<torch::data::Example<at::Tensor, at::Tensor>>>,
                                   torch::data::samplers::RandomSampler>;

/**
 * @brief The Trainer serves as a way to train the network based on given settings.
 *
 */
class Trainer
{
  public:
    /**
     * @brief Construct a new Trainer
     *
     * @param settings
     */
    Trainer(std::shared_ptr<TrainerSettings> settings);

    /**
     * @brief Destroy the Trainer object
     *
     */
    ~Trainer();

    /**
     * @brief Train the model.
     *
     * @return std::filesystem::path: the path of the new trained model
     */
    std::filesystem::path train();

  private:
    std::shared_ptr<NeuralNetwork>   m_NN;
    std::shared_ptr<TrainerSettings> m_Settings;
    torch::Device                    m_Device = torch::Device(torch::kCPU);
};