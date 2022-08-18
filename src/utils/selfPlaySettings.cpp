#include "selfPlaySettings.hpp"

SelfPlaySettings::SelfPlaySettings() {}

/* SelfPlaySettings::SelfPlaySettings(const std::filesystem::path &settings_path) {
  // TODO: load selfPlaySettings from file
} */

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

bool SelfPlaySettings::showMoves() { return m_ShowMoves; }

void SelfPlaySettings::setShowMoves(bool show_moves) { m_ShowMoves = show_moves; }

bool SelfPlaySettings::saveMemory() { return m_SaveMemory; }

void SelfPlaySettings::setSaveMemory(bool save_memory) { m_SaveMemory = save_memory; }

