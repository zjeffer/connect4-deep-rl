#pragma once

#include <filesystem>
#include <string>

#include "neuralNetwork/network.hpp"
#include "connect4/environment.hpp"
#include "utils/selfPlaySettings.hpp"
#include "utils/utils.hpp"
#include "common.hpp"

/**
 * @brief The NeuralNetwork class holds the torch Network to run inference with.
 * It also features some methods to load & save the model, and
 * convert a board to an input state for use with the network.
 *
 */
class NeuralNetwork {
  public:
	NeuralNetwork(Settings* selfPlaySettings);
	~NeuralNetwork();

	/**
	 * @brief Convert the given board to the network's input state.
	 * 
	 * @param board 
	 * @param player 
	 * @param inputPlanes 
	 * @return torch::Tensor 
	 */
	static torch::Tensor boardToInput(torch::Tensor board, int player, int inputPlanes);

	/**
	 * @brief Convert the given environment's board to an input state.
	 * 
	 * @param env 
	 * @return torch::Tensor
	 */
	torch::Tensor boardToInput(const std::shared_ptr<Environment>& env);

	/**
	 * @brief Run inference on the network
	 * 
	 * @param input: the input to give the network
	 * @return std::pair<torch::Tensor, torch::Tensor>: two outputs: policy & value output
	 */
	std::pair<torch::Tensor, torch::Tensor> predict(torch::Tensor& input);

	/**
	 * @brief Load the weights into the network.
	 * 
	 * @param modelPath: path to the saved weights
	 * @return true if successful 
	 */
	bool loadModel(std::string modelPath);

	/**
	 * @brief Save the model's weights to a file
	 * 
	 * @param modelPath: the path to save the weights to.
	 * @return std::filesystem::path: the resulting path
	 */
	std::filesystem::path saveModel(std::filesystem::path modelPath);

	/**
	 * @brief Get the Network
	 * 
	 * @return Network 
	 */
	Network getNetwork() const;

  private:
	torch::Device m_Device = torch::Device(torch::kCPU);
	Network m_Net = nullptr;
	Settings* m_Settings = nullptr;
};