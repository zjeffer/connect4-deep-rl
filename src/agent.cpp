#include "agent.hpp"



Agent::Agent(std::string name, std::string model_path, SelfPlaySettings *selfPlaySettings) {
	m_Name = name;
	m_NN = std::make_shared<NeuralNetwork>(selfPlaySettings);
	m_NN->loadModel(model_path);
	m_MCTS = new MCTS(selfPlaySettings, nullptr, m_NN);
}

Agent::Agent(std::string name, std::shared_ptr<NeuralNetwork> nn, SelfPlaySettings *selfPlaySettings) {
	m_Name = name;
	m_NN = nn;
	m_MCTS = new MCTS(selfPlaySettings, nullptr, m_NN);
}

Agent::~Agent(){
	delete m_MCTS;
}

MCTS* Agent::getMCTS(){
	return m_MCTS;
}

std::string Agent::getName(){
	return m_Name;
}

void Agent::setName(std::string name){
	m_Name = name;
}