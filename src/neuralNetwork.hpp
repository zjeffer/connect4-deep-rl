#pragma once

#include <filesystem>
#include <string>

#include "neuralNetwork/network.hpp"
#include "connect4/environment.hpp"
#include "utils/selfPlaySettings.hpp"
#include "utils/utils.hpp"
#include "common.hpp"


class NeuralNetwork {
  public:
	NeuralNetwork(Settings* selfPlaySettings);
	~NeuralNetwork();

	static torch::Tensor boardToInput(torch::Tensor board, int player, int inputPlanes);
	torch::Tensor boardToInput(const std::shared_ptr<Environment>& env);
	std::tuple<torch::Tensor, torch::Tensor> predict(torch::Tensor &input);

	bool loadModel(std::string modelPath);
	std::filesystem::path saveModel(std::filesystem::path modelPath);

	Network getNetwork() const;


  private:
	torch::Device m_Device = torch::Device(torch::kCPU);
	Network m_Net = nullptr;
	Settings* m_Settings = nullptr;
};