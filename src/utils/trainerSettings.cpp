#include "trainerSettings.hpp"

TrainerSettings::TrainerSettings() {}

float TrainerSettings::getLearningRate() const
{
    return m_LearningRate;
}

void TrainerSettings::setLearningRate(float learningRate)
{
    m_LearningRate = learningRate;
}

int TrainerSettings::getBatchSize() const
{
    return m_BatchSize;
}

void TrainerSettings::setBatchSize(int batchSize)
{
    m_BatchSize = batchSize;
}

int TrainerSettings::getEpochs() const
{
    return m_Epochs;
}

void TrainerSettings::setEpochs(int epochs)
{
    m_Epochs = epochs;
}