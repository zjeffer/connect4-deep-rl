#include "agent.hpp"



Agent::Agent(std::string name, std::string model_path, Settings *settings) {
	m_Name = name;
	m_NN = std::make_shared<NeuralNetwork>(settings);
	m_NN->loadModel(model_path);
	m_MCTS = new MCTS(settings, nullptr, m_NN);
}

Agent::~Agent(){
	delete m_MCTS;
}

MCTS* Agent::getMCTS(){
	return m_MCTS;
}