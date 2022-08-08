#include "selfPlaySettings.hpp"

SelfPlaySettings::SelfPlaySettings() {}

SelfPlaySettings::SelfPlaySettings(const std::filesystem::path &settings_path) {
  // TODO: load selfPlaySettings from file
}

SelfPlaySettings::~SelfPlaySettings() {}

int SelfPlaySettings::getSimulations() { return m_Simulations; }

void SelfPlaySettings::setSimulations(int simulations) { m_Simulations = simulations; }

bool SelfPlaySettings::isStochastic() { return m_UseStochasticSearch; }

void SelfPlaySettings::setStochastic(bool stochastic) {
  m_UseStochasticSearch = stochastic;
}

void SelfPlaySettings::addAgent(std::string name, const std::string &model_path,
                        ePlayer player) {
  AgentData data;
  data.name = name;
  data.nn_path = model_path;
  data.player = player;
  m_AgentSettings.push_back(data);
}

std::vector<AgentData> SelfPlaySettings::getAgents() { return m_AgentSettings; }

int SelfPlaySettings::getRows() { return m_Rows; }

void SelfPlaySettings::setRows(int rows) { m_Rows = rows; }

int SelfPlaySettings::getCols() { return m_Cols; }

void SelfPlaySettings::setCols(int cols) { m_Cols = cols; }

int SelfPlaySettings::getInputPlanes() { return m_InputPlanes; }

void SelfPlaySettings::setInputPlanes(int inputPlanes) { m_InputPlanes = inputPlanes; }

int SelfPlaySettings::getOutputSize() { return m_Cols; }
