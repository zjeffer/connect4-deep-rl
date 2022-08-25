#include "selfPlaySettings.hpp"

SelfPlaySettings::SelfPlaySettings() {}

SelfPlaySettings::SelfPlaySettings(const std::filesystem::path& settings_path) {
	// TODO: load selfPlaySettings from file
	(void)settings_path; // ignore warning
}

int SelfPlaySettings::getSimulations() const { return m_Simulations; }

void SelfPlaySettings::setSimulations(int simulations) { m_Simulations = simulations; }

bool SelfPlaySettings::isStochastic() const { return m_UseStochasticSearch; }

void SelfPlaySettings::setStochastic(bool stochastic) { m_UseStochasticSearch = stochastic; }

void SelfPlaySettings::addAgent(std::string name, const std::string& model_path, ePlayer player) {
	AgentData data;
	data.name = name;
	data.nn_path = model_path;
	data.player = player;
	m_AgentSettings.push_back(data);
}

const std::vector<AgentData>& SelfPlaySettings::getAgents() const { return m_AgentSettings; }

bool SelfPlaySettings::showMoves() const { return m_ShowMoves; }

void SelfPlaySettings::setShowMoves(bool show_moves) { m_ShowMoves = show_moves; }

bool SelfPlaySettings::saveMemory() const { return m_SaveMemory; }

void SelfPlaySettings::setSaveMemory(bool save_memory) { m_SaveMemory = save_memory; }

int SelfPlaySettings::getPipelineGames() const { return m_PipelineGames; }

void SelfPlaySettings::setPipelineGames(int games) {
	m_PipelineGames = games;
}