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

private:
  bool m_UseGPU = true;
  std::string m_MemoryFolder = "memory/";
};