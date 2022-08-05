#pragma once

#include <filesystem>
#include <string>

#include "neuralNetwork/network.hpp"
#include "utils.hpp"


class NeuralNetwork {
  public:
	NeuralNetwork();
	~NeuralNetwork();

	std::tuple<torch::Tensor, torch::Tensor> predict(torch::Tensor &input);

	bool loadModel(std::string modelPath);
	bool saveModel(std::string modelPath, bool isTrained = false);

	Network getNetwork();


  private:
	torch::Device device = torch::Device(torch::kCPU);
	Network net = nullptr;
};