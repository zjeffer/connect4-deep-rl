#pragma once

#include <string>

class Settings {
public:
  Settings();
  virtual ~Settings();

  [[nodiscard]] bool useCUDA() const;
  void setuseCUDA(bool useCUDA);

  [[nodiscard]] const std::string& getMemoryFolder() const;
  void setMemoryFolder(std::string memoryFolder);

  [[nodiscard]] int getRows() const;
	void setRows(int rows);

	[[nodiscard]] int getCols() const;
	void setCols(int cols);

	[[nodiscard]] int getInputPlanes() const;
	void setInputPlanes(int inputPlanes);

  [[nodiscard]] int getOutputSize() const;

  [[nodiscard]] std::string getModelPath() const;
	void setModelPath(const std::string& model_path);

private:
  bool m_useCUDA = true;
  std::string m_MemoryFolder = "memory";
  std::string m_ModelPath = "models/model.pt";

  int m_Rows = 6;
	int m_Cols = 7;
	int m_InputPlanes = 3;
};