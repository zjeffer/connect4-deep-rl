#pragma once

#include "settings.hpp"

class TrainerSettings : public Settings
{
  public:
    TrainerSettings();

    float getLearningRate() const;
    void  setLearningRate(float learningRate);

    int  getBatchSize() const;
    void setBatchSize(int batchSize);

    int  getEpochs() const;
    void setEpochs(int epochs);

  private:
    float m_LearningRate = 0.02f;
    int   m_BatchSize    = 64;
    int   m_Epochs       = 10;
};