#include "agent.hpp"

Agent::Agent(std::string name, std::string model_path, std::shared_ptr<Settings> settings)
  : m_Name(name)
  , m_NN(std::make_shared<NeuralNetwork>(settings))
  , m_MCTS(std::make_shared<MCTS>(settings, nullptr, m_NN))
{
    LDEBUG << "Creating agent '" << m_Name << "'";

    // load the weights
    m_NN->loadModel(model_path);
    if (settings->useCUDA()) {
        m_NN->getNetwork()->to(torch::kCUDA);
    }
}

Agent::Agent(std::string name, std::shared_ptr<NeuralNetwork> nn, std::shared_ptr<Settings> settings)
  : m_Name(name)
  , m_NN(nn)
  , m_MCTS(std::make_shared<MCTS>(settings, nullptr, m_NN))
{
    LDEBUG << "Creating agent '" << m_Name << "'";
}

Agent::~Agent() {
    LDEBUG << "Destroying agent " << m_Name;
}

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

std::shared_ptr<NeuralNetwork> Agent::getModel() const
{
    return m_NN;
}