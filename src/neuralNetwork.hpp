#pragma once

#include <filesystem>
#include <string>

#include "neuralNetwork/network.hpp"
#include "connect4/environment.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include "common.hpp"


class NeuralNetwork {
  public:
	NeuralNetwork(Settings* settings);
	~NeuralNetwork();

	torch::Tensor boardToInput(Environment* env);
	std::tuple<torch::Tensor, torch::Tensor> predict(torch::Tensor &input);

	bool loadModel(std::string modelPath);
	bool saveModel(std::string modelPath, bool isTrained = false);

	Network getNetwork();


  private:
	torch::Device m_Device = torch::Device(torch::kCPU);
	Network m_Net = nullptr;
	Settings* m_Settings = nullptr;
};