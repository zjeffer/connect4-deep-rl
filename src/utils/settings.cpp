#include "settings.hpp"

Settings::Settings() {}

Settings::~Settings() = default;

bool Settings::useCUDA() const
{
    return m_useCUDA;
}

void Settings::setuseCUDA(bool useCUDA)
{
    m_useCUDA = useCUDA;
}

std::string const& Settings::getMemoryFolder() const
{
    return m_MemoryFolder;
}

void Settings::setMemoryFolder(std::string memoryFolder)
{
    // TODO: is this even necessary?
    if (!memoryFolder.ends_with('/'))
    {
        memoryFolder += '/';
    }
    m_MemoryFolder = memoryFolder;
}

int Settings::getRows() const
{
    return m_Rows;
}

void Settings::setRows(int rows)
{
    m_Rows = rows;
}

int Settings::getCols() const
{
    return m_Cols;
}

void Settings::setCols(int cols)
{
    m_Cols = cols;
}

int Settings::getInputPlanes() const
{
    return m_InputPlanes;
}

void Settings::setInputPlanes(int inputPlanes)
{
    m_InputPlanes = inputPlanes;
}

int Settings::getOutputSize() const
{
    return m_Cols;
}

std::string Settings::getModelPath() const
{
    return m_ModelPath;
}

void Settings::setModelPath(std::string const& model_path)
{
    m_ModelPath = model_path;
}