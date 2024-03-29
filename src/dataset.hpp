#pragma once

#include <filesystem>

#include "common.hpp"
#include "neuralNetwork.hpp"
#include "utils/types.hpp"

// represent one sample of data
using Data = std::pair<torch::Tensor, torch::Tensor>;

/**
 * @brief The dataset to load Connect4 games into
 *
 */
class C4Dataset : public torch::data::datasets::Dataset<C4Dataset>
{
  public:
    /**
     * @brief Construct a new dataset
     *
     * @param settings
     */
    explicit C4Dataset(Settings* settings);

    /**
     * @brief Load data into the dataset from a given folder
     *
     * @param folder: the folder where the .bin files are located
     * @return true on success
     */
    bool loadData(std::filesystem::path folder);

    /**
     * @brief Override the torch getter to get a single data sample
     *
     * @param index: where the data is located
     * @return torch::data::Example<>
     */
    torch::data::Example<> get(size_t index) override;

    /**
     * @brief Size of the dataset
     *
     * @return torch::optional<size_t>
     */
    torch::optional<size_t> size() const override;

  private:
    std::vector<Data>                m_Data;
    Settings* m_Settings;
};