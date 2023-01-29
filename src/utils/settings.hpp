#pragma once

#include <filesystem>
#include <string>

#include "../connect4/player.hpp"
#include "types.hpp"

class Settings
{
  public:
    Settings();

    /**
     * @brief Load settings based on a settings file.
     * TODO: implement this.
     * @param settings_path: path to the file
     */
    Settings(std::filesystem::path const & settings_path);

    virtual ~Settings();

    int  getSimulations() const;
    void setSimulations(int simulations);

    bool isStochastic() const;
    void setStochastic(bool stochastic);

    bool showMoves() const;
    void setShowMoves(bool show_moves);

    bool saveMemory() const;
    void setSaveMemory(bool save_memory);

    int  getPipelineGames() const;
    void setPipelineGames(int games);

    float getLearningRate() const;
    void  setLearningRate(float learningRate);

    int  getBatchSize() const;
    void setBatchSize(int batchSize);

    int  getEpochs() const;
    void setEpochs(int epochs);

    bool useCUDA() const;
    void setuseCUDA(bool useCUDA);

    std::filesystem::path const & getMemoryFolder() const;
    void                          setMemoryFolder(std::filesystem::path memoryFolder);

    int  getRows() const;
    void setRows(int rows);

    int  getCols() const;
    void setCols(int cols);

    int  getInputPlanes() const;
    void setInputPlanes(int inputPlanes);

    int getOutputSize() const;

    std::filesystem::path getModelPath() const;
    void                  setModelPath(std::filesystem::path const & model_path);

  private:
    int                   m_Simulations         = 200;
    bool                  m_UseStochasticSearch = true;
    bool                  m_ShowMoves           = false;
    bool                  m_SaveMemory          = true;
    int                   m_PipelineGames       = 50;
    bool                  m_useCUDA             = true;
    std::filesystem::path m_MemoryFolder        = "memory";
    std::filesystem::path m_ModelPath           = "models/model.pt";

    float m_LearningRate = 0.02f;
    int   m_BatchSize    = 64;
    int   m_Epochs       = 10;

    int m_Rows        = 6;
    int m_Cols        = 7;
    int m_InputPlanes = 3;
};