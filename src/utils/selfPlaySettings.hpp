#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include "../connect4/player.hpp"
#include "settings.hpp"
#include "types.hpp"



class SelfPlaySettings : public Settings {
  public:
	SelfPlaySettings();
	SelfPlaySettings(const std::filesystem::path& settings_path);
	~SelfPlaySettings();

	[[nodiscard]] int getSimulations();
	void setSimulations(int simulations);

	[[nodiscard]] bool isStochastic();
	void setStochastic(bool stochastic);

	void addAgent(std::string name, const std::string& model_path, ePlayer player);
	std::vector<AgentData> getAgents();

	bool showMoves();
	void setShowMoves(bool show_moves);

	bool saveMemory();
	void setSaveMemory(bool save_memory);

  private:
	int m_Simulations = 400;
	bool m_UseStochasticSearch = true;
	std::vector<AgentData> m_AgentSettings = std::vector<AgentData>();
	bool m_ShowMoves = false;
	bool m_SaveMemory = true;
};