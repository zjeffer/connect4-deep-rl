#include "agent.hpp"

Agent::Agent(std::string name, std::string model_path, std::shared_ptr<SelfPlaySettings> selfPlaySettings)
  : m_Name(name)
  , m_NN(std::make_shared<NeuralNetwork>(selfPlaySettings))
  , m_MCTS(std::make_shared<MCTS>(selfPlaySettings, nullptr, m_NN))
{
    // load the weights
    m_NN->loadModel(model_path);
}

Agent::Agent(std::string name, std::shared_ptr<NeuralNetwork> nn, std::shared_ptr<SelfPlaySettings> selfPlaySettings)
  : m_Name(name)
  , m_NN(nn)
  , m_MCTS(std::make_shared<MCTS>(selfPlaySettings, nullptr, m_NN))
{
}

Agent::~Agent() {}

std::shared_ptr<MCTS> Agent::getMCTS() const
{
    return m_MCTS;
}

std::string const& Agent::getName() const
{
    return m_Name;
}

void Agent::setName(std::string name)
{
    m_Name = name;
}