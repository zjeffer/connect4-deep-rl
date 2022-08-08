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