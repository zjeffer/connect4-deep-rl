#include "settings.hpp"

Settings::Settings() {}

Settings::~Settings() {}

bool Settings::useGPU() { return m_UseGPU; }

void Settings::setUseGPU(bool useGPU) { m_UseGPU = useGPU; }

std::string Settings::getMemoryFolder() { return m_MemoryFolder; }

void Settings::setMemoryFolder(std::string memoryFolder) {
  if (!memoryFolder.ends_with('/')) {
    memoryFolder += '/';
  }
  m_MemoryFolder = memoryFolder;
}

int Settings::getRows() { return m_Rows; }

void Settings::setRows(int rows) { m_Rows = rows; }

int Settings::getCols() { return m_Cols; }

void Settings::setCols(int cols) { m_Cols = cols; }

int Settings::getInputPlanes() { return m_InputPlanes; }

void Settings::setInputPlanes(int inputPlanes) { m_InputPlanes = inputPlanes; }

int Settings::getOutputSize() { return m_Cols; }

std::string Settings::getModelPath() { return m_ModelPath; }

void Settings::setModelPath(const std::string& model_path) { m_ModelPath = model_path; }