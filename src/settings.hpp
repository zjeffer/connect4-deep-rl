#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include "connect4/player.hpp"

struct AgentData {
	std::string name;
	std::string nn_path;
	ePlayer player;
};

class Settings {
  public:
	Settings();
	Settings(const std::filesystem::path& settings_path);
	~Settings();

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

	[[nodiscard]] bool useGPU();
	void setUseGPU(bool useGPU);

  private:
	int m_Simulations = 400;
	bool m_UseStochasticSearch = true;
	int m_Rows = 6;
	int m_Cols = 7;
	std::vector<AgentData> m_AgentSettings = std::vector<AgentData>();
	bool m_UseGPU = true;
};