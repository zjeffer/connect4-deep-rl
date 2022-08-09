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
	NeuralNetwork(SelfPlaySettings* selfPlaySettings);
	~NeuralNetwork();

	static torch::Tensor boardToInput(torch::Tensor board, int player, int inputPlanes);
	torch::Tensor boardToInput(Environment* env);
	std::tuple<torch::Tensor, torch::Tensor> predict(torch::Tensor &input);

	bool loadModel(std::string modelPath);
	bool saveModel(std::string modelPath, bool isTrained = false);

	Network getNetwork();


  private:
	torch::Device m_Device = torch::Device(torch::kCPU);
	Network m_Net = nullptr;
	SelfPlaySettings* m_Settings = nullptr;
};