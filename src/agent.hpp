#pragma once

#include <string>
#include "mcts.hpp"
#include "neuralNetwork.hpp"
#include "tree/node.hpp"
#include "connect4/environment.hpp"
#include "common.hpp"

class Agent {
  public:
	Agent(std::string name, std::string model_path, SelfPlaySettings *selfPlaySettings);
	Agent(std::string name, std::shared_ptr<NeuralNetwork> nn, SelfPlaySettings *selfPlaySettings);
	~Agent();

	[[nodiscard]] MCTS* getMCTS();

	[[nodiscard]] std::string getName();
	void setName(std::string name);

  private:
	std::string m_Name;
	std::shared_ptr<NeuralNetwork> m_NN;
	MCTS *m_MCTS;
};