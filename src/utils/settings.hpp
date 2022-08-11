#pragma once

#include <string>

class Settings {
public:
  Settings();
  ~Settings();

  [[nodiscard]] bool useGPU();
  void setUseGPU(bool useGPU);

  [[nodiscard]] std::string getMemoryFolder();
  void setMemoryFolder(std::string memoryFolder);

  [[nodiscard]] int getRows();
	void setRows(int rows);

	[[nodiscard]] int getCols();
	void setCols(int cols);

	[[nodiscard]] int getInputPlanes();
	void setInputPlanes(int inputPlanes);

  [[nodiscard]] int getOutputSize();

  [[nodiscard]] std::string getModelPath();
	void setModelPath(const std::string& model_path);

private:
  bool m_UseGPU = true;
  std::string m_MemoryFolder = "memory/";
  std::string m_ModelPath = "models/model.pt";

  int m_Rows = 6;
	int m_Cols = 7;
	int m_InputPlanes = 3;
};