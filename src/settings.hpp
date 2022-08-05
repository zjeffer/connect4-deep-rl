#pragma once

#include <string>

class Settings {
  public:
	Settings();
	~Settings();

	[[nodiscard]] int getSimulations();
	void setSimulations(int simulations);

	[[nodiscard]] bool isStochastic();
	void setStochastic(bool stochastic);

	[[nodiscard]] std::string getModelPath();
	void setModelPath(std::string modelPath);

	[[nodiscard]] int getRows();
	void setRows(int rows);

	[[nodiscard]] int getCols();
	void setCols(int cols);

  private:
	int m_Simulations = 400;
	bool m_UseStochasticSearch = true;
	std::string m_ModelPath = "./models/model.pt";
	int m_Rows = 6;
	int m_Cols = 7;
};