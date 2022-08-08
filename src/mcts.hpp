#pragma once

#include "neuralNetwork.hpp"
#include "utils/selfPlaySettings.hpp"
#include "tree/node.hpp"
#include "common.hpp"

class MCTS {
  public:
	MCTS(SelfPlaySettings *selfPlaySettings, Node *root, const std::shared_ptr<NeuralNetwork> &nn);
	~MCTS();

	void run_simulations();

	Node *select(Node *root);

	float expand(Node *root);

	void backpropagate(Node *root, float result);

	Node *getRoot();
	void setRoot(Node *root);

	int getBestMoveDeterministic();
	int getBestMoveStochastic();

	static int getTreeDepth(Node *root);

  private:
	SelfPlaySettings *m_Settings = nullptr;
	Node *m_Root = nullptr;
	std::shared_ptr<NeuralNetwork> m_NN = nullptr;
	torch::Device m_Device = torch::kCPU;
};