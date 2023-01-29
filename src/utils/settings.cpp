#include "settings.hpp"

Settings::Settings() {}

Settings::~Settings() = default;

Settings::Settings(std::filesystem::path const & settings_path)
{
    // TODO: load Settings from file
    (void)settings_path; // ignore warning
}

int Settings::getSimulations() const
{
    return m_Simulations;
}

void Settings::setSimulations(int simulations)
{
    m_Simulations = simulations;
}

bool Settings::isStochastic() const
{
    return m_UseStochasticSearch;
}

void Settings::setStochastic(bool stochastic)
{
    m_UseStochasticSearch = stochastic;
}

bool Settings::showMoves() const
{
    return m_ShowMoves;
}

void Settings::setShowMoves(bool show_moves)
{
    m_ShowMoves = show_moves;
}

bool Settings::saveMemory() const
{
    return m_SaveMemory;
}

void Settings::setSaveMemory(bool save_memory)
{
    m_SaveMemory = save_memory;
}

int Settings::getPipelineGames() const
{
    return m_PipelineGames;
}

void Settings::setPipelineGames(int games)
{
    m_PipelineGames = games;
}

float Settings::getLearningRate() const
{
    return m_LearningRate;
}

void Settings::setLearningRate(float learningRate)
{
    m_LearningRate = learningRate;
}

int Settings::getBatchSize() const
{
    return m_BatchSize;
}

void Settings::setBatchSize(int batchSize)
{
    m_BatchSize = batchSize;
}

int Settings::getEpochs() const
{
    return m_Epochs;
}

void Settings::setEpochs(int epochs)
{
    m_Epochs = epochs;
}

bool Settings::useCUDA() const
{
    return m_useCUDA;
}

void Settings::setuseCUDA(bool useCUDA)
{
    m_useCUDA = useCUDA;
}

std::filesystem::path const & Settings::getMemoryFolder() const
{
    return m_MemoryFolder;
}

void Settings::setMemoryFolder(std::filesystem::path memoryFolder)
{
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

std::filesystem::path Settings::getModelPath() const
{
    return m_ModelPath;
}

void Settings::setModelPath(std::filesystem::path const & model_path)
{
    m_ModelPath = model_path;
}