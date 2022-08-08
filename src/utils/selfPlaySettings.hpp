#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include "../connect4/player.hpp"
#include "settings.hpp"

struct AgentData {
	std::string name;
	std::string nn_path;
	ePlayer player;
};

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

	[[nodiscard]] int getRows();
	void setRows(int rows);

	[[nodiscard]] int getCols();
	void setCols(int cols);

	[[nodiscard]] int getInputPlanes();
	void setInputPlanes(int inputPlanes);

	[[nodiscard]] int getOutputSize();

	

  private:
	int m_Simulations = 400;
	bool m_UseStochasticSearch = true;
	std::vector<AgentData> m_AgentSettings = std::vector<AgentData>();
	int m_Rows = 6;
	int m_Cols = 7;
	int m_InputPlanes = 3;
};