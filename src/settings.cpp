#include "settings.hpp"

Settings::Settings() {}

Settings::Settings(const std::filesystem::path &settings_path) {
  // TODO: load settings from file
}

Settings::~Settings() {}

int Settings::getSimulations() { return m_Simulations; }

void Settings::setSimulations(int simulations) { m_Simulations = simulations; }

bool Settings::isStochastic() { return m_UseStochasticSearch; }

void Settings::setStochastic(bool stochastic) {
  m_UseStochasticSearch = stochastic;
}

void Settings::addAgent(std::string name, const std::string &model_path,
                        ePlayer player) {
  AgentData data;
  data.name = name;
  data.nn_path = model_path;
  data.player = player;
  m_AgentSettings.push_back(data);
}

std::vector<AgentData> Settings::getAgents() { return m_AgentSettings; }

int Settings::getRows() { return m_Rows; }

void Settings::setRows(int rows) { m_Rows = rows; }

int Settings::getCols() { return m_Cols; }

void Settings::setCols(int cols) { m_Cols = cols; }

bool Settings::useGPU() { return m_UseGPU; }

void Settings::setUseGPU(bool useGPU) { m_UseGPU = useGPU; }

int Settings::getInputPlanes() { return m_InputPlanes; }

void Settings::setInputPlanes(int inputPlanes) { m_InputPlanes = inputPlanes; }

int Settings::getOutputSize() { return m_Cols; }